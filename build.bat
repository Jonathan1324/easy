setlocal enabledelayedexpansion

:: Set the compiler and assembler commands
set NASM=nasm
set GCC=g++

:: Directories for source files and object files
set CPP_DIR=c++
set C_DIR=c
set ASM_DIR=assembly

set OBJ_DIR=debug

set RUST_DIR=my_rust_lib
set RUST_LIB_NAME=my_rust_lib

:: Create the object directory if it does not exist
if not exist %OBJ_DIR% mkdir %OBJ_DIR%

:: Kompiliere die MAIN CPP Datei
%GCC% -c -o %OBJ_DIR%\main.obj main.cpp

:: ASSEMBLY
for %%f in (%ASM_DIR%\*.asm) do (
    %NASM% -f win64 %%f -o %OBJ_DIR%\%%~nf.obj
)

:: CPP
:: for %%f in (%CPP_DIR%\*.cpp) do (
::    %GCC% -c -o %OBJ_DIR%\%%~nf.obj %%f
::)

:: C
for %%f in (%C_DIR%\*.c) do (
    %GCC% -c -o %OBJ_DIR%\%%~nf.obj %%f
)

:: MAIN
%GCC% -o easy.exe %OBJ_DIR%\*.obj -static