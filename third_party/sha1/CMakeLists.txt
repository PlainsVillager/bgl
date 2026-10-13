add_library(bgl_third_party_sha1 INTERFACE)
add_library(bgl_third_party::sha1 ALIAS bgl_third_party_sha1)

target_sources(bgl_third_party_sha1
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}
        FILES
            ${PROJECT_SOURCE_DIR}/third_party/sha1/sha1.hpp
)
