/* voice.cpp -- voice module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <parameters.hpp>
#include <cstring>

//=============================================================================
static inline void copy_VoiceOrientation_ 
    (const VoiceOrientation& src, VoiceOrientation& dst)
{
    static const size_t size_ (3*sizeof(float));
    memcpy (dst.position, src.position, size_);
    memcpy (dst.velocity, src.velocity, size_);
    memcpy (dst.at, src.at, size_);
    memcpy (dst.up, src.up, size_);
    memcpy (dst.left, src.left, size_);
}

VoiceOrientation::VoiceOrientation ()
{
    fill_n (position, 3, 0.0f);
    fill_n (velocity, 3, 0.0f);
    fill_n (at, 3, 0.0f);
    fill_n (up, 3, 0.0f);
    fill_n (left, 3, 0.0f);
}

VoiceOrientation::VoiceOrientation (const VoiceOrientation& rhs)
{
    copy_VoiceOrientation_ (rhs, *this);
}

VoiceOrientation& VoiceOrientation::operator= (const VoiceOrientation& rhs)
{
    if (this != &rhs)
        copy_VoiceOrientation_ (rhs, *this);

    return *this;
}
