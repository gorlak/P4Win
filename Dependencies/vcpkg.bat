@echo off

:: -disableMetrics in important to avoid Malwarebytes quarantine the vcpkg file. 
call %~dp0vcpkg\bootstrap-vcpkg.bat -disableMetrics

:: build for each triplet
for %%x in (overlay-x64-windows overlay-x86-windows) do (
    %~dp0vcpkg\vcpkg.exe install --x-install-root=%~dp0vcpkg-installed --overlay-ports=%~dp0/vcpkg-overlay --overlay-triplets=%~dp0/vcpkg-overlay --triplet=%%x-%1 openssl
    if ERRORLEVEL 1 exit /b 1
)
