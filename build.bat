setlocal enabledelayedexpansion

:: Set the compiler and assembler commands
set NASM=nasm
set GCC=g++

:: Directories for source files and object files
set CPP_DIR=c++
set C_DIR=c

set OBJ_DIR=debug
set ASM_DIR=assembly

:: Create the object directory if it does not exist
if not exist %OBJ_DIR% mkdir %OBJ_DIR%

%GCC% -c -o %OBJ_DIR%\main.obj main.cpp

:: CPP
for %%f in (%CPP_DIR%\*.cpp) do (
    %GCC% -c -o %OBJ_DIR%\%%~nf.obj %%f
)

:: C

for %%f in (%C_DIR%\*.c) do (
    %GCC% -c -o %OBJ_DIR%\%%~nf.o %%f
)

:: MAIN

%GCC% -o main.exe %OBJ_DIR%\*.obj %OBJ_DIR%\*.o