del /q Emscripten\*.*
mkdir Emscripten

SET optimisation=%*

IF "%optimisation%" == "" (
  SET optimisation = "-O0"
)

SET "allFiles = "

setlocal enabledelayedexpansion
for %%f in (*.cpp) do (
	echo Processing %%f...
	call emcc %optimisation% %%f -o Emscripten\%%~nf.bc -std=c++11
	set "allFiles=!allFiles! Emscripten\%%~nf.bc "
)
setlocal disabledelayedexpansion

echo Linking JS...
emcc %optimisation% --memory-init-file 0 %allFiles% -o ../ChessMaker/Scripts/Game/Chessmaker.js