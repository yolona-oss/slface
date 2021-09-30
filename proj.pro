#-----------------------------------------------------------------
# Project converted from qmake file using q2c
# https://github.com/benapetr/q2c at Thu Sep 30 15:35:23 2021
#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.6)
project()
option(QT5BUILD "Build using Qt5 libs" TRUE)
IF (QT5BUILD)
    find_package(Qt5Core REQUIRED)
    set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS})
    include_directories(${QT_INCLUDES})
    QT5_WRAP_CPP(_HEADERS_MOC ${HuggleLite_HEADERS})
ELSE()
    find_package(Qt4 REQUIRED)
ENDIF()
add_executable()
IF (QT5BUILD)
    qt5_use_modules( Core)
ENDIF()
