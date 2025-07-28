@echo off
call .\compilers\msvc420\bin\VCVARS32.BAT x86
del out\code.asm.txt
del out\code.map.txt
cl /c /O2 /GX /I msvc420\include %1 /Faout\code.asm.txt /Fmout\code.map.txt
