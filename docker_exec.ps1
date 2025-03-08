param (
    [string]$ContainerName,
    [string]$Command
)

# Construct the correct Docker execution command
$cmd = "docker exec -i $ContainerName sh -c ""$Command"""

# Run the command in PowerShell and capture exit code
$process = Start-Process -NoNewWindow -FilePath "cmd.exe" -ArgumentList "/c $cmd" -PassThru -Wait

# Return success (0) if the command executed properly, otherwise return failure (1)
exit $process.ExitCode
