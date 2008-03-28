/* voice.cpp -- voice module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <voice.hpp>
#include <cstring>

//=============================================================================
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
    copy (rhs.position, rhs.position+3, position);
    copy (rhs.velocity, rhs.velocity+3, velocity);
    copy (rhs.at, rhs.at+3, at);
    copy (rhs.up, rhs.up+3, up);
    copy (rhs.left, rhs.left+3, left);
}

VoiceOrientation& VoiceOrientation::operator= (const VoiceOrientation& rhs)
{
    if (this != &rhs)
    {
        VoiceOrientation tmp (rhs);
        swap (tmp);
    }

    return *this;
}

void VoiceOrientation::swap (VoiceOrientation& rhs)
{
    static const size_t size_ (3*sizeof(float));
    float tmp [3]; 

    memcpy (tmp, position, size_); 
    memcpy (position, rhs.position, size_); 
    memcpy (rhs.position, tmp, size_); 

    memcpy (tmp, velocity, size_); 
    memcpy (velocity, rhs.velocity, size_); 
    memcpy (rhs.velocity, tmp, size_); 

    memcpy (tmp, at, size_); 
    memcpy (at, rhs.at, size_); 
    memcpy (rhs.at, tmp, size_); 

    memcpy (tmp, up, size_); 
    memcpy (up, rhs.up, size_); 
    memcpy (rhs.up, tmp, size_); 

    memcpy (tmp, left, size_); 
    memcpy (left, rhs.left, size_); 
    memcpy (rhs.left, tmp, size_); 
}
