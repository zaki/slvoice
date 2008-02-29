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

        Locally
        a. change into pjproject/
        b. ./configure && make
        c. make DESTDIR=/path/to/voiceforvw/ install
        d. ./fixpjlinks.sh ./local/path/to/libs

        System
        a. change into pjproject/
        b. configure && make
        c. sudo make install
        d. ./fixpjlinks.sh /usr/local/lib

    2. Build the application
        a. cmake .
        b. make
