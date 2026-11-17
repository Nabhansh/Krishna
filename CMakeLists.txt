cmake_minimum_required(VERSION 3.20)

project(Project31 VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project31_core
    src/core/Airport.cpp
    src/core/Passenger.cpp
    src/core/PassengerIndex.cpp
    src/core/SecurityLane.cpp
    src/core/SecurityOfficer.cpp
    src/algorithms/BinarySearch.cpp
    src/algorithms/ActivitySelection.cpp
    src/algorithms/CelebrityProblem.cpp
    src/algorithms/ClassicalMatrixMultiplication.cpp
    src/algorithms/Dijkstra.cpp
    src/algorithms/FastExponentiation.cpp
    src/algorithms/HuffmanCoding.cpp
    src/algorithms/IntervalPartitioning.cpp
    src/algorithms/Kruskal.cpp
    src/algorithms/MaximumSubarray.cpp
    src/algorithms/MergeSort.cpp
    src/algorithms/Prim.cpp
    src/algorithms/QuickSort.cpp
    src/algorithms/StrassenMatrixMultiplication.cpp
    src/simulator/QueueManager.cpp
    src/simulator/AirportOperations.cpp
    src/simulator/QueueWorkloadIndex.cpp
)
target_include_directories(project31_core PUBLIC include)
target_compile_options(project31_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

add_executable(project31
    src/main.cpp
    src/simulator/CommandLineInterface.cpp
)
target_link_libraries(project31 PRIVATE project31_core)
target_compile_options(project31 PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(project31_tests
        tests/unit/passenger_tests.cpp
        tests/unit/sorting_tests.cpp
        tests/unit/searching_tests.cpp
        tests/unit/index_tests.cpp
        tests/unit/divide_conquer_tests.cpp
        tests/unit/greedy_tests.cpp
        tests/integration/airport_tests.cpp
    )
    target_link_libraries(project31_tests PRIVATE project31_core GTest::gtest_main)
    target_compile_options(project31_tests PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    )
    include(GoogleTest)
    gtest_discover_tests(project31_tests)
endif()

add_executable(project31_batch_a_example examples/batch_a_example.cpp)
target_link_libraries(project31_batch_a_example PRIVATE project31_core)
target_compile_options(project31_batch_a_example PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

add_executable(project31_greedy_example examples/greedy_example.cpp)
target_link_libraries(project31_greedy_example PRIVATE project31_core)
target_compile_options(project31_greedy_example PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

option(PROJECT31_BUILD_BENCHMARKS "Build sorting performance benchmarks" ON)
if(PROJECT31_BUILD_BENCHMARKS)
    add_executable(project31_sorting_benchmark benchmarks/sorting_benchmark.cpp)
    target_link_libraries(project31_sorting_benchmark PRIVATE project31_core)
    target_compile_options(project31_sorting_benchmark PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    )
endif()
