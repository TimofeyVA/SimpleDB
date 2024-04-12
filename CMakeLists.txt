cmake_minimum_required(VERSION 3.0.0)
project(SimpleBD VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(SOURCES src/main.c 
            src/server.c
            src/db_manager.c)

set(INCLUDES includes/server.h
             includes/db_manager.h)



include_directories(includes/)

add_executable(SimpleBD ${INCLUDES} ${SOURCES})

set_property(TARGET SimpleBD PROPERTY C_STANDARD 99)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
