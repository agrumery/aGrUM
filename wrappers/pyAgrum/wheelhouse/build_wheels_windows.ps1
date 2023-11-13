param(
    [string]$CI_JOB_ID,
    [string]$CI_PROJECT_DIR,
    [string]$PYTHON_VERSION
)

# Stop the script on any non-terminating errors
$ErrorActionPreference = "Stop"

function Check-LastCommand {
    if (-not $?) {
        Write-Error "Previous command failed."
        exit 1
    }
}

# Create and activate a new conda environment
Write-Host "Creating a new conda environment with Python $PYTHON_VERSION"
conda create -n gitlab-ci-$CI_JOB_ID -c conda-forge python=$PYTHON_VERSION -y
Check-LastCommand

Write-Host "Activating the new conda environment: gitlab-ci-$CI_JOB_ID"
conda activate gitlab-ci-$CI_JOB_ID
Check-LastCommand

# Install dependencies
Write-Host "Installing requirements and cmake..."
conda install -y -c conda-forge --file $CI_PROJECT_DIR\wrappers\pyAgrum\testunits\requirements.txt
conda install -y -c conda-forge cmake
Check-LastCommand

# Update version
Write-Host "Updating version..."
Set-Location $CI_PROJECT_DIR\wrappers\pyAgrum\wheelhouse\scripts
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

# Cleanup
Write-Host "Cleaning up..."
conda deactivate
conda remove -y -n gitlab-ci-$CI_JOB_ID --all --force
Check-LastCommand

Write-Host "Script completed successfully."
exit 0