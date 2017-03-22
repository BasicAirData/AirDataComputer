rem this file convert from .odt file format to markdown
rem file adapted from Naphier reply here https://gist.github.com/vzvenyach/7278543
rem You will need Pandoc to run http://pandoc.org/installing.html
@echo off
echo enter input file
set /p input=":"
echo enter output file
set /p output=":"
pandoc.exe -w markdown_github -o %output% %input%
echo Created %output%
pause