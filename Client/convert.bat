setlocal enableextensions enabledelayedexpansion
mkdir  %2\models 2> NUL
mkdir  %2\animations 2> NUL
FOR %%f IN (%3\*.tx) DO (
IF EXIST %2\models\%%~nf.btx (
FOR %%i IN (%3\%%~nf.tx) DO SET DATE1=%%~ti
FOR %%i IN (%2\models\%%~nf.btx) DO SET DATE2=%%~ti
IF NOT "!DATE1!" EQU "!DATE2!" (
echo Started converting %%~nf
call %1 %%f > NUL
move /y %3\%%~nf.btx %2\models\ > NUL
IF EXIST %3\%%~nf.atx (
move /y %3\%%~nf.atx %2\animations\ > NUL
)
copy %%f /b +,, %3\ /y > NUL
echo Updated the %%~nf.btx to a newer version. 
) 
) ELSE (
echo Started converting %%~nf
call %1 %%f > NUL
move /y %3\%%~nf.btx %2\models\ > NUL
IF EXIST %3\%%~nf.atx (
move /y %3\%%~nf.atx %2\animations\ > NUL
)
copy %%f /b +,, %3\ /y > NUL
echo Converted the new %%~nf.tx file. )
)