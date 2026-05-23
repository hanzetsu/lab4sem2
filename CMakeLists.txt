cmake_minimum_required(VERSION 3.16)
project(Lab4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lab2sem2/include
)

set(SOURCES
    source/main.cpp
    source/MainWindow.cpp
    source/StatisticsWidget.cpp
    source/SubstringSearchWidget.cpp
)

set(HEADERS
    include/MainWindow.hpp
    include/IStream.hpp
    include/Cardinal.hpp
    include/Generator.hpp
    include/LazySequence.hpp
    include/LazySequence.tpp
    include/MapGenerator.hpp
    include/MapGenerator.tpp
    include/WhereGenerator.hpp
    include/WhereGenerator.tpp
    include/CompositeGenerator.hpp
    include/CompositeGenerator.tpp
    include/RecurrentGenerator.hpp
    include/RecurrentGenerator.tpp
    include/SequenceGenerator.hpp
    include/SequenceGenerator.tpp
    include/StreamStatistics.hpp
    include/SubstringCounter.hpp
    include/FileStream.hpp
    include/ArrayStream.hpp
    include/StringStream.hpp
    include/StreamAlgorithms.hpp
    include/StatisticsWidget.hpp
    include/SubstringSearchWidget.hpp
    include/LazySequenceStream.hpp
    include/Option.hpp
)

add_executable(Lab4 ${SOURCES} ${HEADERS})
set_target_properties(Lab4 PROPERTIES WIN32_EXECUTABLE OFF)
target_link_libraries(Lab4 Qt6::Widgets)


add_executable(test_manual tests/test_manual.cpp)
target_link_libraries(test_manual ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(test_manual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(test_manual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lab2sem2/include)

if(WIN32)
    target_link_options(Lab4 PRIVATE -mconsole)
endif()

enable_testing()

add_executable(test_lazysequence tests/test_lazysequence.cpp)
target_link_libraries(test_lazysequence)
target_include_directories(test_lazysequence PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(test_lazysequence PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lab2sem2/include)

add_executable(test_stream tests/test_stream.cpp)
target_link_libraries(test_stream)
target_include_directories(test_stream PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(test_stream PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lab2sem2/include)
add_test(NAME test_lazysequence COMMAND test_lazysequence)
add_test(NAME test_stream COMMAND test_stream)