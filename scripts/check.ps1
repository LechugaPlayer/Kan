#requires -Version 5.1
<#
.SYNOPSIS
    Compile / lint / sanitize gate for Kan.

.DESCRIPTION
    Default      compile every entry in compile_commands.json with the project's
                 strict warning set plus -Werror (syntax only, no output files).
    -Tidy        also run clang-tidy (config: .clang-tidy) over those units.
    -Asan        build tests/smoke.cpp and includes/Lin/src/lin.cpp with MSVC
                 AddressSanitizer and run the result.

                 MSVC is used for ASan because MSYS2's compiler-rt package ships
                 no ASan/UBSan runtime for the x86_64-w64-mingw32 target, so
                 `clang++ -fsanitize=address` cannot link. The ASan build is a
                 separate configuration (MSVC ABI); never mix its objects with
                 MinGW/LLVM ones.

.EXAMPLE
    ./scripts/check.ps1
    ./scripts/check.ps1 -All
#>
[CmdletBinding()]
param(
    [switch]$Tidy,
    [switch]$Asan,
    [switch]$All
)

$ErrorActionPreference = 'Stop'

$root      = Split-Path -Parent $PSScriptRoot
$dbPath    = Join-Path $root 'compile_commands.json'
$clang     = 'C:/msys64/mingw64/bin/clang++'
$clangTidy = 'C:/msys64/mingw64/bin/clang-tidy.exe'

if ($All) { $Tidy = $true; $Asan = $true }

if (-not (Test-Path $dbPath)) { throw "missing compile database: $dbPath" }

# PS 5.1 hands back a top-level JSON array as one non-enumerated object, so
# flatten it explicitly instead of trusting @(...).
$db = Get-Content $dbPath -Raw | ConvertFrom-Json
$entries = New-Object System.Collections.Generic.List[object]
foreach ($item in $db) { $entries.Add($item) }

# Compiler flags from a database entry, minus the driver, -c, -o and the file.
function Get-Flags($entry) {
    $argsIn = @($entry.arguments)
    $flags  = New-Object System.Collections.Generic.List[string]
    $i      = 1
    while ($i -lt $argsIn.Count) {
        $arg = $argsIn[$i]
        if ($arg -eq '-o')         { $i += 2; continue }
        if ($arg -eq '-c')         { $i += 1; continue }
        if ($arg -eq $entry.file)  { $i += 1; continue }
        $flags.Add($arg)
        $i += 1
    }
    return ,$flags.ToArray()
}

$failed = New-Object System.Collections.Generic.List[string]

Write-Host '== compile gate (clang++, -Werror) ==' -ForegroundColor Cyan
foreach ($entry in $entries) {
    $file = Join-Path $root $entry.file
    Write-Host "   $($entry.file)"
    $flags = Get-Flags $entry
    & $clang -fsyntax-only -Werror @flags $file
    if ($LASTEXITCODE -ne 0) { $failed.Add("compile: $($entry.file)") }
}

if ($Tidy) {
    Write-Host '== clang-tidy ==' -ForegroundColor Cyan
    foreach ($entry in $entries) {
        Write-Host "   $($entry.file)"
        & $clangTidy -p $root --quiet (Join-Path $root $entry.file)
        if ($LASTEXITCODE -ne 0) { $failed.Add("tidy: $($entry.file)") }
    }
}

if ($Asan) {
    Write-Host '== AddressSanitizer (MSVC) ==' -ForegroundColor Cyan

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) { throw 'vswhere.exe not found - is Visual Studio installed?' }
    $vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vs) { throw 'no MSVC C++ toolset found' }
    $vcvars = Join-Path $vs 'VC\Auxiliary\Build\vcvars64.bat'

    $out = Join-Path $root 'build\asan'
    New-Item -ItemType Directory -Force -Path $out | Out-Null

    $sources = @(
        (Join-Path $root 'tests\smoke.cpp'),
        (Join-Path $root 'includes\Lin\src\lin.cpp')
    )
    $quoted = ($sources | ForEach-Object { '"' + $_ + '"' }) -join ' '

    $build = 'call "{0}" >nul && cl /nologo /std:c++latest /EHsc /Zi /O1 /W4 /fsanitize=address ' +
             '/I"{1}\src" /I"{1}\includes\Lin\src" /Fd"{2}\vc.pdb" /Fo"{2}\\" {3} /Fe"{2}\kan_asan.exe"'
    $build = $build -f $vcvars, $root, $out, $quoted

    cmd /c $build
    if ($LASTEXITCODE -ne 0) {
        $failed.Add('asan: build')
    }
    else {
        $run = 'call "{0}" >nul && "{1}\kan_asan.exe"' -f $vcvars, $out
        cmd /c $run
        if ($LASTEXITCODE -ne 0) { $failed.Add('asan: run') }
    }
}

Write-Host ''
if ($failed.Count -gt 0) {
    Write-Host ("FAILED: " + ($failed -join ', ')) -ForegroundColor Red
    exit 1
}
Write-Host 'all gates passed' -ForegroundColor Green
exit 0
