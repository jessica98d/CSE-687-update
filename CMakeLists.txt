cmake_minimum_required(VERSION 3.20)
project(MapReduceSystem LANGUAGES CXX)

# ----------------------------------------------------------
# Compiler / language settings
# ----------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# ----------------------------------------------------------
# Phase 1/2 core sources (mr:: namespace)
# ----------------------------------------------------------
set(MR_CORE_SOURCES
    FileManager.cpp
    Mapper.cpp
    Reducer.cpp
    Workflow.cpp
)

# GUI for Phases 1/2 (ASCII table)
add_executable(mapreduce_gui WIN32
    GuiApp.cpp
    ${MR_CORE_SOURCES}
)

# CLI – uses the Phase 3 controller in main_cli.cpp,
# but it is fine to also compile the Phase 1/2 core sources.
add_executable(mapreduce_cli
    ${MR_CORE_SOURCES}
    P3_FileManager.cpp
    P3_Mapper.cpp
    P3_Reducer.cpp
    P3_Logger.cpp
    MapReduceController.cpp
    main_cli.cpp

)

# Link user32 / GDI etc for the Win32 GUIs
if (WIN32)
    target_link_libraries(mapreduce_gui PRIVATE user32 gdi32 comdlg32 shell32)
endif()

# ----------------------------------------------------------
# Phase 3 GUI (multi-threaded controller)
# ----------------------------------------------------------
set(P3_SOURCES
    P3_FileManager.cpp
    P3_Mapper.cpp
    P3_Reducer.cpp
    P3_Logger.cpp
    MapReduceController.cpp
)

add_executable(MRP2_Phase3_GUI WIN32
    GuiApp_Phase3.cpp
    ${P3_SOURCES}
)

if (WIN32)
    target_link_libraries(MRP2_Phase3_GUI PRIVATE user32 gdi32 comdlg32 shell32)
endif()

# ----------------------------------------------------------
# Include paths
# ----------------------------------------------------------
# All targets need access to the public mr/ headers
target_include_directories(mapreduce_gui   PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(mapreduce_cli   PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(MRP2_Phase3_GUI PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ----------------------------------------------------------
# Warnings / MSVC options
# ----------------------------------------------------------
if (MSVC)
    target_compile_options(mapreduce_gui   PRIVATE /W3 /MP /permissive-)
    target_compile_options(mapreduce_cli   PRIVATE /W3 /MP /permissive-)
    target_compile_options(MRP2_Phase3_GUI PRIVATE /W3 /MP /permissive-)
endif()

# ----------------------------------------------------------
# Output layout
# ----------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_target_properties(mapreduce_gui   PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(mapreduce_cli   PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(MRP2_Phase3_GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ----------------------------------------------------------
# Phase 2 plugins (Map.dll / Reduce.dll)
# ----------------------------------------------------------
add_library(Map SHARED dlls/MapDLL.cpp)
add_library(Reduce SHARED dlls/ReduceDLL.cpp)

target_include_directories(Map    PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Reduce PRIVATE ${CMAKE_SOURCE_DIR}/include)

set_target_properties(Map    PROPERTIES OUTPUT_NAME "Map"    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Reduce PROPERTIES OUTPUT_NAME "Reduce" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ----------------------------------------------------------
# After build: copy DLLs next to each EXE
# ----------------------------------------------------------
foreach(target mapreduce_cli mapreduce_gui MRP2_Phase3_GUI)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Map>"    "$<TARGET_FILE_DIR:${target}>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Reduce>" "$<TARGET_FILE_DIR:${target}>/"
    )
endforeach()

# ----------------------------------------------------------
# After build: ensure expected folder layout for input/output
# ----------------------------------------------------------
foreach(target mapreduce_gui mapreduce_cli MRP2_Phase3_GUI)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/sample_input"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/temp"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/output"
    )
endforeach()

# ----------------------------------------------------------
# Enable the Phase-2 code paths in Workflow (mr namespace).
# These macros only affect the mr::Workflow implementation;
# they do not change the Phase-3 controller at all.
# ----------------------------------------------------------
target_compile_definitions(mapreduce_gui PRIVATE MR_PHASE2_AVAILABLE)
target_compile_definitions(mapreduce_cli PRIVATE MR_PHASE2_AVAILABLE)


