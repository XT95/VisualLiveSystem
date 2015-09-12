
#include "signal/basserrorhandler.hpp"

std::string handleBassInitError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_DX:	return "DirectX (or ALSA on Linux or OpenSL ES on Android) is not installed.";
    case BASS_ERROR_DEVICE:	return "device is invalid.";
    case BASS_ERROR_ALREADY:	return "The device has already been initialized. BASS_Free must be called before it can be initialized again.";
    case BASS_ERROR_DRIVER:	return "There is no available device driver. The device may already be in use.";
    case BASS_ERROR_FORMAT:	return "The specified format is not supported by the device. Try changing the freq and flags parameters.";
    case BASS_ERROR_MEM:	return "There is insufficient memory.";
    case BASS_ERROR_NO3D:	return "Could not initialize 3D support.";
    case BASS_ERROR_UNKNOWN:	return "Some other mystery problem!";
    default: return "";
  }
}

std::string handleBassStreamCreateError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_INIT:	return "BASS_Init has not been successfully called.";
    case BASS_ERROR_NOTAVAIL:	return "Only decoding channels (BASS_STREAM_DECODE) are allowed when using the 'no sound' device. The BASS_STREAM_AUTOFREE flag is also unavailable to decoding channels.";
    case BASS_ERROR_FORMAT:	return "The sample format is not supported by the device/drivers. If the stream is more than stereo or the BASS_SAMPLE_FLOAT flag is used, it could be that they are not supported.";
    case BASS_ERROR_SPEAKER:	return "The specified SPEAKER flags are invalid. The device/drivers do not support them, they are attempting to assign a stereo stream to a mono speaker or 3D functionality is enabled.";
    case BASS_ERROR_MEM:	return "There is insufficient memory.";
    case BASS_ERROR_NO3D:	return "Could not initialize 3D support.";
    case BASS_ERROR_UNKNOWN:	return "Some other mystery problem!";
    default: return "";
  }
}

std::string handleBassChannelPlayError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_HANDLE:	return "handle is not a valid channel.";
    case BASS_ERROR_START:	return "The output is paused/stopped, use BASS_Start to start it.";
    case BASS_ERROR_DECODE:	return "The channel is not playable; it is a 'decoding channel'.";
    case BASS_ERROR_BUFLOST:	return "Should not happen... check that a valid window handle was used with BASS_Init.";
    case BASS_ERROR_NOHW:	return "No hardware voices are available (HCHANNEL only). This only occurs if the sample was loaded/created with the BASS_SAMPLE_VAM flag and BASS_VAM_HARDWARE is set in the sample's VAM mode, and there are no hardware voices available to play it.";
    default: return "";
  }
}
