@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Find Python
for /f "delims=" %%i in ('where python') do (
    set "RAW=%%i"
    goto :found
)

echo ERROR: Python not found
exit /b 1

:found
REM Replace backslashes with forward slashes
set "PYTHON_EXE=!RAW:\=/!"

endlocal & set "PYTHON_EXE=%PYTHON_EXE%"
