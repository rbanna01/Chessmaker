del /q Emscripten\*.*
mkdir Emscripten

SET optimisation=%1

@ECHO ON

IF "%1" == "" (
  SET optimisation = "-O0"
)

SET "allFiles = "

setlocal enabledelayedexpansion
for %%f in (*.cpp) do (
	echo Generating %%f...
	call emcc %optimisation% %%f -o Emscripten\%%~nf.bc -std=c++11
	set "allFiles=!allFiles! Emscripten\%%~nf.bc "
)
setlocal disabledelayedexpansion

echo Linking JS...
emcc %optimisation% %allFiles% -o ../ChessMaker/Scripts/chessmaker.js