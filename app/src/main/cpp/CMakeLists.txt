# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.4.1)

# Declares the project name. The project name can be accessed via ${PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("barge")


set(JDK_PATH "/Library/Java/JavaVirtualMachines/jdk-21.jdk/Contents/Home/include/")
include_directories(${JDK_PATH}/include)
include_directories(${JDK_PATH}/include/darwin) # For macOS
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
#add_library(${CMAKE_PROJECT_NAME} SHARED
    # List C/C++ source files with relative paths to this CMakeLists.txt.
    #barge.cpp
    # Add your ComputeShaderSetup.cpp here
    #ComputeShaderSetup.cpp easyvk.cpp cpu_MatrixMultiplication.cpp)

add_library(
        barge
        SHARED
        cpu_MatrixMultiplication.cpp easyvk.cpp ComputeShaderSetup.cpp
)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
    # List libraries to link to the target library
    android
    log
    # Link against Vulkan library
    vulkan)

# Include Vulkan headers from the NDK
find_library(VULKAN_LIB vulkan)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${VULKAN_LIB}/include)

# If there are specific Vulkan features or extensions you need, you might also need
# to check for their availability and conditionally compile code based on that.
# This is just a basic setup to get started with Vulkan in an Android CMake project.

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

