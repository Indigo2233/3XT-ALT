# Tests use disposable files only. No PixInsight UI, keys, network or installed files.
$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent $PSScriptRoot
. (Join-Path $repository 'installer-common.ps1')

function Expect-Failure {
    param([scriptblock]$Action, [string]$Expected)
    $caught = $false
    try { & $Action }
    catch {
        $caught = $true
        if ($_.Exception.Message -notlike "*$Expected*") { throw }
    }
    if (-not $caught) { throw "Expected failure containing: $Expected" }
}

# Parse all installer/packaging scripts with the actual interpreter running this test.
foreach ($file in Get-ChildItem -LiteralPath $repository -Filter '*.ps1' -File) {
    $tokens = $null
    $errors = $null
    [Management.Automation.Language.Parser]::ParseFile($file.FullName, [ref]$tokens, [ref]$errors) | Out-Null
    if ($errors.Count -gt 0) { throw ($errors | Out-String) }
}

$hash = 'a' * 64
$text = "$hash  complete.zip`r`n$('b' * 64)  setup.zip`r`n"
if ((Get-XTAltExpectedHash $text 'complete.zip') -cne $hash) { throw 'Wrong checksum selected.' }
Expect-Failure { Get-XTAltExpectedHash $text 'missing.zip' } 'exactly one'
Expect-Failure { Get-XTAltExpectedHash ($text + "$hash  complete.zip") 'complete.zip' } 'exactly one'

$fixture = Join-Path ([IO.Path]::GetTempPath()) ('xtalt-installer-test-' + [guid]::NewGuid().ToString('N'))
$package = Join-Path $fixture 'package with spaces'
New-Item -ItemType Directory -Path $package -Force | Out-Null
try {
    $files = @('XTAlt-pxm.dll', 'install-local.ps1', 'installer-common.ps1',
        'XTAltBackend/runtime/python.exe', 'XTAltBackend/engine/run_models.py',
        'XTAltBackend/tools/xt_alt_worker.py', 'XTAltBackend/engine/models/InDeConv.onnx',
        'XTAltBackend/engine/models/InDeNoise.onnx', 'XTAltBackend/engine/models/InDeStars.onnx',
        'test/native-selftest.js')
    $entries = @()
    foreach ($relative in $files) {
        $path = Get-XTAltChildPath $package $relative
        New-Item -ItemType Directory -Path (Split-Path -Parent $path) -Force | Out-Null
        [IO.File]::WriteAllText($path, 'disposable installer test fixture')
        $entries += @{ path = $relative; sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
    }
    $manifest = @{ schema = 1; platform = 'windows-x64'; files = $entries }
    $manifestPath = Join-Path $package 'package-manifest.json'
    $manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding UTF8
    Test-XTAltPackage $package
    Push-Location $fixture
    try {
        Test-XTAltPackage '.\package with spaces'
        if ((Get-XTAltChildPath '.\package with spaces' 'XTAlt-pxm.dll') -ine (Join-Path $package 'XTAlt-pxm.dll')) {
            throw 'Relative package path resolved outside the PowerShell location.'
        }
    }
    finally { Pop-Location }
    $extraFile = Join-Path $package 'unexpected.py'
    [IO.File]::WriteAllText($extraFile, 'unexpected')
    Expect-Failure { Test-XTAltPackage $package } 'Unlisted package file'
    Remove-Item -LiteralPath $extraFile
    $tampered = Join-Path $package 'XTAltBackend/engine/models/InDeConv.onnx'
    [IO.File]::WriteAllText($tampered, 'tampered')
    Expect-Failure { Test-XTAltPackage $package } 'checksum mismatch'
    [IO.File]::WriteAllText($tampered, 'disposable installer test fixture')
    $manifest.files = $entries[1..($entries.Count - 1)]
    $manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding UTF8
    Expect-Failure { Test-XTAltPackage $package } 'omits a required file'
    foreach ($bad in @('..\escape', 'C:\escape', '/absolute', 'dir/../../escape', 'file:stream')) {
        Expect-Failure { Get-XTAltChildPath $package $bad } 'Invalid package path'
    }
    Assert-XTAltDestination (Join-Path $fixture 'install') $package 'C:\Program Files\PixInsight'
    Expect-Failure { Assert-XTAltDestination $package $package 'C:\Program Files\PixInsight' } 'protected'
    Expect-Failure { Assert-XTAltDestination $fixture $package 'C:\Program Files\PixInsight' } 'protected'
    Expect-Failure { Assert-XTAltDestination (Join-Path $package 'nested') $package '' } 'outside'
    Expect-Failure { Assert-XTAltDestination 'C:\' $package '' } 'dedicated'
    $unknown = Join-Path $fixture 'unrelated'
    New-Item -ItemType Directory -Path $unknown | Out-Null
    [IO.File]::WriteAllText((Join-Path $unknown 'user-data.txt'), 'preserve')
    Expect-Failure { Assert-XTAltDestination $unknown $package '' } 'nonempty'
    Expect-Failure { Get-XTAltPEMachine $tampered } 'Invalid Windows executable'
    Write-Host "Installer smoke tests passed (PowerShell $($PSVersionTable.PSVersion))."
}
finally {
    $resolved = [IO.Path]::GetFullPath($fixture)
    $tempRoot = [IO.Path]::GetFullPath([IO.Path]::GetTempPath()).TrimEnd('\') + '\'
    if ($resolved.StartsWith($tempRoot, [StringComparison]::OrdinalIgnoreCase) -and
        (Split-Path -Leaf $resolved) -like 'xtalt-installer-test-*') {
        Remove-Item -LiteralPath $resolved -Recurse -Force
    }
}
