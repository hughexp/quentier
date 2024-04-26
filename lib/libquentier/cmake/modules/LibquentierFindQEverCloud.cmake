find_package(QEverCloud-qt5 QUIET REQUIRED)

include_directories(${QEVERCLOUD_INCLUDE_DIRS})

get_property(QEVERCLOUD_LIBRARY_LOCATION TARGET ${QEVERCLOUD_LIBRARIES} PROPERTY LOCATION)
message(STATUS "Found QEverCloud library: ${QEVERCLOUD_LIBRARY_LOCATION}")

get_filename_component(QEVERCLOUD_LIB_DIR "${QEVERCLOUD_LIBRARY_LOCATION}" PATH)