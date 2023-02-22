xcopy .\x64\ Release4Submission\x64\ /Y /E /I /H /R
xcopy .\Release4Submission\x64\Release\Haru86_GraphicsEngine.exe Release4Submission\x64\DemoRelease\ /Y /E /I /H /R

xcopy .\setup.bat Release4Submission\ /Y /I /H /R
xcopy .\setup_x64.bat Release4Submission\x64\ /Y /I /H /R
xcopy .\compress_tools\squishy-x64.exe Release4Submission\compress_tools\ /Y /E /I /H /R

@rem 再表示
set EnvPath=%~dp0

set EnvPathDebug=%EnvPath%Release4Submission\x64\Debug\bin
set EnvPathRelease=%EnvPath%Release4Submission\x64\Release\bin
set EnvPathDemoRelease=%EnvPath%Release4Submission\x64\DemoRelease\bin
set Path_setup_x64=%EnvPath%Release4Submission\x64\setup_x64.bat

attrib -s -h %EnvPathDebug%
attrib -s -h %EnvPathRelease%
attrib -s -h %EnvPathDemoRelease%
attrib -s -h %Path_setup_x64%

pause