cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies wh
project("hilcodec")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc++_shared")
# 헤더 파일 디렉토리 설정
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(android_lyra SHARED IMPORTED)
# encoder_main_lib.so의 위치를 설정

link_directories(${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI})

set_target_properties(android_lyra PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libandroid_lyra.so"
        IMPORTED_SONAME "libandroid_lyra.so"
)

add_library(lyrawrapper SHARED lyraJNIWrapper.cpp)

# log 라이브러리 찾기
find_library(log-lib log)


# 라이브러리 링크 설정
#1)
#android_lyra
#2)
#-landroid_lyra

target_link_libraries(lyrawrapper
        PRIVATE
#        android_lyra
        -landroid_lyra
        ${log-lib}
        atomic
)
