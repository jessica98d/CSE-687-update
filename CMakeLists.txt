cmake_minimum_required(VERSION 3.20)
project(MapReduceSystem LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SHARED_SOURCES
    FileManager.cpp
    Mapper.cpp
    Reducer.cpp
    Workflow.cpp
)

add_executable(mapreduce_gui WIN32
    GuiApp.cpp
    ${SHARED_SOURCES}
)

if (WIN32)
    target_link_libraries(mapreduce_gui PRIVATE user32 gdi32 comdlg32 shell32)
endif()


add_executable(mapreduce_cli
    ${SHARED_SOURCES}
    main_cli.cpp
)


add_executable(MRP2_Phase3_GUI WIN32
    GuiApp_Phase3.cpp
    P3_FileManager.cpp
    P3_Mapper.cpp
    P3_Reducer.cpp
    P3_Logger.cpp
    MapReduceController.cpp
)

if (WIN32)
    target_link_libraries(MRP2_Phase3_GUI PRIVATE user32 gdi32 comdlg32 shell32)
endif()


target_include_directories(mapreduce_gui       PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(mapreduce_cli       PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(MRP2_Phase3_GUI     PRIVATE ${CMAKE_SOURCE_DIR}/include)


if (MSVC)
    target_compile_options(mapreduce_gui       PRIVATE /W3 /MP /permissive-)
    target_compile_options(mapreduce_cli       PRIVATE /W3 /MP /permissive-)
    target_compile_options(MRP2_Phase3_GUI     PRIVATE /W3 /MP /permissive-)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_target_properties(mapreduce_gui       PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(mapreduce_cli       PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(MRP2_Phase3_GUI     PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


add_library(Map SHARED dlls/MapDLL.cpp)
add_library(Reduce SHARED dlls/ReduceDLL.cpp)

target_include_directories(Map    PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Reduce PRIVATE ${CMAKE_SOURCE_DIR}/include)

set_target_properties(Map    PROPERTIES OUTPUT_NAME "Map"    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Reduce PROPERTIES OUTPUT_NAME "Reduce" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


foreach(target mapreduce_cli mapreduce_gui MRP2_Phase3_GUI)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Map>"    "$<TARGET_FILE_DIR:${target}>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Reduce>" "$<TARGET_FILE_DIR:${target}>/"
    )
endforeach()


foreach(target mapreduce_gui mapreduce_cli MRP2_Phase3_GUI)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/sample_input"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/temp"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/output"
    )
endforeach()


target_compile_definitions(mapreduce_gui       PRIVATE MR_PHASE2_AVAILABLE)
target_compile_definitions(mapreduce_cli       PRIVATE MR_PHASE2_AVAILABLE)
target_compile_definitions(MRP2_Phase3_GUI     PRIVATE MR_PHASE2_AVAILABLE)



