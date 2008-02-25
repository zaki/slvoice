
PROJECT (jinsei-voice)

SET(CMAKE_BUILD_TYPE Debug)

SET (VOICEDIR .)
SET (PJDIR pjproject)
SET (SOCKETDIR sockets)
SET (TINYXMLDIR tinyxml)

SET (VOICESRC 
    ${VOICEDIR}/main.cpp)

SET (SOCKETSRC 
    ${SOCKETDIR}/Sockets.cpp)

SET (TINYXMLSRC 
    ${TINYXMLDIR}/tinyxml.cpp 
    ${TINYXMLDIR}/tinyxmlerror.cpp
    ${TINYXMLDIR}/tinyxmlparser.cpp 
    ${TINYXMLDIR}/tinystr.cpp )

ADD_DEFINITIONS (-DTIXML_USE_STL)

IF (WIN32)
    ADD_DEFINITIONS(-DPJ_WIN32=1)
ENDIF (WIN32)

IF (UNIX)
    IF (APPLE)
        ADD_DEFINITIONS (-DPJ_AUTOCONF=0)
        SET (PJTARGET i386-apple-darwin8.11.1)
    ELSE (APPLE)
        ADD_DEFINITIONS (-DPJ_AUTOCONF=1)
        SET (PJTARGET i686-pc-linux-gnu)
    ENDIF (APPLE)

    FIND_LIBRARY (LIBMATH m)
    FIND_LIBRARY (LIBUUID uuid)
    FIND_LIBRARY (LIBNSL nsl)
    FIND_LIBRARY (LIBRT rt)
    FIND_LIBRARY (LIBTHREAD pthread)
    FIND_LIBRARY (LIBASOUND asound)
    FIND_LIBRARY (LIBSSL ssl)
    FIND_LIBRARY (LIBCRYPTO crypto)

    FIND_LIBRARY (LIBTINYXML tinyxml ${TINYXMLDIR})
    FIND_LIBRARY (LIBPJSUA pjsua ${PJDIR})
    FIND_LIBRARY (LIBPJSIP_UA pjsip-ua ${PJDIR})
    FIND_LIBRARY (LIBPJSIP_SIMPLE pjsip-simple ${PJDIR})
    FIND_LIBRARY (LIBPJSIP pjsip ${PJDIR})
    FIND_LIBRARY (LIBPJMEDIA_CODEC pjmedia-codec ${PJDIR})
    FIND_LIBRARY (LIBPJMEDIA pjmedia ${PJDIR})
    FIND_LIBRARY (LIBPJNATH pjnath ${PJDIR})
    FIND_LIBRARY (LIBPJLIB pjlib-util ${PJDIR})
    FIND_LIBRARY (LIBRESAMPLE resample ${PJDIR})
    FIND_LIBRARY (LIBMILENAGE milenage ${PJDIR})
    FIND_LIBRARY (LIBGSMCODEC gsmcodec ${PJDIR})
    FIND_LIBRARY (LIBILBCCODEC ilbccodec ${PJDIR})
    FIND_LIBRARY (LIBPORTAUDIO portaudio ${PJDIR})
    FIND_LIBRARY (LIBSPEEX speex ${PJDIR})
    FIND_LIBRARY (LIBPJ pj ${PJDIR})
    
    FIND_PATH (PJLIB_INCL pjlib ${PJDIR})
    FIND_PATH (PJLIB_UTIL_INCL pjlib-util ${PJDIR})
    FIND_PATH (PJNATH_INCL pjnath ${PJDIR})
    FIND_PATH (PJMEDIA_INCL pjmedia ${PJDIR})
    FIND_PATH (PJSIP_INCL pjsip ${PJDIR})
    
    SET (LIBS
        ${LIBPJSUA}
        ${LIBPJSIP_UA}
        ${LIBPJSIP_SIMPLE}
        ${LIBPJSIP}
        ${LIBPJMEDIA_CODEC}
        ${LIBPJMEDIA}
        ${LIBPJNATH}
        ${LIBPJLIB}
        ${LIBRESAMPLE}
        ${LIBMILENAGE}
        ${LIBGSMCODEC}
        ${LIBILBCCODEC}
        ${LIBPORTAUDIO}
        ${LIBPJ}
        ${LIBSPEEX}
        ${LIBMATH}
        ${LIBUUID}
        ${LIBNSL}
        ${LIBRT}
        ${LIBTHREAD}
        ${LIBSSL}
        ${LIBCRYPTO})
    
    # FIND_LIBRARY unfortunately prefers dynamic over static
    # with no option to override. We break out the sledgehammer.
    #FOREACH (L ${LIBS})
    #    STRING (REPLACE ".so" ".a" SL ${L})
    #    SET (STATIC_LIBS ${STATIC_LIBS} ${SL})
    #ENDFOREACH (L)

    SET (LIBS ${LIBS} ${LIBASOUND})
    SET (SRC ${VOICESRC} ${SOCKETSRC} ${TINYXMLSRC})
    SET (INCL 
        ${VOICEDIR}
        ${PJLIB_INCL} 
        ${PJLIB_UTIL_INCL} 
        ${PJNATH_INCL} 
        ${PJMEDIA_INCL} 
        ${PJSIP_INCL})


ENDIF (UNIX)

INCLUDE_DIRECTORIES (${INCL})
ADD_EXECUTABLE (voice ${SRC})
TARGET_LINK_LIBRARIES (voice ${LIBS})
