PATH=\msys\1.0\mingw\bin;\MinGW\bin\;%PATH%
REM ---------- SlashEM ----------
del /S /Q slashem\binary
del /S /Q slashem\win\newt
mkdir slashem\win\newt
xcopy /E /-Y newt\* slashem\win\newt\
cd slashem\sys\winnt
call nhsetup
cd ..\..\src
mingw32-make -f Makefile.gcc spotless
mingw32-make -f Makefile.gcc all
cd ..\..
