@echo off
echo Installing Python dependencies for graphs generation...
echo.

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python not found!
    echo Please install Python from: https://www.python.org/
    echo Make sure to check "Add Python to PATH" during installation
    pause
    exit /b 1
)

echo Python found. Installing libraries...
echo.

REM Install required libraries
pip install matplotlib pandas numpy

if errorlevel 1 (
    echo.
    echo ERROR: Installation failed.
    echo Try: python -m pip install matplotlib pandas numpy
    pause
    exit /b 1
)

echo.
echo SUCCESS: All dependencies installed!
echo.
echo You can now:
echo 1. Run the C++ program to generate results
echo 2. Run: python scripts/generate_plots.py
echo.
pause