param(
    [string]$CI_JOB_ID,
    [string]$CI_PROJECT_DIR,
    [string]$CONDA_ENV,
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

Write-Host "Activating the new conda environment: $CONDA_ENV"
conda activate $CONDA_ENV
Check-LastCommand

# Update version
Write-Host "Updating version..."
Set-Location $CI_PROJECT_DIR\wrappers\pyagrum\wheelhouse\scripts
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
        python act install release pyAgrum --compiler=mingw64 -d build -j except1 -m quick -t all
        $files = @("libgcc_s_seh-1", "libgomp-1", "libssp-0", "libstdc++-6", "libwinpthread-1")

        foreach ($file in $files) {
            Copy-Item -Path "C:\msys64\mingw64\bin\$file.dll" -Destination "$PWD\build\pyagrum\release\wrappers\pyAgrum" -Force
        }
        python act test release pyAgrum
    } else {
        python act --no-fun test release pyAgrum --no-fun --compiler=$COMPILER -d build -j except1  -m quick -t all
    }
}
Check-LastCommand

Write-Host "Script completed successfully."
exit 0
