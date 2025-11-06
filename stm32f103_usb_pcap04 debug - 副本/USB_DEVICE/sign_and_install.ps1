<#
One-click: Create CAT, make/import test code-signing cert, sign, and install INF
Requirements:
  - Run in elevated PowerShell (Admin)
  - Windows 10/11 with Windows SDK/WDK tools in PATH (inf2cat, signtool)
This script targets the INF in the same folder as this script.
#>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

Write-Host "=== PCAP04 CDC test-signed driver installation ===" -ForegroundColor Cyan

# 1) Paths
$DriverDir = $PSScriptRoot
$InfPath   = Join-Path $DriverDir "pcap04_cdc.inf"
$CatPath   = [System.IO.Path]::ChangeExtension($InfPath, ".cat")
$Cn        = "YourCompany Test Signing"
$CerOut    = Join-Path $env:TEMP "yourcompany_test.cer"

if (-not (Test-Path $InfPath)) { throw "INF not found: $InfPath" }

# 2) Tool checks (auto-locate Windows Kits tools if not in PATH)
function Add-ToolToPathIfFound($exeName) {
  if (Get-Command $exeName -ErrorAction SilentlyContinue) { return }
  $candidates = @(
    "C:\\Program Files (x86)\\Windows Kits\\10\\bin",
    "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin",
    "C:\\Program Files\\Windows Kits\\10\\bin"
  )
  foreach ($root in $candidates) {
    if (-not (Test-Path $root)) { continue }
    # Pick highest version dir then prefer x64
    $versionDirs = Get-ChildItem -Path $root -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending
    foreach ($ver in $versionDirs) {
      foreach ($arch in @('x64','x86')) {
        $full = Join-Path (Join-Path $ver.FullName $arch) $exeName
        if (Test-Path $full) {
          $env:Path = (Split-Path $full) + ";" + $env:Path
          return
        }
      }
    }
    # Some kits place tools directly under bin\\x64
    foreach ($arch in @('x64','x86')) {
      $full = Join-Path (Join-Path $root $arch) $exeName
      if (Test-Path $full) {
        $env:Path = (Split-Path $full) + ";" + $env:Path
        return
      }
    }
  }
}

Add-ToolToPathIfFound 'inf2cat.exe'
Add-ToolToPathIfFound 'signtool.exe'

foreach ($tool in @('inf2cat','signtool','pnputil')) {
  if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
    throw "Required tool not found in PATH: $tool"
  }
}

# 3) Generate CAT (Win10 x64 compatible; Win11 accepts 10_X64)
Write-Host "[1/5] Generating CAT via inf2cat..." -ForegroundColor Yellow
& inf2cat /driver:"$DriverDir" /os:10_X64 | Write-Host
if (-not (Test-Path $CatPath)) { throw "CAT not generated: $CatPath" }

# 4) Create or reuse a local machine Code Signing cert
Write-Host "[2/5] Creating or locating code-signing certificate..." -ForegroundColor Yellow
$cert = Get-ChildItem Cert:\LocalMachine\My | Where-Object { $_.Subject -eq "CN=$Cn" -and $_.HasPrivateKey }
if (-not $cert) {
  $cert = New-SelfSignedCertificate `
    -Type CodeSigningCert `
    -Subject "CN=$Cn" `
    -KeyUsage DigitalSignature `
    -KeyExportPolicy Exportable `
    -CertStoreLocation "Cert:\LocalMachine\My"
}

# 5) Trust the cert (Root + TrustedPublisher)
Write-Host "[3/5] Trusting certificate (Root & TrustedPublisher)..." -ForegroundColor Yellow
Export-Certificate -Cert $cert -FilePath $CerOut | Out-Null
Import-Certificate -FilePath $CerOut -CertStoreLocation "Cert:\LocalMachine\Root" | Out-Null
Import-Certificate -FilePath $CerOut -CertStoreLocation "Cert:\LocalMachine\TrustedPublisher" | Out-Null

# 6) Sign CAT (timestamped)
Write-Host "[4/5] Signing CAT..." -ForegroundColor Yellow
& signtool sign `
  /fd SHA256 `
  /td SHA256 `
  /tr http://timestamp.digicert.com `
  /sha1 $cert.Thumbprint `
  "$CatPath" | Write-Host

# 7) Install driver (binds to usbser.sys)
Write-Host "[5/5] Installing INF via pnputil..." -ForegroundColor Yellow
& pnputil /add-driver "$InfPath" /install | Write-Host

Write-Host "=== Done. If device was previously installed, uninstall the old instance (check 'Delete the driver software for this device') and replug. ===" -ForegroundColor Green

