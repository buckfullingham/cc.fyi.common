find_package(unordered_dense REQUIRED)

add_library(common
        io.cpp
)

target_include_directories(common PUBLIC
        ../../include
        ${unordered_dense_INCLUDE_DIRS}
)

target_link_libraries(common PRIVATE
        ${unordered_dense_LIBRARIES}
)
