@echo off

REM  Copyright 2016 CUBRID Corporation
REM
REM   This program is free software; you can redistribute it and/or modify
REM   it under the terms of the GNU General Public License as published by
REM   the Free Software Foundation; either version 2 of the License, or
REM   (at your option) any later version.
REM
REM  This program is distributed in the hope that it will be useful,
REM  but WITHOUT ANY WARRANTY; without even the implied warranty of
REM  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
REM  GNU General Public License for more details.
REM
REM  You should have received a copy of the GNU General Public License
REM  along with this program; if not, write to the Free Software
REM  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

rem batch script for CUBRID Environments, (window services, registry)

rem LOADING CUBRID Environments
echo Setting CUBRID Environments


set CUBRID=C:\CUBRID
set CUBRID_DATABASES=%CUBRID%\databases

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\CUBRID\cmserver" /v "ROOT_PATH" /t REG_SZ /d "%CUBRID%\\" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\CUBRID\cmserver" /v "Version" /t REG_SZ /d "2.1.1" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\CUBRID\CUBRID" /v "ROOT_PATH" /t REG_SZ /d "%CUBRID%\\" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\CUBRID\CUBRID" /v "Version" /t REG_SZ /d "6.0.1" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\CUBRID\CUBRID" /v "Patch" /t REG_SZ /d "8" /f

reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v "CUBRID" /t REG_SZ /d "%CUBRID%\\" /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v "CUBRID_DATABASES" /t REG_SZ /d "%CUBRID_DATABASES%" /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v "Path" /t REG_SZ /d "%CUBRID%\bin;%PATH%" /f

echo %CUBRID%
echo %CUBRID_DATABASES%
echo %Path%