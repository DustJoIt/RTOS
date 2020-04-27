cmake_minimum_required(VERSION 3.14)
project(Os_2_c__)

set(CMAKE_CXX_STANDARD 14)

add_executable(Os_2_c__ defs.h event.cpp global.cpp os.cpp resource.cpp rtos_api.h sys.h task.cpp test.cpp)