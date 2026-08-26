#Requires -Version 7.0
<#
.SYNOPSIS
Build and flash the STM32 firmware from native PowerShell 7.

.EXAMPLE
./runbuild_and_flash.ps1
Build the Debug preset and flash it.

.EXAMPLE
./runbuild_and_flash.ps1 -BuildOnly
Build without flashing.

.EXAMPLE
./runbuild_and_flash.ps1 -FlashOnly
Flash the existing Debug image.

.EXAMPLE
./runbuild_and_flash.ps1 -Preset Release -BuildOnly
Build the Release preset without flashing.
#>
[CmdletBinding()]
param(
    [Alias('c')]
    [switch]$Clean,

    [Alias('r')]
    [switch]$Reconfigure,

    [Alias('b')]
    [switch]$BuildOnly,

    [Alias('f')]
    [switch]$FlashOnly,

    [switch]$ForceErase,

    [Alias('v')]
    [switch]$Verify,

    [switch]$NoReset,

    [Alias('d')]
    [string]$Device = 'STM32H743VI',

    [Alias('s')]
    [ValidateRange(1, 50000)]
    [uint32]$Speed = 4000U,

    [Alias('p')]
    [ValidateNotNullOrEmpty()]
    [string]$Preset = 'Debug',

    [string]$HexFile,

    [string[]]$CMakeArgs = @(),

    [string[]]$JLinkArgs = @(),

    [ValidateRange(10, 600)]
    [uint32]$JLinkTimeoutSeconds = 60U
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Write-Info {
    param([Parameter(Mandatory)][string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Blue
}

function Write-Success {
    param([Parameter(Mandatory)][string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor Green
}

function Invoke-LoggedCommand {
    param(
        [Parameter(Mandatory)][string]$FilePath,
        [Parameter(Mandatory)][string[]]$CommandArgs,
        [Parameter(Mandatory)][string]$LogPath,
        [uint32]$TimeoutSeconds = 0U
    )

    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $FilePath
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    foreach($argument in $CommandArgs)
    {
        $startInfo.ArgumentList.Add($argument)
    }

    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    try
    {
        if(-not $process.Start())
        {
            throw "Failed to start command: $FilePath"
        }
        $standardOutputTask = $process.StandardOutput.ReadToEndAsync()
        $standardErrorTask = $process.StandardError.ReadToEndAsync()

        $completed = if(0U -eq $TimeoutSeconds)
        {
            $process.WaitForExit()
            $true
        }
        else
        {
            $process.WaitForExit([int]($TimeoutSeconds * 1000U))
        }
        if(-not $completed)
        {
            $process.Kill($true)
            $process.WaitForExit()
        }

        $standardOutput = $standardOutputTask.GetAwaiter().GetResult()
        $standardError = $standardErrorTask.GetAwaiter().GetResult()
        Set-Content -LiteralPath $LogPath -Value $standardOutput -Encoding utf8
        if(-not [string]::IsNullOrEmpty($standardError))
        {
            Add-Content -LiteralPath $LogPath -Value $standardError `
                -Encoding utf8
        }
        Get-Content -LiteralPath $LogPath

        if(-not $completed)
        {
            throw "Command timed out after $TimeoutSeconds seconds: $FilePath"
        }
        if(0 -ne $process.ExitCode)
        {
            throw "Command failed with exit code $($process.ExitCode): $FilePath"
        }
    }
    finally
    {
        $process.Dispose()
    }
}

function Find-JLinkCommander {
    foreach($commandName in @('JLink.exe', 'JLinkExe', 'jlink.exe'))
    {
        $command = Get-Command $commandName -ErrorAction SilentlyContinue
        if($null -ne $command)
        {
            return $command.Source
        }
    }
    throw 'J-Link Commander was not found in PATH.'
}

if($FlashOnly -and $BuildOnly)
{
    throw '-FlashOnly and -BuildOnly cannot be used together.'
}
if($FlashOnly -and $Clean)
{
    throw '-FlashOnly and -Clean cannot be used together.'
}

$projectRoot = [System.IO.Path]::GetFullPath($PSScriptRoot)
$buildRoot = [System.IO.Path]::GetFullPath(
    (Join-Path -Path $projectRoot -ChildPath 'build'))
$buildDirectory = [System.IO.Path]::GetFullPath(
    (Join-Path -Path $buildRoot -ChildPath $Preset))
if((Split-Path -Path $buildDirectory -Parent) -ne $buildRoot)
{
    throw "Preset resolves outside the build directory: $Preset"
}

if([string]::IsNullOrWhiteSpace($HexFile))
{
    $hexPath = Join-Path -Path $buildDirectory `
        -ChildPath 'cmake-clang-h743vi.hex'
}
elseif([System.IO.Path]::IsPathRooted($HexFile))
{
    $hexPath = [System.IO.Path]::GetFullPath($HexFile)
}
else
{
    $hexPath = [System.IO.Path]::GetFullPath(
        (Join-Path -Path $projectRoot -ChildPath $HexFile))
}
$binaryPath = [System.IO.Path]::ChangeExtension($hexPath, '.bin')

New-Item -ItemType Directory -Path $buildRoot -Force | Out-Null
Push-Location $projectRoot
try
{
    if($Clean)
    {
        $BuildOnly = $true
        Write-Info "Clean mode: removing $buildDirectory and rebuilding"
        if(Test-Path -LiteralPath $buildDirectory)
        {
            Remove-Item -LiteralPath $buildDirectory -Recurse -Force
        }
    }

    if(-not $FlashOnly)
    {
        $cmakeCommand = (Get-Command 'cmake.exe' -ErrorAction Stop).Source
        $ninjaFile = Join-Path -Path $buildDirectory -ChildPath 'build.ninja'
        if($Reconfigure -or -not (Test-Path -LiteralPath $ninjaFile))
        {
            Write-Info "CMake configuration (preset: $Preset)"
            $configureArgs = @('--preset', $Preset)
            if($Reconfigure)
            {
                $configureArgs += '--fresh'
            }
            $configureArgs += $CMakeArgs
            Invoke-LoggedCommand -FilePath $cmakeCommand `
                -CommandArgs $configureArgs `
                -LogPath (Join-Path $buildRoot "pwsh_configure_${Preset}.log")
        }
        else
        {
            Write-Info 'CMake cache valid, skipping configuration (use -Reconfigure to force)'
        }

        $jobCount = [Environment]::ProcessorCount
        Write-Info "Starting build (parallel: $jobCount threads)"
        Invoke-LoggedCommand -FilePath $cmakeCommand `
            -CommandArgs @('--build', $buildDirectory, '-j', "$jobCount") `
            -LogPath (Join-Path $buildRoot "pwsh_build_${Preset}.log")
        Write-Success "Build completed: $hexPath"
    }

    if($BuildOnly)
    {
        Write-Info 'Build-only mode, skipping loading operations'
        return
    }

    if(-not (Test-Path -LiteralPath $hexPath -PathType Leaf))
    {
        throw "Hex file not found: $hexPath"
    }
    if($Verify -and -not (Test-Path -LiteralPath $binaryPath -PathType Leaf))
    {
        throw "Binary file required by -Verify was not found: $binaryPath"
    }

    $jlinkCommand = Find-JLinkCommander
    $commanderScript = Join-Path $buildRoot `
        ('.jlink_' + [Guid]::NewGuid().ToString('N') + '.cmd')
    $jlinkConsoleLog = Join-Path $buildRoot "pwsh_flash_${Preset}.log"
    $jlinkInternalLog = Join-Path $buildRoot "pwsh_jlink_${Preset}.log"

    $commands = [System.Collections.Generic.List[string]]::new()
    $commands.Add('si SWD')
    $commands.Add("speed $Speed")
    $commands.Add("device $Device")
    $commands.Add('connect')
    $commands.Add('r')
    $commands.Add('h')
    if($ForceErase)
    {
        $commands.Add('erase')
        $commands.Add('sleep 100')
    }
    $commands.Add("loadfile `"$hexPath`"")
    if($Verify)
    {
        $commands.Add("verifybin `"$binaryPath`", 0x08000000")
    }
    $commands.Add('r')
    if($NoReset)
    {
        $commands.Add('h')
        $commands.Add('echo Flash done, target halted (-NoReset)')
    }
    else
    {
        $commands.Add('g')
    }
    $commands.Add('exit')

    try
    {
        Set-Content -LiteralPath $commanderScript -Value $commands `
            -Encoding ascii
        $flashArgs = @(
            '-Device', $Device,
            '-If', 'SWD',
            '-Speed', "$Speed",
            '-AutoConnect', '1',
            '-ExitOnError', '1',
            '-Log', $jlinkInternalLog,
            '-CommanderScript', $commanderScript
        )
        $flashArgs += $JLinkArgs

        Write-Info "Preparing to flash device: $Device"
        Invoke-LoggedCommand -FilePath $jlinkCommand `
            -CommandArgs $flashArgs -LogPath $jlinkConsoleLog `
            -TimeoutSeconds $JLinkTimeoutSeconds
        Write-Success 'Flashing successful'
    }
    finally
    {
        if(Test-Path -LiteralPath $commanderScript)
        {
            Remove-Item -LiteralPath $commanderScript -Force
        }
    }
}
finally
{
    Pop-Location
}
