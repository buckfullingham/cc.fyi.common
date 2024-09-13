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
        Catch2::Catch2WithMain
        common
        ${CMAKE_DL_LIBS}
)

catch_discover_tests(tests)
