g++ main.cpp src/Vehicle.cpp src/Road.cpp src/TrafficLight.cpp src/Intersection.cpp src/TrafficStrategy.cpp src/TrafficController.cpp src/Statistics.cpp -I ./include -o SmartTraffic.exe
if ($?) {
    .\SmartTraffic.exe
} else {
    Write-Host "Compilation failed. Make sure g++ is installed and in your PATH." -ForegroundColor Red
}
