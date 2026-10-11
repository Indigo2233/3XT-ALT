[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^\d+\.\d+\.\d+([-.][0-9A-Za-z.-]+)?$')]
    [string]$Version,

    [string]$Repository = 'Indigo2233/3XT-ALT',

    [string]$ReleaseTag,

    [switch]$ConfirmPublicRelease
)

$ErrorActionPreference = 'Stop'
$moduleRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if ([string]::IsNullOrWhiteSpace($ReleaseTag)) {
    $ReleaseTag = "v$Version-local"
}
if (-not $ConfirmPublicRelease) {
    throw 'Publishing requires an explicit -ConfirmPublicRelease confirmation.'
}

$archiveName = "XTAlt-$Version-local-windows-x64.zip"
$archive = Join-Path $moduleRoot "release\$archiveName"
$checksums = Join-Path $moduleRoot 'release\SHA256SUMS-local.txt'
foreach ($path in @($archive, $checksums)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required release artifact was not found: $path"
    }
}

$checksumLine = Get-Content -LiteralPath $checksums -Raw
$sha256 = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
if ($checksumLine -notmatch ('^' + [regex]::Escape($sha256) + '\s+' + [regex]::Escape($archiveName) + '\s*$')) {
    throw 'The release archive SHA-256 does not match SHA256SUMS-local.txt.'
}

& gh auth status | Out-Host
if ($LASTEXITCODE -ne 0) {
    throw 'GitHub CLI is not authenticated.'
}

$notes = @"
3XT-ALT $Version Local Signing Edition for PixInsight 1.9.x on Windows x64.

This package contains BXT-ALT, SXT-ALT and NXT-ALT, the self-contained inference runtime, and the project-owned InDeConv, InDeNoise and InDeStars models.

Each user must sign XTAlt-pxm.dll with their own PixInsight Local Signing Identity before installation. Download the ZIP and SHA256SUMS-local.txt, verify the checksum, then follow AGENTS.md or docs/INSTALL.zh-CN.md in the package.

This release cannot be installed through Manage Repositories because the module does not carry a Certified PixInsight Developer signature.
"@

& gh release view $ReleaseTag --repo $Repository *> $null
if ($LASTEXITCODE -eq 0) {
    & gh release upload $ReleaseTag $archive $checksums --repo $Repository --clobber
    & gh release edit $ReleaseTag --repo $Repository --title "3XT-ALT $Version Local Signing Edition" --notes $notes
}
else {
    & gh release create $ReleaseTag $archive $checksums --repo $Repository --title "3XT-ALT $Version Local Signing Edition" --notes $notes
}
if ($LASTEXITCODE -ne 0) {
    throw 'GitHub Release publication failed.'
}

Write-Host "Published GitHub Release: https://github.com/$Repository/releases/tag/$ReleaseTag"
