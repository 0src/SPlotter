@rem not working yet... building vs stuff in cli is PITA!
@echo off

@rem unset these variables
@set WindowsSdkDir=
@set WindowsSDK_ExecutablePath_x64=
@set WindowsSDK_ExecutablePath_x86=
@set Framework40Version=
@set FrameworkDIR32=
@set FrameworkVersion32=
@set FSHARPINSTALLDIR=
@set VSINSTALLDIR=
@set VCINSTALLDIR=
@set MSVCDIR=%programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\bin\HostX64\x64
@set MSBUILDDIR=%programfiles(x86)%\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin
@set VCTARGETSPATH=%programfiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\VCTargets
@rem Add path to MSBuild Binaries
@if exist "%MSBUILDDIR%" set PATH=%MSBUILDDIR%;%MSVCDIR%;%PATH%

MSBuild ..\SPlotter.sln /t:Rebuild /p:Configuration=AVX2 /p:VCTargetsPath="%VCTARGETSPATH%"
rem MSBuild SPlotter.sln /t:Rebuild /p:Configuration=AVX
rem MSBuild SPlotter.sln /t:Rebuild /p:Configuration=SSE

@goto end

:end
