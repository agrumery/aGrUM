param(
    [string]$CI_JOB_ID,
    [string]$CI_PROJECT_DIR,
    [string]$PYTHON_VERSION,
    [string]$TARGET,
    [string]$COMPILER
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

# Building wheels based on WHEEL_TYPE
Write-Host "Building $TARGET..."
Set-Location $CI_PROJECT_DIR
python act clean
if ($TARGET -eq "aGrUM") {
    python act --no-fun --compiler=$COMPILER -d build -j except1 test release aGrUM
} else {

    if ($COMPILER -eq "mingw64") {
        python act install release pyAgrum --compiler=mingw64 -d build -j except1 -t quick
        $files = @("libgcc_s_seh-1", "libgomp-1", "libssp-0", "libstdc++-6", "libwinpthread-1")

        foreach ($file in $files) {
            Copy-Item -Path "C:\msys64\mingw64\bin\$file.dll" -Destination "$PWD\build\pyAgrum\release\wrappers\pyAgrum" -Force
        }
        python act test release pyAgrum
    } else {
        python act --no-fun test release pyAgrum --no-fun --compiler=$COMPILER -d build -j except1 -t quick
    }
}
Check-LastCommand

# Cleanup
Write-Host "Cleaning up..."
conda deactivate
conda remove -y -n gitlab-ci-$CI_JOB_ID --all --force
Check-LastCommand

Write-Host "Script completed successfully."
exit 0