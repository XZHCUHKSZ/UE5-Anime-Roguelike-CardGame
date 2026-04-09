Param(
    [string]$VsInstallPath = "D:\Microsoft VisualStudio",
    [switch]$InstallEpicLauncher,
    [switch]$InstallVsGameWorkload
)

$ErrorActionPreference = "Stop"

function Write-Step {
    param([string]$Msg)
    Write-Host "`n=== $Msg ===" -ForegroundColor Cyan
}

function Test-CommandExists {
    param([string]$CommandName)
    return [bool](Get-Command $CommandName -ErrorAction SilentlyContinue)
}

Write-Step "System Check"
Write-Host "OS: $([System.Environment]::OSVersion.VersionString)"
Write-Host "PowerShell: $($PSVersionTable.PSVersion)"

$drive = Get-PSDrive -Name D -ErrorAction SilentlyContinue
if ($drive) {
    $freeGB = [math]::Round($drive.Free / 1GB, 2)
    Write-Host "D: Free Space: ${freeGB} GB"
} else {
    Write-Host "D: drive not found. UE5 recommends > 150GB free on install drive."
}

Write-Step "Git Check"
if (Test-CommandExists "git") {
    git --version
} else {
    Write-Warning "git not found."
}

Write-Step "Winget Check"
if (Test-CommandExists "winget") {
    winget --version
} else {
    throw "winget not found. Please install App Installer from Microsoft Store."
}

Write-Step "Epic Launcher Check"
$launcherCandidates = @(
    "C:\Program Files\Epic Games\Launcher\Engine\Binaries\Win64\EpicGamesLauncher.exe",
    "C:\Program Files\Epic Games\Launcher\Portal\Binaries\Win64\EpicGamesLauncher.exe"
)
$launcherPath = $launcherCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if ($launcherPath) {
    Write-Host "Epic Launcher found: $launcherPath"
} else {
    Write-Warning "Epic Launcher not found."
    if ($InstallEpicLauncher) {
        Write-Host "Installing Epic Launcher with winget..."
        winget install --id EpicGames.EpicGamesLauncher --accept-package-agreements --accept-source-agreements --silent
    } else {
        Write-Host "Run with -InstallEpicLauncher to auto install."
    }
}

Write-Step "Visual Studio Check"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "vswhere not found. Please install Visual Studio 2022 Community first."
}

$vsPath = & $vswhere -latest -products * -property installationPath
if (-not $vsPath) {
    throw "Visual Studio installation not found."
}
Write-Host "Visual Studio found: $vsPath"

$hasNativeGame = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Workload.NativeGame -property installationPath
if ($hasNativeGame) {
    Write-Host "Native Game workload is installed."
} else {
    Write-Warning "Native Game workload is NOT installed."
    if ($InstallVsGameWorkload) {
        $setupExe = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\setup.exe"
        if (-not (Test-Path $setupExe)) {
            throw "VS setup.exe not found."
        }

        Write-Host "Installing Native Game workload..."
        & $setupExe modify `
            --installPath $vsPath `
            --productId Microsoft.VisualStudio.Product.Community `
            --channelId VisualStudio.17.Release `
            --add Microsoft.VisualStudio.Workload.NativeGame `
            --includeRecommended `
            --passive `
            --norestart

        Write-Host "VS installer command finished. Re-checking..."
        $hasNativeGame2 = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Workload.NativeGame -property installationPath
        if ($hasNativeGame2) {
            Write-Host "Native Game workload installed successfully."
        } else {
            Write-Warning "Workload may still be pending or installer needs manual confirmation."
            Write-Host "Open Visual Studio Installer and add: 'Game development with C++'."
        }
    } else {
        Write-Host "Run with -InstallVsGameWorkload to try auto install."
    }
}

Write-Step "Next Actions"
Write-Host "1) Launch Epic Games Launcher and sign in."
Write-Host "2) Install Unreal Engine 5.6 (or latest UE5 LTS preference)."
Write-Host "3) In UE installer options, include C++ source and symbols if disk allows."
Write-Host "4) Create C++ project and point to this repo folder."
