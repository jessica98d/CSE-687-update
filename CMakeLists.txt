cmake_minimum_required(VERSION 3.20)
project(mapreduce_gui LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)


set(SHARED_SOURCES
    FileManager.cpp
    Mapper.cpp
    Reducer.cpp
    Workflow.cpp
)


add_executable(mapreduce_gui WIN32 GuiApp.cpp ${SHARED_SOURCES})
if (WIN32)
    target_link_libraries(mapreduce_gui PRIVATE user32 gdi32 comdlg32 shell32)
endif()


add_executable(mapreduce_cli ${SHARED_SOURCES} main_cli.cpp)

# Common options
target_include_directories(mapreduce_gui PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(mapreduce_cli PRIVATE ${CMAKE_SOURCE_DIR}/include)

if (MSVC)
    target_compile_options(mapreduce_gui PRIVATE /W3 /MP /permissive-)
    target_compile_options(mapreduce_cli  PRIVATE /W3 /MP /permissive-)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(mapreduce_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(mapreduce_cli  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


# Build shared libraries (Windows DLLs)
add_library(Map SHARED dlls/MapDLL.cpp)
add_library(Reduce SHARED dlls/ReduceDLL.cpp)


target_include_directories(Map    PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Reduce PRIVATE ${CMAKE_SOURCE_DIR}/include)

set_target_properties(Map    PROPERTIES OUTPUT_NAME "Map"    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Reduce PROPERTIES OUTPUT_NAME "Reduce" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_custom_command(TARGET mapreduce_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Map>"    "$<TARGET_FILE_DIR:mapreduce_cli>/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Reduce>" "$<TARGET_FILE_DIR:mapreduce_cli>/"
)
add_custom_command(TARGET mapreduce_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Map>"    "$<TARGET_FILE_DIR:mapreduce_gui>/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Reduce>" "$<TARGET_FILE_DIR:mapreduce_gui>/"
)


add_custom_command(TARGET mapreduce_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_gui>/sample_input"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_gui>/temp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_gui>/output"
)

add_custom_command(TARGET mapreduce_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_cli>/sample_input"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_cli>/temp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:mapreduce_cli>/output"
)
# GUI target (Win32)
add_executable(mapreduce_gui WIN32
    GuiApp.cpp
    FileManager.cpp
    Mapper.cpp
    Reducer.cpp
    Workflow.cpp
)
target_link_libraries(mapreduce_gui PRIVATE user32 comdlg32 shell32)
set_target_properties(mapreduce_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

