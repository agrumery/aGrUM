param(
    [string]$CI_JOB_ID,
    [string]$CI_PROJECT_DIR,
    [string]$PYTHON_VERSION
)

# Tests the wheel(s) of wheels\ in a throwaway venv, private to this job.
#
# The conda env $PYTHON_VERSION (py311, ..., py315) is only used as the base
# interpreter: nothing is installed in it anymore. Before, the wheel was
# pip-(un)installed directly in the shared conda env, so two concurrent jobs on
# the same Python version (two pipelines at once) broke each other, a cancelled
# job left pyAgrum installed, and the test dependencies were frozen at the
# env's creation date. The dependencies now come fresh from requirements.txt,
# as on Linux (pip's per-user cache makes it fast after the first job).

# Stop the script on any non-terminating errors
$ErrorActionPreference = "Stop"

# throw (not exit): the finally block below must always run to remove the venv.
function Assert-Success([string]$what) {
    if ($LASTEXITCODE -ne 0) {
        throw "$what failed (exit code $LASTEXITCODE)."
    }
}

# Activating the conda env is still required: it puts <env>\Library\bin on the
# PATH, where the venv's interpreter finds conda's DLLs (openssl, ...).
Write-Host "Activating the conda environment (base interpreter): $PYTHON_VERSION"
conda activate $PYTHON_VERSION
if (-not $?) {
    Write-Host "conda activate $PYTHON_VERSION failed."
    exit 1
}

$VENV = Join-Path $env:TEMP "pyagrum-test-$CI_JOB_ID"
$PY = Join-Path $VENV "Scripts\python.exe"

# Same job id = a retried job: start from scratch.
if (Test-Path $VENV) {
    Remove-Item -Recurse -Force $VENV
}

try {
    Write-Host "Creating the job venv: $VENV"
    python -m venv $VENV
    Assert-Success "venv creation"

    # The venv's python is called directly: no Activate.ps1 (and no execution
    # policy question), no ambiguity on which pip is used.
    Write-Host "Installing the test requirements..."
    & $PY -m pip install --upgrade pip
    Assert-Success "pip upgrade"
    & $PY -m pip install -r "$CI_PROJECT_DIR\wrappers\pyagrum\testunits\requirements.txt"
    Assert-Success "requirements install"

    Write-Host "Installing wheels from the wheels directory..."
    $wheels = @(Get-ChildItem -Path "$CI_PROJECT_DIR\wheels" -Filter "*.whl")
    if ($wheels.Count -eq 0) {
        throw "No wheel found in $CI_PROJECT_DIR\wheels."
    }
    foreach ($wheel in $wheels) {
        & $PY -m pip install $wheel.FullName
        Assert-Success "install of $($wheel.Name)"
    }

    Write-Host "Running Python test script..."
    Set-Location $CI_PROJECT_DIR
    & $PY wrappers\pyagrum\testunits\gumTest.py
    Assert-Success "gumTest.py"
}
finally {
    Write-Host "Removing the job venv..."
    Set-Location $CI_PROJECT_DIR
    Remove-Item -Recurse -Force $VENV -ErrorAction SilentlyContinue
}

Write-Host "Script execution completed."
exit 0
