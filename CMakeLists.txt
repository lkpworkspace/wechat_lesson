cmake_minimum_required(VERSION 3.10)
if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()
project(wechat_lesson VERSION 1.0.2)

#### compile setting
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

#### path setting
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(MYFRAME_ROOT ${CMAKE_INSTALL_PREFIX})
set(MYFRAME_INSTALL_SERVICE_DIR ${MYFRAME_ROOT}/service)
set(MYFRAME_INSTALL_LIB_DIR ${MYFRAME_ROOT}/lib)

#### dep 3rd lib
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(PUGIXML REQUIRED)
find_package(myframe REQUIRED)
find_package(myframe_pb REQUIRED)

#### include directory
include_directories(${Protobuf_INCLUDE_DIRS})

#### genrate mod lib
add_library(${PROJECT_NAME} SHARED
    ${PROJECT_NAME}.cpp
)
target_link_libraries(${PROJECT_NAME}
    Threads::Threads dl
    ${Protobuf_LIBRARIES}
    pugixml
    myframe
    myframe_pb
)

#### install
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${MYFRAME_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${MYFRAME_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${MYFRAME_INSTALL_LIB_DIR}
)
file(GLOB conf_files "*.json")
install(FILES
    ${conf_files}
    PERMISSIONS
    OWNER_READ
    GROUP_READ
    WORLD_READ
    DESTINATION ${MYFRAME_INSTALL_SERVICE_DIR}
)
