mkdir %1\accounts
mkdir %1\archives
mkdir %1\books
mkdir %1\dfndata
mkdir %1\dictionaries
mkdir %1\docs
mkdir %1\help
mkdir %1\html
mkdir %1\js
mkdir %1\logs
mkdir %1\msgboards
mkdir %1\shared

xcopy data\accounts\*.* %1\accounts /s /c /y
xcopy data\archives\*.* %1\archives /s /c /y
xcopy data\books\*.* %1\books /s /c /y
xcopy data\dfndata\*.* %1\dfndata /s /c /y
xcopy data\dictionaries\*.* %1\dictionaries /s /c /y
xcopy docs\*.* %1\docs /s /c /y
rem xcopy data\help\*.* %1\help /s /c /y
xcopy html\*.* %1\html /s /c /y
xcopy data\js\*.* %1\js /s /c /y
xcopy data\logs\*.* %1\logs /s /c /y
xcopy data\msgboards\*.* %1\msgboards /s /c /y
rem xcopy data\shared\*.* %1\shared /s /c /y
xcopy data\*.ini %1 /c /y
xcopy w32installer\uox.nsi %1