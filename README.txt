1. Building

Requirements
- CMake
- Boost (Statechart -- header-based library)
 
Linux
    Requirements
    - Bash
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
