rem this file convert from markdown file format to odt
rem file adapted from Naphier reply here https://gist.github.com/vzvenyach/7278543
rem You will need Pandoc to run http://pandoc.org/installing.html
@echo off
echo enter input file
set /p input=":"
echo enter output file
set /p output=":"
pandoc.exe -s %input% -o %output%
echo Created %output%
pause