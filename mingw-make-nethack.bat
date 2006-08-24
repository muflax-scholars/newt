PATH=\msys\1.0\mingw\bin;\MinGW\bin\;%PATH%
REM ---------- NetHack ----------
del /S /Q nethack\binary
del /S /Q nethack\win\newt
mkdir nethack\win\newt
xcopy /E /-Y newt\* nethack\win\newt\
cd nethack\sys\winnt
call nhsetup
cd ..\..\src
mingw32-make -f Makefile.gcc spotless
mingw32-make -f Makefile.gcc all
cd ..\..
