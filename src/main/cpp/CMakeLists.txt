#[[

cmake_minimum_required(VERSION 3.4.1)


set(libjpeg_dir ${CMAKE_SOURCE_DIR}/libjpeg)
set(INC_DIR ${libjpeg_dir}/include)
set(libjpeg_lib_dir ${libjpeg_dir}/prebuilt)

#添加头文件目录
include_directories(${INC_DIR})

set(SOURCE_FILES src/main/cpp/compress.c)

add_library(compress SHARED
            ${SOURCE_FILES})

find_library(log-lib log)
find_library(graphics jnigraphics)

add_library(libjpeg SHARED IMPORTED)
set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${libjpeg_lib_dir}/${ANDROID_ABI}/libjpeg.so)



target_link_libraries(compress libjpeg ${log-lib} ${graphics})
]]
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(libjpeg_dir ${CMAKE_SOURCE_DIR}/..)
set(INC_DIR ${libjpeg_dir}/cpp/include)
set(libjpeg_lib_dir ${libjpeg_dir}/jniLibs)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        compress

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        compress.c)

target_include_directories(compress
        PRIVATE
        ${INC_DIR})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )


find_library(graphics jnigraphics)

add_library(libjpeg SHARED IMPORTED)
set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${libjpeg_lib_dir}/${ANDROID_ABI}/libjpeg.so)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        compress
        libjpeg

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${graphics})

