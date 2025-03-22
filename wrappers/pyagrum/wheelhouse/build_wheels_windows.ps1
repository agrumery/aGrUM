param(
    [string]$CI_JOB_ID,
    [string]$CI_PROJECT_DIR,
    [string]$CONDA_ENV
)

# Stop the script on any non-terminating errors
$ErrorActionPreference = "Stop"

function Check-LastCommand {
    if (-not $?) {
        Write-Error "Previous command failed."
        exit 1
    }
}

Write-Host "Activating the conda environment: $CONDA_ENV"
conda activate $CONDA_ENV
Check-LastCommand

# Update version
Write-Host "Updating version..."
Set-Location $CI_PROJECT_DIR\wrappers\pyagrum\wheelhouse\scripts
python update_version.py $CI_PROJECT_DIR 0
Check-LastCommand

# Create wheels directory
Write-Host "Creating a dictionary to store wheels"
New-Item -Path "$CI_PROJECT_DIR\wheels" -ItemType Directory
Check-LastCommand

# Building wheels
Write-Host "Building the wheels..."
Set-Location $CI_PROJECT_DIR
python act clean
python act --no-fun --compiler=mvsc22 -d wheels release wheel pyAgrum -j halfexcept1
Check-LastCommand

Write-Host "Script completed successfully."
exit 0
