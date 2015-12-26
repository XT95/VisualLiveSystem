
/*

THIS FILE IS SUBJECT TO STRICT RULES OF BDE NE777 COPYDOWN. 
NOBODY IS PERMITTED TO COPY AND DISTRIBUTE VERBATIM OR MODIFIED COPIES OF
THIS LICENSE HEADER. A SECURITY LEVEL IS ASSIGNED TO THE FILE AND A VISIBILITY.
THIS FILE MUST NOT BE COPIED OR REDISTRIBUTED IF IT'S VISIBILITY IS NOT PUBLIC. 
VISIBILITY HAS 3 POSSIBLE STATES:
- PRIVATE: LIMITED TO PROJECTS WITH THE SAME SECURITY LEVEL WITHIN THE BDE NE777. (DEFAULT VISIBILITY)
- PROTECTED: LIMITED TO PROJECTS USING THE SAME SECURITY RULES WITHIN THE BDE NE777. 
- PUBLIC: USABLE FOR FREE IN PUBLIC PROJECTS UNDER THE FOLLOWING LICENSES: BSD, MIT OR DWTFYWT 
WITHOUT OTHER CONDITIONS THAN THE CONSERVATION OF THIS HEADER INCLUDING: 
RULES, ORIGINAL AUTHORS (WITH EMAIL), ORIGINAL FILENAME, AND VERSION, INSIDE THE FILE
WHICH CONTAINS PART (OR ENTIRE) CODE FROM THIS FILE. USABLE IN OTHER PROJECTS WITH CONDITIONS 
AND REMUMERATIONS, FIXED BY ORIGINAL AUTHORS (CONTACT THEM).

*/

/**
  * THIS FILE IS PART OF LIBTOOLS 
  * SECURITY LEVEL : 8 (CRITICAL)  
  * VISIBILITY     : PROTECTED
  * © COPYDOWN™ LAMOGUI ALL RIGHTS RESERVED 
  *
  * FILE         : bassproc.h
  * AUTHORS      : Julien De Loor (julien.deloor@gmail.com)
  * VERSION      : 1.2 
  * DEPENDENCIES : config.h
  */
  
#ifndef LIBTOOLS_BASSPROC_H
#define LIBTOOLS_BASSPROC_H


#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(BASS_H)
#include <windows.h>

#if (_MSC_VER == 1500)
#ifndef BASSDEF
#define BASSDEF(f) (WINAPI f)
#endif

#ifndef BASSFLACDEF
#define BASSFLACDEF(f) (WINAPI f)
#endif

#else 
#ifndef BASSDEF
#define BASSDEF (WINAPI
#endif

#ifndef BASSFLACDEF
#define BASSFLACDEF WINAPI
#endif
#endif

typedef DWORD HMUSIC;		// MOD music handle
typedef DWORD HSAMPLE;		// sample handle
typedef DWORD HCHANNEL;		// playing sample's channel handle
typedef DWORD HSTREAM;		// sample stream handle
typedef DWORD HRECORD;		// recording handle
typedef DWORD HSYNC;		// synchronizer handle
typedef DWORD HDSP;			// DSP handle
typedef DWORD HFX;			// DX8 effect handle
typedef DWORD HPLUGIN;		// Plugin handle
// Error codes returned by BASS_ErrorGetCode
#define BASS_OK				0	// all is OK
#define BASS_ERROR_MEM		1	// memory error
#define BASS_ERROR_FILEOPEN	2	// can't open the file
#define BASS_ERROR_DRIVER	3	// can't find a free/valid driver
#define BASS_ERROR_BUFLOST	4	// the sample buffer was lost
#define BASS_ERROR_HANDLE	5	// invalid handle
#define BASS_ERROR_FORMAT	6	// unsupported sample format
#define BASS_ERROR_POSITION	7	// invalid position
#define BASS_ERROR_INIT		8	// BASS_Init has not been successfully called
#define BASS_ERROR_START	9	// BASS_Start has not been successfully called
#define BASS_ERROR_ALREADY	14	// already initialized/paused/whatever
#define BASS_ERROR_NOCHAN	18	// can't get a free channel
#define BASS_ERROR_ILLTYPE	19	// an illegal type was specified
#define BASS_ERROR_ILLPARAM	20	// an illegal parameter was specified
#define BASS_ERROR_NO3D		21	// no 3D support
#define BASS_ERROR_NOEAX	22	// no EAX support
#define BASS_ERROR_DEVICE	23	// illegal device number
#define BASS_ERROR_NOPLAY	24	// not playing
#define BASS_ERROR_FREQ		25	// illegal sample rate
#define BASS_ERROR_NOTFILE	27	// the stream is not a file stream
#define BASS_ERROR_NOHW		29	// no hardware voices available
#define BASS_ERROR_EMPTY	31	// the MOD music has no sequence data
#define BASS_ERROR_NONET	32	// no internet connection could be opened
#define BASS_ERROR_CREATE	33	// couldn't create the file
#define BASS_ERROR_NOFX		34	// effects are not available
#define BASS_ERROR_NOTAVAIL	37	// requested data is not available
#define BASS_ERROR_DECODE	38	// the channel is/isn't a "decoding channel"
#define BASS_ERROR_DX		39	// a sufficient DirectX version is not installed
#define BASS_ERROR_TIMEOUT	40	// connection timedout
#define BASS_ERROR_FILEFORM	41	// unsupported file format
#define BASS_ERROR_SPEAKER	42	// unavailable speaker
#define BASS_ERROR_VERSION	43	// invalid BASS version (used by add-ons)
#define BASS_ERROR_CODEC	44	// codec is not available/supported
#define BASS_ERROR_ENDED	45	// the channel/file has ended
#define BASS_ERROR_BUSY		46	// the device is busy
#define BASS_ERROR_UNKNOWN	-1	// some other mystery problem

// Channel info structure
typedef struct {
	DWORD freq;		// default playback rate
	DWORD chans;	// channels
	DWORD flags;	// BASS_SAMPLE/STREAM/MUSIC/SPEAKER flags
	DWORD ctype;	// type of channel
	DWORD origres;	// original resolution
	HPLUGIN plugin;	// plugin
	HSAMPLE sample; // sample
	const char *filename; // filename
} BASS_CHANNELINFO;

typedef struct {
  DWORD flags;	// device capabilities (DSCAPS_xxx flags)
  DWORD hwsize;	// size of total device hardware memory
  DWORD hwfree;	// size of free device hardware memory
  DWORD freesam;	// number of free sample slots in the hardware
  DWORD free3d;	// number of free 3D sample slots in the hardware
  DWORD minrate;	// min sample rate supported by the hardware
  DWORD maxrate;	// max sample rate supported by the hardware
  BOOL eax;		// device supports EAX? (always FALSE if BASS_DEVICE_3D was not used)
  DWORD minbuf;	// recommended minimum buffer length in ms (requires BASS_DEVICE_LATENCY)
  DWORD dsver;	// DirectSound version
  DWORD latency;	// delay (in ms) before start of playback (requires BASS_DEVICE_LATENCY)
  DWORD initflags; // BASS_Init "flags" parameter
  DWORD speakers; // number of speakers available
  DWORD freq;		// current output rate
} BASS_INFO;

typedef unsigned __int64 QWORD;

typedef struct {
    char id[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    BYTE genre;
} TAG_ID3;

#define BASS_SAMPLE_8BITS		1	// 8 bit
#define BASS_SAMPLE_FLOAT		256	// 32-bit floating-point
#define BASS_SAMPLE_MONO		2	// mono
#define BASS_SAMPLE_LOOP		4	// looped
#define BASS_SAMPLE_3D			8	// 3D functionality
#define BASS_SAMPLE_SOFTWARE	16	// not using hardware mixing
#define BASS_SAMPLE_MUTEMAX		32	// mute at max distance (3D only)
#define BASS_SAMPLE_VAM			64	// DX7 voice allocation & management
#define BASS_SAMPLE_FX			128	// old implementation of DX8 effects
#define BASS_SAMPLE_OVER_VOL	0x10000	// override lowest volume
#define BASS_SAMPLE_OVER_POS	0x20000	// override longest playing
#define BASS_SAMPLE_OVER_DIST	0x30000 // override furthest from listener (3D only)

#define BASS_STREAM_PRESCAN		0x20000 // enable pin-point seeking/length (MP3/MP2/MP1)
#define BASS_MP3_SETPOS			BASS_STREAM_PRESCAN
#define BASS_STREAM_AUTOFREE	0x40000	// automatically free the stream when it stop/ends
#define BASS_STREAM_RESTRATE	0x80000	// restrict the download rate of internet file streams
#define BASS_STREAM_BLOCK		0x100000 // download/play internet file stream in small blocks
#define BASS_STREAM_DECODE		0x200000 // don't play the stream, only decode (BASS_ChannelGetData)
#define BASS_STREAM_STATUS		0x800000 // give server status info (HTTP/ICY tags) in DOWNLOADPROC

#define BASS_MUSIC_FLOAT		BASS_SAMPLE_FLOAT
#define BASS_MUSIC_MONO			BASS_SAMPLE_MONO
#define BASS_MUSIC_LOOP			BASS_SAMPLE_LOOP
#define BASS_MUSIC_3D			BASS_SAMPLE_3D
#define BASS_MUSIC_FX			BASS_SAMPLE_FX
#define BASS_MUSIC_AUTOFREE		BASS_STREAM_AUTOFREE
#define BASS_MUSIC_DECODE		BASS_STREAM_DECODE
#define BASS_MUSIC_PRESCAN		BASS_STREAM_PRESCAN	// calculate playback length
#define BASS_MUSIC_CALCLEN		BASS_MUSIC_PRESCAN
#define BASS_MUSIC_RAMP			0x200	// normal ramping
#define BASS_MUSIC_RAMPS		0x400	// sensitive ramping
#define BASS_MUSIC_SURROUND		0x800	// surround sound
#define BASS_MUSIC_SURROUND2	0x1000	// surround sound (mode 2)
#define BASS_MUSIC_FT2MOD		0x2000	// play .MOD as FastTracker 2 does
#define BASS_MUSIC_PT1MOD		0x4000	// play .MOD as ProTracker 1 does
#define BASS_MUSIC_NONINTER		0x10000	// non-interpolated sample mixing
#define BASS_MUSIC_SINCINTER	0x800000 // sinc interpolated sample mixing
#define BASS_MUSIC_POSRESET		0x8000	// stop all notes when moving position
#define BASS_MUSIC_POSRESETEX	0x400000 // stop all notes and reset bmp/etc when moving position
#define BASS_MUSIC_STOPBACK		0x80000	// stop the music on a backwards jump effect
#define BASS_MUSIC_NOSAMPLE		0x100000 // don't load the samples

// BASS_StreamGetFilePosition modes
#define BASS_FILEPOS_CURRENT	0
#define BASS_FILEPOS_DECODE		BASS_FILEPOS_CURRENT
#define BASS_FILEPOS_DOWNLOAD	1
#define BASS_FILEPOS_END		2
#define BASS_FILEPOS_START		3
#define BASS_FILEPOS_CONNECTED	4
#define BASS_FILEPOS_BUFFER		5
#define BASS_FILEPOS_SOCKET		6

// BASS_ChannelGetLength/GetPosition/SetPosition modes
#define BASS_POS_BYTE			0		// byte position
#define BASS_POS_MUSIC_ORDER	1		// order.row position, MAKELONG(order,row)
#define BASS_POS_OGG			3		// OGG bitstream number
#define BASS_POS_DECODE			0x10000000 // flag: get the decoding (not playing) position
#define BASS_POS_DECODETO		0x20000000 // flag: decode to the position instead of seeking

// BASS_ChannelGetTags types : what's returned
#define BASS_TAG_ID3		0	// ID3v1 tags : TAG_ID3 structure
#define BASS_TAG_ID3V2		1	// ID3v2 tags : variable length block
#define BASS_TAG_OGG		2	// OGG comments : series of null-terminated UTF-8 strings
#define BASS_TAG_HTTP		3	// HTTP headers : series of null-terminated ANSI strings
#define BASS_TAG_ICY		4	// ICY headers : series of null-terminated ANSI strings
#define BASS_TAG_META		5	// ICY metadata : ANSI string
#define BASS_TAG_APE		6	// APE tags : series of null-terminated UTF-8 strings
#define BASS_TAG_MP4 		7	// MP4/iTunes metadata : series of null-terminated UTF-8 strings
#define BASS_TAG_VENDOR		9	// OGG encoder : UTF-8 string
#define BASS_TAG_LYRICS3	10	// Lyric3v2 tag : ASCII string
#define BASS_TAG_CA_CODEC	11	// CoreAudio codec info : TAG_CA_CODEC structure
#define BASS_TAG_MF			13	// Media Foundation tags : series of null-terminated UTF-8 strings
#define BASS_TAG_WAVEFORMAT	14	// WAVE format : WAVEFORMATEEX structure
#define BASS_TAG_RIFF_INFO	0x100 // RIFF "INFO" tags : series of null-terminated ANSI strings
#define BASS_TAG_RIFF_BEXT	0x101 // RIFF/BWF "bext" tags : TAG_BEXT structure
#define BASS_TAG_RIFF_CART	0x102 // RIFF/BWF "cart" tags : TAG_CART structure
#define BASS_TAG_RIFF_DISP	0x103 // RIFF "DISP" text tag : ANSI string
#define BASS_TAG_APE_BINARY	0x1000	// + index #, binary APE tag : TAG_APE_BINARY structure
#define BASS_TAG_MUSIC_NAME		0x10000	// MOD music name : ANSI string
#define BASS_TAG_MUSIC_MESSAGE	0x10001	// MOD message : ANSI string
#define BASS_TAG_MUSIC_ORDERS	0x10002	// MOD order list : BYTE array of pattern numbers
#define BASS_TAG_MUSIC_INST		0x10100	// + instrument #, MOD instrument name : ANSI string
#define BASS_TAG_MUSIC_SAMPLE	0x10300	// + sample #, MOD sample name : ANSI string

//Others
#define BASS_ASYNCFILE			0x40000000
#define BASS_UNICODE			0x80000000


typedef void (CALLBACK DOWNLOADPROC)(const void *buffer, DWORD length, void *user);
/* Internet stream download callback function.
buffer : Buffer containing the downloaded data... NULL=end of download
length : Number of bytes in the buffer
user   : The 'user' parameter value given when calling BASS_StreamCreateURL */

///BASS PROCS
//Typedefs
#if defined(_WIN32) && !defined(_WIN32_WCE)
typedef BOOL BASSDEF BASS_INIT_PROC)(int device, DWORD freq, DWORD flags, HWND win, const GUID *dsguid);
#else
typedef BOOL BASSDEF (BASS_INIT_PROC)(int device, DWORD freq, DWORD flags, void *win, void *dsguid);
#endif

typedef int BASSDEF BASS_ERRORGETCODE_PROC)();
typedef BOOL BASSDEF BASS_FREE_PROC)();
typedef HMUSIC BASSDEF BASS_MUSICLOAD_PROC)(BOOL mem, const void *file, QWORD offset, DWORD length, DWORD flags, DWORD freq);

typedef BOOL BASSDEF BASS_MUSICFREE_PROC)(HMUSIC handle);
typedef BOOL BASSDEF BASS_STREAMFREE_PROC)(HSTREAM handle);

typedef HSTREAM BASSDEF BASS_STREAMCREATEFILE_PROC)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
typedef HSTREAM BASSDEF BASS_STREAMCREATEURL_PROC)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);

typedef BOOL BASSDEF BASS_CHANNELGETINFO_PROC)(DWORD handle, BASS_CHANNELINFO *info);

typedef DWORD BASSDEF BASS_CHANNELGETDATA_PROC)(DWORD handle, void *buffer, DWORD length);

typedef BOOL BASSDEF BASS_CHANNELSETPOSITION_PROC)(DWORD handle, QWORD pos, DWORD mode);
typedef QWORD BASSDEF BASS_CHANNELGETPOSITION_PROC)(DWORD handle, DWORD mode);

typedef QWORD BASSDEF BASS_CHANNELGETLENGTH_PROC)(DWORD handle, DWORD mode);
typedef double BASSDEF BASS_CHANNELBYTES2SECONDS_PROC)(DWORD handle, QWORD pos);
typedef QWORD BASSDEF BASS_CHANNELSECONDS2BYTES_PROC)(DWORD handle, double pos);

typedef const char* BASSDEF BASS_CHANNELGETTAGS_PROC)(DWORD handle, DWORD tags);

//extern
#ifndef BASSPROC_IMPLEMENT
extern BASS_INIT_PROC* BASS_Init;
extern BASS_ERRORGETCODE_PROC* BASS_ErrorGetCode;
extern BASS_FREE_PROC* BASS_Free;
extern BASS_MUSICLOAD_PROC* BASS_MusicLoad;
extern BASS_MUSICFREE_PROC* BASS_MusicFree;
extern BASS_STREAMFREE_PROC* BASS_StreamFree;
extern BASS_STREAMCREATEFILE_PROC* BASS_StreamCreateFile;
extern BASS_STREAMCREATEURL_PROC* BASS_StreamCreateURL;
extern BASS_CHANNELGETINFO_PROC* BASS_ChannelGetInfo;
extern BASS_CHANNELGETDATA_PROC* BASS_ChannelGetData;
extern BASS_CHANNELSETPOSITION_PROC* BASS_ChannelSetPosition;
extern BASS_CHANNELGETPOSITION_PROC* BASS_ChannelGetPosition;
extern BASS_CHANNELGETLENGTH_PROC* BASS_ChannelGetLength;
extern BASS_CHANNELBYTES2SECONDS_PROC* BASS_ChannelBytes2Seconds;
extern BASS_CHANNELSECONDS2BYTES_PROC* BASS_ChannelSeconds2Bytes;
extern BASS_CHANNELGETTAGS_PROC* BASS_ChannelGetTags;
#endif

///Load from dll
int load_bass_procs(const char* bassdllname);
int load_bassflac_procs(const char* bassflacdllname);

#endif //LIBTOOLS_WINDOWS

int BASS_EnsureBassInit(int device, DWORD freq, DWORD flags);
int BASS_EnsureBassContext();
void BASS_EnsureBassFree();

#ifdef __cplusplus
}
#endif

#endif
  
  
