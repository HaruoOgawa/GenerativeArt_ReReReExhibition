set CurrentDir=%~dp0
set AppDir=Haru86_GraphicsEngine\Assets\App\GradDemo 
set BuiltDir=Haru86_GraphicsEngine\GraphicsEngine\Graphics\Shader

for /R %AppDir% %%i in (*.vert) do copy %%~i %%~pi%%~ni_Vert.h

for /R %AppDir% %%i in (*.frag) do copy %%~i %%~pi%%~ni_Frag.h

for /R %BuiltDir% %%i in (*.vert) do copy %%~i %%~pi%%~ni_Vert.h

for /R %BuiltDir% %%i in (*.frag) do copy %%~i %%~pi%%~ni_Frag.h

pause