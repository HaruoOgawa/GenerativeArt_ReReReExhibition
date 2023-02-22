set EnvPath=%~dp0

set EnvPathDebug=%EnvPath%Debug\bin
set EnvPathRelease=%EnvPath%Release\bin
set EnvPathDemoRelease=%EnvPath%DemoRelease\bin
set Path_setup_x64=%EnvPath%setup_x64.bat

setx /M Path "%Path%;%EnvPathDebug%;%EnvPathRelease%;%EnvPathDemoRelease%"

attrib +s +h %EnvPathDebug%
attrib +s +h %EnvPathRelease%
attrib +s +h %EnvPathDemoRelease%
attrib +s +h %Path_setup_x64%

xcopy %EnvPath%Release\Haru86_GraphicsEngine.exe %EnvPath%DemoRelease\ /Y /E /I /H /R
call %EnvPath%..\compress_tools\squishy-x64.exe -i %EnvPath%DemoRelease\Haru86_GraphicsEngine.exe -o %EnvPath%DemoRelease\Haru86_GraphicsEngine.exe

@rem echo %EnvPathDebug%
@rem echo %EnvPathRelease% 
@rem echo %EnvPathDemoRelease%
 
@rem pause