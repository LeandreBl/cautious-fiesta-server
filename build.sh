#!/bin/bash

if [[ $1 == "debug" ]];
then
    mkdir -p build
    cd build
    conan install .. --build=missing
    cmake -DCMAKE_BUILD_TYPE=DEBUG ..
    cmake --build . -- -j
elif [[ $1 == "rebuild" ]];
then
    rm -rf build
    $0
elif [[ $1 == "windows" ]];
then
    mkdir -p build
    conan install .. --build=missing
    cmake -DCMAKE_BUILD_TYPE=RELEASE ..
    cmake --build . -- -G "Visual Studio 16 2019" -A x64
else
    mkdir -p build
    cd build
    conan install .. --build=missing
    cmake -DCMAKE_BUILD_TYPE=RELEASE ..
    cmake --build . -- -j
fi;
