$ErrorActionPreference = "Stop"

Write-Host "================================================================================"

Write-Host "Compiling..."
g++ src/zipper.cpp -Iinclude -o src/zipper.exe
Write-Host "Done Compiling!"

Write-Host "Running tests..."

$sw = [System.Diagnostics.Stopwatch]::StartNew()

Write-Host "Adventures of Sherlock Holmes.txt..."
./src/zipper.exe `
    "examples/Adventures of Sherlock Holmes/Adventures of Sherlock Holmes.txt" "Adventures of Sherlock Holmes.txt"
Move-Item *.txt, *.bin -Destination "examples/Adventures of Sherlock Holmes/" -Force
Write-Host "================================================================================"

Write-Host "The Tempest.txt..."
./src/zipper.exe `
    "examples/The Tempest/The Tempest.txt" "The Tempest.txt"
Move-Item *.txt, *.bin -Destination "examples/The Tempest/" -Force
Write-Host "================================================================================"

Write-Host "Random.txt..."
./src/zipper.exe `
    "examples/random/random.txt" "random.txt"
Move-Item *.txt, *.bin -Destination "examples/random/" -Force
Write-Host "================================================================================"

Write-Host "Cat Picture..."
./src/zipper.exe `
    "examples/Images/no filter/catPic.bmp" "catPic.bmp"
Move-Item *.bmp, *.bin -Destination "examples/Images/no filter/" -Force
Write-Host "================================================================================"

Write-Host "Cat Picture (Sobel filtered)..."
./src/zipper.exe `
    "examples/Images/filtered/catPicWithSobel.bmp" "catPicWithSobel.bmp"
Move-Item *.bmp, *.bin -Destination "examples/Images/filtered/" -Force
Write-Host "================================================================================"

$sw.Stop()
Write-Host ("Total Time: {0:N3} seconds" -f $sw.Elapsed.TotalSeconds)
Write-Host "All tests done!"
Write-Host "================================================================================"