INCLUDE (CheckLibraryExists)
INCLUDE (CheckIncludeFiles)

PROJECT (voiceforvw)

SET (CMAKE_BUILD_TYPE Debug)

# settings of third-party libs

# pjsip
SET (PJDIR "C:/path/to/pjproject-1.2")

# curl
SET (CURLDIR "C:/path/to/curl-7.19.5")
SET (CURLINCDIR ${CURLDIR}/include)
SET (CURLLIBDIR ${CURLDIR}/lib/LIB-Release)
SET (CURLLIBS ${CURLLIBDIR}/libcurl.lib)

# boost
SET (BOOSTDIR "C:/path/to/boost_1_39_0")
SET (BOOSTINCDIR ${BOOSTDIR})
SET (BOOSTLIBDIR ${BOOSTDIR}/stage/lib)
SET (BOOSTLIBS ${BOOSTLIBDIR}/libboost_thread-vc90-mt-s.lib)

SET (EXTLIBDIR ${CURLLIBDIR} ${BOOSTLIBDIR})
SET (EXTINCDIR ${CURLINCDIR} ${BOOSTINCDIR})
SET (EXTLIBS ${CURLLIBS} ${BOOSTLIBS})

# included source variables
SET (VOICEDIR ${CMAKE_SOURCE_DIR}/voiceforvw)
SET (SOCKETDIR ${CMAKE_SOURCE_DIR}/sockets)
SET (TINYXMLDIR ${CMAKE_SOURCE_DIR}/tinyxml)

SET (VOICESRCDIR ${VOICEDIR}/src)

SET (VOICESRC 
    ${VOICESRCDIR}/main/main.cpp
    ${VOICESRCDIR}/config/config.cpp
    ${VOICESRCDIR}/logger/logger.cpp
    ${VOICESRCDIR}/sip/sip.cpp
    ${VOICESRCDIR}/server/server.cpp
    ${VOICESRCDIR}/server/server_util.cpp
    ${VOICESRCDIR}/state/connect_state.cpp
    ${VOICESRCDIR}/state/account_state.cpp
    ${VOICESRCDIR}/state/session_state.cpp
    ${VOICESRCDIR}/manage/base_manager.cpp
    ${VOICESRCDIR}/manage/account_manager.cpp
    ${VOICESRCDIR}/manage/session_manager.cpp
    ${VOICESRCDIR}/manage/event_manager.cpp
    ${VOICESRCDIR}/parsing/parsing.cpp
    ${VOICESRCDIR}/parsing/parameters.cpp)

SET (VOICEINCDIR ${VOICEDIR}/include)

SET (VOICEINC 
	${VOICEINCDIR}/main.h
	${VOICEINCDIR}/config.hpp
	${VOICEINCDIR}/logger.hpp
	${VOICEINCDIR}/parameters.hpp 
	${VOICEINCDIR}/parsing.hpp 
	${VOICEINCDIR}/server.hpp 
	${VOICEINCDIR}/server_util.hpp 
	${VOICEINCDIR}/sip.hpp 
	${VOICEINCDIR}/event.hpp 
	${VOICEINCDIR}/state.hpp)

SET (SOCKETSRC 
    ${SOCKETDIR}/Sockets.cpp)

SET (TINYXMLSRC 
    ${TINYXMLDIR}/tinyxml.cpp 
    ${TINYXMLDIR}/tinyxmlerror.cpp
    ${TINYXMLDIR}/tinyxmlparser.cpp 
    ${TINYXMLDIR}/tinystr.cpp )

# where to find PJSIP files locally
FILE (GLOB PJINCLDIR ${PJDIR}/*/include)
FILE (GLOB PJLIBDIR ${PJDIR}/*/lib)

# cause tinyxml to use the STL
ADD_DEFINITIONS (-DTIXML_USE_STL)

# platform specific area

# Windows
IF (WIN32)
    ADD_DEFINITIONS(-DPJ_WIN32=1)
    
    # necessary system-level libraries
    SET (PJTARGET i386-win32-vc8-release)
    SET (LIBS ${LIBS} wsock32.lib ws2_32.lib winmm.lib wldap32.lib)

    SET(CMAKE_CXX_FLAGS "/D WIN32 /D CURL_STATICLIB /D BOOST_ALL_NO_LIB /nologo /W3 /Gy /EHsc")
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")

    # you can use MT, if pjsip is built with MT option.
    SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
#    SET(CMAKE_CXX_FLAGS_RELEASE "/MD /O2")

ENDIF (WIN32)

# Unix-likes
IF (UNIX)

    # MacOS
    IF (APPLE)
        ADD_DEFINITIONS (-DPJ_AUTOCONF=0)
        SET (PJTARGET i386-apple-darwin8.11.1)
    ELSE (APPLE)

    # Linux
        ADD_DEFINITIONS (-DPJ_AUTOCONF=1)
        SET (PJTARGET i686-pc-linux-gnu)
    ENDIF (APPLE)

    # necessary system-level libraries
    SET (LIBS ${LIBS} m uuid nsl rt pthread asound ssl crypto)

    # FIND_LIBRARY unfortunately prefers dynamic over static
    # with no option to override. We break out the sledgehammer.
    #FOREACH (L ${LIBS})
    #    STRING (REPLACE ".so" ".a" SL ${L})
    #    SET (STATIC_LIBS ${STATIC_LIBS} ${SL})
    #ENDFOREACH (L)
ENDIF (UNIX)
    
# PJSIP is inconsistent in naming across platforms
MACRO (FIND_PJSIP_LIBRARY N P)
    FIND_LIBRARY (${N}_LIB 
        NAMES 
        ${N} 
        "${N}-${PJTARGET}" 
        "lib${N}" 
        "lib${N}-${PJTARGET}"
        "${N}-lib"
        "${N}-lib-${PJTARGET}"
        "${N}lib"
        "${N}lib-${PJTARGET}"
        "${N}-core"
        "${N}-core-${PJTARGET}"
        PATHS ${PJDIR}/${P}/lib)
    SET (PJLIBS ${PJLIBS} ${${N}_LIB})
ENDMACRO (FIND_PJSIP_LIBRARY)

# included PJSIP libraries
FIND_PJSIP_LIBRARY (pjsua pjsip)
FIND_PJSIP_LIBRARY (pjsip-ua pjsip)
FIND_PJSIP_LIBRARY (pjsip-simple pjsip)
FIND_PJSIP_LIBRARY (pjsip pjsip)
FIND_PJSIP_LIBRARY (pjmedia-codec pjmedia)
FIND_PJSIP_LIBRARY (pjmedia pjmedia)
FIND_PJSIP_LIBRARY (pjmedia-audiodev pjmedia)
FIND_PJSIP_LIBRARY (pjnath pjnath)
FIND_PJSIP_LIBRARY (pjlib-util pjlib-util)
FIND_PJSIP_LIBRARY (pj pjlib)
FIND_PJSIP_LIBRARY (resample third_party)
FIND_PJSIP_LIBRARY (milenage third_party)
FIND_PJSIP_LIBRARY (gsmcodec third_party)
FIND_PJSIP_LIBRARY (ilbccodec third_party)
FIND_PJSIP_LIBRARY (portaudio third_party)
FIND_PJSIP_LIBRARY (speex third_party)
FIND_PJSIP_LIBRARY (srtp third_party)
FIND_PJSIP_LIBRARY (g7221codec third_party)

# find headers
FIND_FILE (PJINCL 
    NAMES "pjsua-lib/pjsua.h" 
    PATHS "${PJDIR}/pjsip/include" ENV INCLUDE)

# aggregate the variables
SET (SRC ${VOICESRC} ${SOCKETSRC} ${TINYXMLSRC})
SET (INC ${VOICEINC})

SET (LIBS ${LIBS} ${PJLIBS} ${EXTLIBS})

SET (INCLDIR ${VOICEINCDIR} ${PJINCLDIR} ${CMAKE_SOURCE_DIR} ${EXTINCDIR})
SET (LIBDIR ${PJLIBDIR} ${EXTLIBDIR})

# set the build
INCLUDE_DIRECTORIES (${INCLDIR})
LINK_DIRECTORIES (${LIBDIR})

IF (WIN32)
    ADD_EXECUTABLE (SLVoice WIN32 ${SRC} ${INC})
ENDIF (WIN32)
IF (UNIX)
    ADD_EXECUTABLE (SLVoice ${SRC} ${INC})
ENDIF (UNIX)

TARGET_LINK_LIBRARIES (SLVoice ${LIBS})
