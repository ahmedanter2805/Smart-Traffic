cmake_minimum_required(VERSION 3.16)

project(Smart-Traffic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

# Include directories
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # For generated ui_*.h files

# Sources and Headers
set(SOURCES
    main.cpp
    mainwindow.cpp
    src/Vehicle.cpp
    src/Road.cpp
    src/TrafficLight.cpp
    src/Intersection.cpp
    src/TrafficStrategy.cpp
    src/TrafficController.cpp
    src/Statistics.cpp
    src/AITrafficOptimizer.cpp
)

set(HEADERS
    mainwindow.h
    include/Vehicle.h
    include/Road.h
    include/TrafficLight.h
    include/Intersection.h
    include/TrafficStrategy.h
    include/TrafficController.h
    include/Statistics.h
    include/AITrafficOptimizer.h
    include/HardwareBridge.h
)

set(FORMS
    mainwindow.ui
)

set(RESOURCES
    resources.qrc
)

# Define the executable
add_executable(Smart-Traffic 
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# Link with Qt libraries
target_link_libraries(Smart-Traffic PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
