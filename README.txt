1. Building

Requirements
- CMake
- PJSIP (SIP stack / media stack)
- Boost (Statechart / Thread)
- Curl
 
Win32
    Requirements
    - Platform SDK and DirectX SDK

    1. Build the PJSIP stack
        a. copy config_site_sample.h to config_site.h (in pjlib\include\pj)
        b. open the solution file and set to Release
        c. build
        d. note the "PJTARGET" which is a string that describes the platform PJSIP is built for (for example "i386-win32-vc8-release")

    2. Build Boost 
        a. build it by using bjam
           bjam --toolset=msvc link=static runtime-link=static threading=multi --with-thread stage

    3. Build Curl 
        a. build
           (set the Runtime Library option to Multi-Threaded[/MT])

    4. Build the application
        a. open CMakeLists.txt and ensure that the variable for PJTARGET is correct for your system (for example "i386-win32-vc8-release")
           Also, set the path and filename to the PJSIP, boost and curl libraries at the top of the file.
        b. open the cmake GUI and configure the project
        c. open the solution file and build

Linux
    Requirements
    - libssl-devel
    - libspeex-devel
    - libasound-devel
    - libuuid-devel

    1. Build the PJSIP stack 
        a. change into pjproject/
        b. ./configure && make
        d. note the "PJTARGET" which is a string that describes the platform PJSIP is built for (for example "i386-pc-linux-gnu")

    2. Build the application
        a. open CMakeLists.txt and ensure that the variable for PJTARGET is correct for your system (for example "i386-pc-linux-gnu")
        a. cmake .
        b. make

2. Running
    - copy SLVoice(.exe) to the SecondLife directory, replacing the existing binary file

3. Connecting to a SIP conference
    - in the same directory as the SecondLife executable file, create a file called "slvoice.ini" that has a HTTP server URI which returns a single SIP-domain on the first line.

    For example: http://userserver:8002/?method=voip
    
    - you can also set the preferred codec name in slvoice.ini by adding a second line with the codec's name on it such as speex or speex/8000. On the next line, optionally you can specify whether this codec should be the only one enabled by writing "disable" without the quotes. This is useful for testing with an Asterisk server that allows multiple codecs, however also sets a preference on these codecs. 
In many cases, the server settings might override the codec priority settings on the client.
Please note that these advanced settings will probably CHANGE in a later release.
    
A simple slvoice.ini would look like:
    
http://userserver:8002/?method=voip
    
whereas an slvoice.ini that sets a single preferred codec "speex/8000" would look like the following:
    
http://userserver:8002/?method=voip
speex/8000
disable
    
    
4. Setting up to work with OpenSim
    - ensure that your OpenSim.ini contains the following sections:
        [Voice]
        enabled = false
        account_management_server = https://account-server.example.com
        sip_domain = sip.example.com
        
        [AsteriskVoice]
        enabled = true
        sip_domain = 1.2.3.4
        conf_domain = 1.2.3.4
        asterisk_frontend = http://1.2.3.4:12345/
        asterisk_password = 123456
        asterisk_timeout = 3000
        asterisk_salt = paluempalum
