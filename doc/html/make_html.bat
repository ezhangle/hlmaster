@echo off

echo Create html ::...................................................
echo -----------------------------------------------------------------
echo   o hlmaster.1
groff -Tascii -man ..\man\hlmaster.1 | sed -f man2html.sed > hlmaster.html

echo   o hlmaster.gameserver.1
groff -Tascii -man ..\man\hlmaster.gameserver.1 | sed -f man2html.sed > hlmaster.gameserver.html

echo   o hlmaster.list.1
groff -Tascii -man ..\man\hlmaster.list.1 | sed -f man2html.sed > hlmaster.list.html

echo   o hlmaster.filter.1
groff -Tascii -man ..\man\hlmaster.filter.1 | sed -f man2html.sed > hlmaster.filter.html


echo.
echo.
echo Checking html files for bad links (they are marked red) ::.......
echo -----------------------------------------------------------------
grep -l "color=red" -F *.html

echo.
echo.
pause
