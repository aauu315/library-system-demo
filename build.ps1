$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Path build -Force | Out-Null
    $sources = @('main.cpp') + @(Get-ChildItem src -Filter *.cpp -Recurse | ForEach-Object FullName)
    & g++ '-std=c++17' '-finput-charset=UTF-8' '-fexec-charset=UTF-8' @sources '-o' 'build/library_system.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
    Write-Host 'Built: build/library_system.exe'
} finally { Pop-Location }
