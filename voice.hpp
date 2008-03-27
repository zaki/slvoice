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

    VoiceOrientation ()
    {
        fill_n (position, 3, 0.0f);
        fill_n (velocity, 3, 0.0f);
        fill_n (at, 3, 0.0f);
        fill_n (up, 3, 0.0f);
        fill_n (left, 3, 0.0f);
    }
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
