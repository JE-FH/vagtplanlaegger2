@echo off
@chcp 65001>nul
gcc -Og -ansi -Wall -pedantic main.c --data-sections -ffunction-sections -g -o a.exe