@echo off
rem 將主控台編碼切換為 UTF-8
chcp 65001 > nul

echo 正在編譯...
g++ -std=c++17 -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp -o main.exe

if %errorlevel% neq 0 (
    echo 編譯失敗！請檢查程式碼。
    pause
) else (
    echo 編譯成功！正在執行遊戲...
    echo ------------------------------
    main.exe
    pause
)