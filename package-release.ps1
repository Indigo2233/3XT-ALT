[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^\d+\.\d+\.\d+([-.][0-9A-Za-z.-]+)?$')]
    [string]$Version,

    [string]$DllPath,

    [Parameter(Mandatory)]
    [string]$SignaturePath,

    [switch]$CpdSigned
)

$ErrorActionPreference = 'Stop'
$moduleRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

if ([string]::IsNullOrWhiteSpace($DllPath)) {
    $DllPath = Join-Path $moduleRoot 'build\bin\XTAlt-pxm.dll'
}

if (-not $CpdSigned) {
    throw 'Public release packaging requires an explicit -CpdSigned confirmation.'
}

foreach ($path in @($DllPath, $SignaturePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required release file was not found: $path"
    }
}

if ([IO.Path]::GetExtension($SignaturePath) -ne '.xsgn') {
    throw 'SignaturePath must identify an .xsgn file.'
}

$signatureText = Get-Content -LiteralPath $SignaturePath -Raw
if ($signatureText -notmatch '<Signature\s+version="[^"]+"\s+developerId="[^"]+">') {
    throw 'The supplied file does not contain a recognizable PixInsight code signature.'
}

$releaseRoot = Join-Path $moduleRoot 'release'
$packageName = "XTAlt-$Version-windows-x64"
$stage = Join-Path $releaseRoot $packageName
$archive = Join-Path $releaseRoot ($packageName + '.zip')
$checksums = Join-Path $releaseRoot 'SHA256SUMS.txt'

$releaseRootFull = [IO.Path]::GetFullPath($releaseRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$stageFull = [IO.Path]::GetFullPath($stage)
if (-not $stageFull.StartsWith($releaseRootFull + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'The release staging directory resolved outside the repository release directory.'
}

if (Test-Path -LiteralPath $stageFull) {
    Remove-Item -LiteralPath $stageFull -Recurse -Force
}
New-Item -ItemType Directory -Path $stageFull -Force | Out-Null

Copy-Item -LiteralPath $DllPath -Destination (Join-Path $stageFull 'XTAlt-pxm.dll')
Copy-Item -LiteralPath $SignaturePath -Destination (Join-Path $stageFull 'XTAlt-pxm.xsgn')
Copy-Item -LiteralPath (Join-Path $moduleRoot 'docs\INSTALL.zh-CN.md') -Destination (Join-Path $stageFull 'INSTALL.zh-CN.md')
Copy-Item -LiteralPath (Join-Path $moduleRoot 'NOTICE.md') -Destination (Join-Path $stageFull 'NOTICE.md')

if (Test-Path -LiteralPath $archive) {
    Remove-Item -LiteralPath $archive -Force
}
Compress-Archive -Path (Join-Path $stageFull '*') -DestinationPath $archive -CompressionLevel Optimal

$hash = Get-FileHash -LiteralPath $archive -Algorithm SHA256
Set-Content -LiteralPath $checksums -Encoding ASCII -Value ("{0}  {1}" -f $hash.Hash.ToLowerInvariant(), (Split-Path -Leaf $archive))

Write-Host "Release archive: $archive"
Write-Host "Checksums: $checksums"
