/* voice.hpp -- state definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _VOICE_HPP_
#define _VOICE_HPP_

using namespace std;


//=============================================================================
// voice state
struct VoiceOrientation
{
    float position [3];
    float velocity [3];

    float at [3];
    float up [3];
    float left [3];

    VoiceOrientation ();
    VoiceOrientation (const VoiceOrientation& copy);
    VoiceOrientation& operator= (const VoiceOrientation& rhs);

    void swap (VoiceOrientation& rhs);

    void set_position (const float v [3]) { copy (v, v+3, position); }
    void set_velocity (const float v [3]) { copy (v, v+3, velocity); }
    void set_at (const float v [3]) { copy (v, v+3, at); }
    void set_up (const float v [3]) { copy (v, v+3, up); }
    void set_left (const float v [3]) { copy (v, v+3, left); }
};

struct VoiceState
{
    float mic_volume;
    float mic_mute;

    float speaker_volume;
    float speaker_mute;

    VoiceOrientation listener;
    VoiceOrientation speaker;

    VoiceState () :
        mic_volume (0.0f),
        mic_mute (0.0f),
        speaker_volume (0.0f),
        speaker_mute (0.0f)
    {}
};

#endif //_VOICE_HPP_
