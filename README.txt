1. Building

Requirements
- CMake
- Boost (Statechart -- header-based library)
 
Win32
    Requirements
    - Platform SDK and DirectX SDK

    1. Build the PJSIP stack
        - copy config_site_sample.h to config_site.h (in pjlib\include\pj)
        - open the solution file and set to Release
        - build
        
    2. Build the application
        - open the cmake GUI and configure the project
        - open the solution file and build

Linux
    Requirements
    - libssl-devel
    - libspeex-devel
    - libasound-devel
    - libuuid-devel

    1. Build the PJSIP stack 
        a. change into pjproject/
        b. ./configure && make

    2. Build the application
        a. cmake .
        b. make
