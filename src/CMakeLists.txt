cmake_minimum_required(VERSION 3.10)
project(MiniAppCalendar)
include(CMakePackageConfigHelpers)

set(QT_MIN_VERSION 
    5.15.0
)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCES
    main.cpp 
    mainwindow.cpp 
    document.cpp 
    month.cpp 
    memodialog.cpp 
    dlgabout.cpp
)

set(UI_FILES 
    dlgabout.ui
    mainwindow.ui
)

set(MOC_HEADERS
    dlgabout.h
    mainwindow.h
)

set(RCS
    MiniAppCalendar.qrc
)

set(CMAKE_AUTOMOC ON) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${QT_INCLUDES})

QT5_WRAP_UI(SOURCES ${UI_FILES})
QT5_ADD_RESOURCES(RC_SRC ${RCS})
add_executable(MiniAppCalendar ${SOURCES} ${UI_SRC} ${UI_FILES}  ${MOC_HEADERS} ${RC_SRC})
target_link_libraries(MiniAppCalendar Qt5::Core Qt5::Widgets Qt5::Gui)
