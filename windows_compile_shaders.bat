:: use .\computer-graphics-project-2024\windows_compile_shaders.bat to call from visual studio terminal

@echo off
set glslc="C:\VulkanSDK\Bin\glslc.exe"
set shader_dir=.\computer-graphics-project-2024\shaders\
for %%f in (%shader_dir%*.frag %shader_dir%*.vert) do (
    call :CompileShader "%%f"
)
echo Compilation completed.
pause
exit

:CompileShader
set input_file=%~1
set filename=%~n1
set shader_name=%filename:Shader=%
set output_dir=%~dp1
set output_file=%output_dir%%shader_name%Frag.spv
if "%~x1"==".vert" set output_file=%output_dir%%shader_name%Vert.spv
%glslc% "%input_file%" -o "%output_file%"
exit /b
