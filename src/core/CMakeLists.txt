add_library(bgl_core)
add_library(bgl::core ALIAS bgl_core)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_sources(bgl_core
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include/bgl
        FILES
            ${PROJECT_SOURCE_DIR}/include/bgl/core/Instance.h
            ${PROJECT_SOURCE_DIR}/include/bgl/core/Launcher.h
            ${PROJECT_SOURCE_DIR}/include/bgl/core/Utility.h
            ${PROJECT_SOURCE_DIR}/include/bgl/core/configuration/Player.hpp
            ${PROJECT_SOURCE_DIR}/include/bgl/core/configuration/PlayerManager.hpp
)

target_sources(bgl_core
    PRIVATE
        FILE_SET
            SOURCES # added in cmake 4.4
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/src/core
        FILES
            ${PROJECT_SOURCE_DIR}/src/core/Instance.cpp
            ${PROJECT_SOURCE_DIR}/src/core/Launcher.cpp
            ${PROJECT_SOURCE_DIR}/src/core/Utility.cpp
            ${PROJECT_SOURCE_DIR}/src/core/configuration/Player.cpp
            ${PROJECT_SOURCE_DIR}/src/core/configuration/PlayerManager.cpp
)

target_link_libraries(bgl_core PRIVATE cpr::cpr)
target_link_libraries(bgl_core PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(bgl_core PRIVATE bgl_third_party::my_uuid)
target_link_libraries(bgl_core PRIVATE bgl_third_party::sha1)
