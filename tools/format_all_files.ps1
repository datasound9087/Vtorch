Get-ChildItem -Path .\vtorch -Recurse -Include *.cpp, *.h, *.hpp, *.c |
ForEach-Object {
    C:\Users\Sam\dev\clang-format.exe -i $_.FullName --style=file:.clang-format
    Write-Host "Formatted:" $_.FullName
}