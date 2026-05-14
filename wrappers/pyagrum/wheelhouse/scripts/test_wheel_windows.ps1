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

# Activate the new environment
Write-Host "Activating the conda environment: $PYTHON_VERSION"
conda activate $PYTHON_VERSION
Check-LastCommand

# Uninstall any existing pyAgrum packages to avoid caching/conflict issues
Write-Host "Uninstalling existing pyAgrum and pyAgrum-nightly packages..."
pip uninstall -y pyAgrum pyAgrum-nightly

# Install the wheels from the wheels directory
Write-Host "Installing wheels from the wheels directory..."
Get-ChildItem -Path "$CI_PROJECT_DIR\wheels" -Filter "*.whl" | ForEach-Object {
    pip install $_.FullName
    Check-LastCommand
}

# Run the Python test script
Write-Host "Running Python test script..."
Set-Location $CI_PROJECT_DIR
python wrappers\pyagrum\testunits\gumTest.py
Check-LastCommand

# Clean up after tests to leave the environment pristine
Write-Host "Cleaning up: Uninstalling pyAgrum and pyAgrum-nightly..."
pip uninstall -y pyAgrum pyAgrum-nightly

Write-Host "Script execution completed."
exit 0