find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

add_executable(tests
        io.cpp
        util.cpp
)

target_include_directories(tests PRIVATE
        ../../include
)

target_link_libraries(tests PRIVATE
        catch2::catch2_with_main
        common
)

catch_discover_tests(tests)
