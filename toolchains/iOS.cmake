include(${CMAKE_SOURCE_DIR}/toolchains/iOS.toolchain.cmake)

add_definitions(-DPLATFORM_IOS)

set(LIBRARY_NAME "tangram")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}
    -fobjc-abi-version=2
    -fobjc-arc
    -std=c++1y
    -stdlib=libc++
    -isysroot ${CMAKE_IOS_SDK_ROOT}")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}
    -fobjc-abi-version=2
    -fobjc-arc
    -isysroot ${CMAKE_IOS_SDK_ROOT}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mios-simulator-version-min=6.0")
set(ARCH "i386 armv7 armv7s arm64")

set(FRAMEWORKS CoreGraphics CoreFoundation QuartzCore UIKit OpenGLES Security CFNetwork GLKit)

# load core library
add_subdirectory(${PROJECT_SOURCE_DIR}/external)
add_subdirectory(${PROJECT_SOURCE_DIR}/core)

# add ios platform source files
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/ios/src/platform_ios.mm")

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/ios/tangram_framework")

target_link_libraries(${LIBRARY_NAME} "-Wl,-whole-archive" ${CORE_LIBRARY} "-Wl,-no-whole-archive")

# setting xcode properties
set_xcode_property(${LIBRARY_NAME} SUPPORTED_PLATFORMS "iphonesimulator iphoneos")
set_xcode_property(${LIBRARY_NAME} ONLY_ACTIVE_ARCH "YES")
set_xcode_property(${LIBRARY_NAME} VALID_ARCHS "${ARCH}")
set_xcode_property(${LIBRARY_NAME} TARGETED_DEVICE_FAMILY "1,2")

macro(add_framework FWNAME APPNAME LIBPATH)
    find_library(FRAMEWORK_${FWNAME} NAMES ${FWNAME} PATHS ${LIBPATH} PATH_SUFFIXES Frameworks NO_DEFAULT_PATH)
    if(${FRAMEWORK_${FWNAME}} STREQUAL FRAMEWORK_${FWNAME}-NOTFOUND)
        message(ERROR ": Framework ${FWNAME} not found")
    else()
        target_link_libraries(${APPNAME} ${FRAMEWORK_${FWNAME}})
        message(STATUS "Framework ${FWNAME} found")
    endif()
endmacro(add_framework)

foreach(_framework ${FRAMEWORKS})
  add_framework(${_framework} ${LIBRARY_NAME} ${CMAKE_SYSTEM_FRAMEWORK_PATH})
endforeach()
