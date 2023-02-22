set CurrentDir=%~dp0
set AppDir=Haru86_GraphicsEngine\Assets\App\GradDemo 
set BuiltDir=Haru86_GraphicsEngine\GraphicsEngine\Graphics\Shader

for /R %AppDir% %%i in (*Vert.h) do .\compress_tools\shader_minifier --format c-array --preserve-externals uniform -o %%~pi%%~niComp.h %%~i

for /R %AppDir% %%i in (*Frag.h) do .\compress_tools\shader_minifier --format c-array --preserve-externals uniform -o %%~pi%%~niComp.h %%~i

for /R %BuiltDir% %%i in (*Vert.h) do .\compress_tools\shader_minifier --format c-array --preserve-externals uniform -o %%~pi%%~niComp.h %%~i

for /R %BuiltDir% %%i in (*Frag.h) do .\compress_tools\shader_minifier --format c-array --preserve-externals uniform -o %%~pi%%~niComp.h %%~i

@rem pause