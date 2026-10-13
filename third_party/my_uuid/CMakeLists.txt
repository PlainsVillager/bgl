add_library(bgl_third_party_my_uuid INTERFACE)
add_library(bgl_third_party::my_uuid ALIAS bgl_third_party_my_uuid)

target_sources(bgl_third_party_my_uuid
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}
        FILES
            ${PROJECT_SOURCE_DIR}/third_party/my_uuid/my_uuid.hpp
)
