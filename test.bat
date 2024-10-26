rmdir /S /Q output
mkdir output
call .\build.bat
main.exe --d --fc --c --c-to-all code.eas -o output/main
main.exe --c-to-all code.eas -o output/main