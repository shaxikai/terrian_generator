cmake_minimum_required(VERSION 2.8)

project (TerrianGenerator)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_module)

find_package(Qt4 REQUIRED QtGui QtCore QtOpengl QtXml)
message("-- find Qt4" ${QT_LIBRARIES})
include(${QT_USE_FILE})

find_package(OPENGL REQUIRED QTIET)
message("-- find OpenGL")

find_package(QGLViewer REQUIRED QUIET)
message("-- find QGLViewer ${QGLVIEWER_LIBRARIES}")

find_package(OpenCV REQUIRED QUIET)
message("-- find OpenCV" ${OpenCV_LIBRARIES})

INCLUDE_DIRECTORIES (BEFORE ./src/)



INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${GLOG_INCLUDE_DIRS}
        ${QGLVIEWER_INCLUDES}
        ${OpenCV_INCLUDES}
        )


SET(SOURCES_FILES
    ./src/main.cpp
    ./src/terrian.cpp
    ./src/viewer.cpp
    ./src/quadtree.cpp
    ./src/sky.cpp
    ./src/cameraViewer.cpp
    ./src/standardCamera.cpp
    ./src/mainWidget.cpp
    )

LIST(APPEND LINK_LIBS
        ${QT_LIBRARIES}
        ${GSLAM_LIBRARIES}
        ${GLOG_LIBRARIES}
        ${QGLVIEWER_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBRARIES}
        )

set(AUTOMOC true)
QT4_WRAP_CPP(example_MOCS ./src/viewer.h)

add_executable (TerrianGenerator ${SOURCES_FILES} ${example_MOCS})

target_link_libraries( TerrianGenerator ${LINK_LIBS})





