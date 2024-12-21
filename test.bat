rmdir /S /Q output
mkdir output
call .\build.bat
.\easy.exe --d --fc --c-to-all -c -dont-i code.eas -o output/main
.\easy.exe --c-to-all code.eas -o output/main