setlocal enabledelayedexpansion

:: Set the compiler and assembler commands
set NASM=nasm
set GCC=g++

:: Directories for source files and object files
set SRC_DIR=c++
set OBJ_DIR=debug
set ASM_DIR=assembly

:: Create the object directory if it does not exist
if not exist %OBJ_DIR% mkdir %OBJ_DIR%

%GCC% -c -o %OBJ_DIR%\main.obj main.cpp

:: Compile C++ files
for %%f in (%SRC_DIR%\*.cpp) do (
    %GCC% -c -o %OBJ_DIR%\%%~nf.obj %%f
)

%GCC% -o main.exe %OBJ_DIR%\*.obj