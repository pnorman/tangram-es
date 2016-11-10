add_definitions(-DPLATFORM_ANDROID)

# build external dependencies
add_subdirectory(${PROJECT_SOURCE_DIR}/external)

# load core library
add_subdirectory(${PROJECT_SOURCE_DIR}/core)

set(ANDROID_PROJECT_DIR ${PROJECT_SOURCE_DIR}/android/tangram)

set(LIB_NAME tangram) # in order to have libtangram.so

add_library(${LIB_NAME} SHARED
  ${CMAKE_SOURCE_DIR}/core/common/platform_gl.cpp
  ${CMAKE_SOURCE_DIR}/android/tangram/src/main/cpp/jniExports.cpp
  ${CMAKE_SOURCE_DIR}/android/tangram/src/main/cpp/platform_android.cpp)


# https://code.google.com/p/android/issues/detail?id=68779
# link atomic support, should be fixed after r10e
if (ANDROID_ABI MATCHES armeabi OR
    ANDROID_ABI MATCHES mips)
  set(ATOMIC_LIB atomic)
endif()

target_link_libraries(${LIB_NAME}
  PUBLIC
  ${CORE_LIBRARY}
  # android libaries
  ${ATOMIC_LIB}
  GLESv2 log z android)

target_compile_options(${LIB_NAME}
  PUBLIC
  -fPIC)

