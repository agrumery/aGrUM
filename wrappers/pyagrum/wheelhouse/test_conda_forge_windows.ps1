param(
    [Parameter(Mandatory = $true)]
    [string]$CI_JOB_ID,

    [Parameter(Mandatory = $true)]
    [string]$CI_PROJECT_DIR
)

$ErrorActionPreference = "Stop"

function Invoke-NativeCommand {
    param(
        [Parameter(Mandatory = $true)]
        [scriptblock]$Command
    )

    & $Command

    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE"
    }
}

function Enable-Msvc2022 {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    $vsPath = & $vswhere `
        -latest `
        -products * `
        -version "[17.0,18.0)" `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath

    if (-not $vsPath) {
        throw "Visual Studio 2022 with C++ tools not found."
    }

    $vcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"

    $environment = cmd.exe /s /c "`"$vcvars`" >nul && set"

    foreach ($line in $environment) {
        if ($line -match "^([^=]+)=(.*)$") {
            Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
        }
    }

    $cl = Get-Command cl.exe -ErrorAction Stop
    Write-Host "MSVC compiler found:"
    Write-Host "  $($cl.Source)"
}

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

# Unique environment for this CI job.
# Do NOT reuse the regular py310 environment: we want the current
# conda-forge packages to be resolved at every execution.
$CONDA_PREFIX_CI = Join-Path $env:TEMP "pyagrum-conda-forge-$CI_JOB_ID"

$BUILD_DIR = Join-Path $CI_PROJECT_DIR "build-conda-forge-preflight"


Write-Host "============================================================"
Write-Host " pyAgrum / conda-forge Windows compatibility test"
Write-Host "============================================================"
Write-Host "Project:      $CI_PROJECT_DIR"
Write-Host "Conda prefix: $CONDA_PREFIX_CI"
Write-Host "Build dir:    $BUILD_DIR"


# ---------------------------------------------------------------------------
# Cleanup from a previous interrupted job
# ---------------------------------------------------------------------------

if (Test-Path $CONDA_PREFIX_CI) {
    Write-Host "Removing previous conda environment..."
    Remove-Item -Recurse -Force $CONDA_PREFIX_CI
}

if (Test-Path $BUILD_DIR) {
    Write-Host "Removing previous build directory..."
    Remove-Item -Recurse -Force $BUILD_DIR
}


try {

    # -----------------------------------------------------------------------
    # Create fresh conda-forge environment
    # -----------------------------------------------------------------------

    Write-Host ""
    Write-Host "Creating fresh conda-forge environment..."

    Invoke-NativeCommand {
        conda create `
            --yes `
            --prefix $CONDA_PREFIX_CI `
            --override-channels `
            --channel conda-forge `
            "python=3.10" `
            "numpy=2" `
            "python-abi3=3.10.*" `
            cmake `
            ninja `
            swig `
            vs2022_win-64 `
            pydot `
            six `
            matplotlib-base `
            ipython `
            ipykernel `
            pandas `
            matplotlib-inline `
            "scikit-learn>=1.7" `
            scipy
    }


    # -----------------------------------------------------------------------
    # Activate environment
    #
    # This also executes the conda-forge MSVC activation scripts installed
    # by vs2022_win-64.
    # -----------------------------------------------------------------------

    Write-Host ""
    Write-Host "Activating conda-forge environment..."

    conda activate $CONDA_PREFIX_CI
    Enable-Msvc2022

    $env:PYTHONUTF8 = "1"
    $env:PYTHONIOENCODING = "utf-8"

    Write-Host ""
    Write-Host "Conda prefix:"
    Write-Host "  $env:CONDA_PREFIX"

    Write-Host ""
    Write-Host "Python:"
    python --version

    Write-Host ""
    Write-Host "CMake:"
    cmake --version

    Write-Host ""
    Write-Host "Ninja:"
    ninja --version

    Write-Host ""
    Write-Host "MSVC:"
    $cl = Get-Command cl.exe -ErrorAction Stop
    Write-Host "  $($cl.Source)"

    # -----------------------------------------------------------------------
    # Paths matching the Windows conda layout
    # -----------------------------------------------------------------------

    $LIBRARY_PREFIX = Join-Path $env:CONDA_PREFIX "Library"
    $SITE_PACKAGES   = Join-Path $env:CONDA_PREFIX "Lib\site-packages"

    # CMake likes forward slashes.
    $LIBRARY_PREFIX_CMAKE = $LIBRARY_PREFIX.Replace("\", "/")
    $CONDA_PREFIX_CMAKE   = $env:CONDA_PREFIX.Replace("\", "/")
    $SITE_PACKAGES_CMAKE  = $SITE_PACKAGES.Replace("\", "/")


    # -----------------------------------------------------------------------
    # Configure
    #
    # Keep this intentionally close to conda-forge's recipe/build.bat.
    # -----------------------------------------------------------------------

    Write-Host ""
    Write-Host "Configuring pyAgrum..."

    Set-Location $CI_PROJECT_DIR

    Invoke-NativeCommand {
        cmake `
            -LAH `
            -G Ninja `
            -DCMAKE_BUILD_TYPE=Release `
            "-DCMAKE_PREFIX_PATH=$LIBRARY_PREFIX_CMAKE" `
            "-DCMAKE_INSTALL_PREFIX=$LIBRARY_PREFIX_CMAKE" `
            -DCMAKE_UNITY_BUILD=ON `
            -DBUILD_PYTHON=ON `
            -DBUILD_SHARED_LIBS=OFF `
            -DPython_FIND_STRATEGY=LOCATION `
            "-DPython_ROOT_DIR=$CONDA_PREFIX_CMAKE" `
            "-DINSTALL_PYTHONDIR=$SITE_PACKAGES_CMAKE" `
            -DAGRUM_PYTHON_SABI=ON `
            "-DCMAKE_CXX_FLAGS=/utf-8 /EHsc" `
            -B $BUILD_DIR `
            .
    }


    # -----------------------------------------------------------------------
    # Build + install
    # -----------------------------------------------------------------------

    Write-Host ""
    Write-Host "Building pyAgrum..."

    $CPU_COUNT = [Environment]::ProcessorCount

    Invoke-NativeCommand {
        cmake `
            --build $BUILD_DIR `
            --target install `
            --config Release `
            --parallel $CPU_COUNT
    }


    # -----------------------------------------------------------------------
    # Tests
    # -----------------------------------------------------------------------

    Write-Host ""
    Write-Host "Testing import..."

    Invoke-NativeCommand {
        python -c "import pyagrum; print('pyagrum import OK:', pyagrum.__version__)"
    }


    Write-Host ""
    Write-Host "Running pyAgrum test suite..."

    Invoke-NativeCommand {
        python "$CI_PROJECT_DIR\wrappers\pyagrum\testunits\gumTest.py"
    }


    Write-Host ""
    Write-Host "============================================================"
    Write-Host " CONDA-FORGE WINDOWS PREFLIGHT SUCCESSFUL"
    Write-Host "============================================================"
}
catch {
    Write-Host ""
    Write-Host "============================================================"
    Write-Host " CONDA-FORGE WINDOWS PREFLIGHT FAILED"
    Write-Host "============================================================"

    Write-Host "ERROR: $($_.Exception.Message)"
    Write-Host ""
    Write-Host "STACK:"
    Write-Host $_.ScriptStackTrace

    exit 1
}
finally {

    Set-Location $CI_PROJECT_DIR

    Write-Host ""
    Write-Host "Cleaning temporary build..."

    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR -ErrorAction SilentlyContinue
    }

    Write-Host "Removing temporary conda environment..."

    # Leaving an active environment while deleting it is not ideal.
    conda deactivate 2>$null

    if (Test-Path $CONDA_PREFIX_CI) {
        conda env remove --yes --prefix $CONDA_PREFIX_CI 2>$null

        if (Test-Path $CONDA_PREFIX_CI) {
            Remove-Item -Recurse -Force $CONDA_PREFIX_CI `
                -ErrorAction SilentlyContinue
        }
    }
}