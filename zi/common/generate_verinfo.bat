git log --date=short --pretty=format:"#define VER_INFO_PRESENT %%nstatic const char* VER_INFO = \"%%h %%an %%ad\"; " -n 1 > version.h

@echo off
REM Batch file to generate a C header file from `git describe`

REM Run git describe and store the output
for /f "delims=" %%i in ('git describe') do set GIT_DESCRIBE=%%i

REM Define the output header file name
set HEADER_FILE=git_describe.h

REM Generate the header file content
echo #ifndef GIT_DESCRIBE_H > %HEADER_FILE%
echo #define GIT_DESCRIBE_H >> %HEADER_FILE%
echo. >> %HEADER_FILE%
echo #define GIT_DESCRIBE_PRESENT >> %HEADER_FILE%
echo static const char* GIT_DESCRIBE = "%GIT_DESCRIBE%"; >> %HEADER_FILE%
echo. >> %HEADER_FILE%
echo #endif /* GIT_DESCRIBE_H */ >> %HEADER_FILE%