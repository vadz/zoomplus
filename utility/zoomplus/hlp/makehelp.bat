echo Input Name: %1

start /wait hcw /C /E /M "hlp\%1.hpj"

if errorlevel 1 goto :Error
if not exist "hlp\%1.hlp" goto :Error
copy "hlp\%1.hlp" .\
goto :done
:Error
echo hlp\%1.hpj(1) : error: 
:done
type "hlp\%1.log"
