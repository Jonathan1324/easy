rmdir /S /Q output
mkdir output
call .\build.bat
main.exe --d --f --c --c-to-all code.eas -o output/main
main.exe --py code.eas -o output/main