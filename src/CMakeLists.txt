# qtsass
file(REAL_PATH "." CURRENT_DIR)
file(REAL_PATH ".." PARENT_DIR)

if (QTSASS_SHARED_LIBRARY)
    add_library(qtsass SHARED)
    target_compile_definitions(qtsass PRIVATE QTSASS_SHARED_LIBRARY)
else()
    add_library(qtsass STATIC)
    target_compile_definitions(qtsass PUBLIC QTSASS_STATIC_LIBRARY)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(qtsass PUBLIC Qt::Core)

# libsass
find_package(libsass REQUIRED)
target_link_libraries(qtsass PUBLIC libsass)

target_include_directories(qtsass PUBLIC ${CURRENT_DIR})
file(GLOB_RECURSE QTSASS_SRC_FILES RELATIVE ${CURRENT_DIR}
    "${CURRENT_DIR}/*.cpp"
    "${CURRENT_DIR}/*.h"
)
target_sources(qtsass PRIVATE ${QTSASS_SRC_FILES})
