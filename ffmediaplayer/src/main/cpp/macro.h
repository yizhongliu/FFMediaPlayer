//
// Created by llm on 20-3-25.
//

#ifndef FFMEDIAPLAYER_MACRO_H
#define FFMEDIAPLAYER_MACRO_H




enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,
    MEDIA_STARTED           = 6,
    MEDIA_PAUSED            = 7,
    MEDIA_STOPPED           = 8,
    MEDIA_SKIPPED           = 9,
    MEDIA_TIMED_TEXT        = 99,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
    MEDIA_SUBTITLE_DATA     = 201,
    MEDIA_META_DATA         = 202,
    MEDIA_DRM_INFO          = 210,
};

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
    // 0xx
            MEDIA_ERROR_UNKNOWN = 1,
    // 1xx
            MEDIA_ERROR_SERVER_DIED = 100,
    // 2xx
            MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
    // 3xx
};

// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant info code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
//
enum media_info_type {
    // 0xx
            MEDIA_INFO_UNKNOWN = 1,
    // The player was started because it was used as the next player for another
    // player, which just completed playback
            MEDIA_INFO_STARTED_AS_NEXT = 2,
    // The player just pushed the very first video frame for rendering
            MEDIA_INFO_RENDERING_START = 3,
    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
            MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,
    // MediaPlayer is temporarily pausing playback internally in order to
    // buffer more data.
            MEDIA_INFO_BUFFERING_START = 701,
    // MediaPlayer is resuming playback after filling buffers.
            MEDIA_INFO_BUFFERING_END = 702,
    // Bandwidth in recent past
            MEDIA_INFO_NETWORK_BANDWIDTH = 703,

    //LoadingPercent add by xhr
            MEDIA_INFO_BUFFERING_PERCENT = 705,

    //LoadingRate add by xhr
            MEDIA_INFO_LAODING_RATE = 706,

    //decoder cfg suport audio play whether,add by xhr
            MEDIA_INFO_DECODER_CFG_AUDIO_SUPPORT = 708,

    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
            MEDIA_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
            MEDIA_INFO_NOT_SEEKABLE = 801,
    // New media metadata is available.
            MEDIA_INFO_METADATA_UPDATE = 802,
    // Audio can not be played.
            MEDIA_INFO_PLAY_AUDIO_ERROR = 804,
    // Video can not be played.
            MEDIA_INFO_PLAY_VIDEO_ERROR = 805,

    //9xx
            MEDIA_INFO_TIMED_TEXT_ERROR = 900,

            MEDIA_INFO_EOF = 1000,
            MEDIA_INFO_ELEMENT_STATE_CHANGE = 1001,
};




//错误代码
#define STATUS_OK 0
#define INVALID_OPERATION -1;
#define ERROR_CODE_PREPARE -1000
#define ERROR_CODE_PLAY -2000


#define OPEN_URL_FAIL (ERROR_CODE_PREPARE - 1)
#define CREATE_PLAYER_FAIL (ERROR_CODE_PREPARE - 2)
#define FIND_STREAM_FAIL (ERROR_CODE_PREPARE - 3)
//找不到解码器
#define FIND_DECODER_FAIL (ERROR_CODE_PREPARE - 4)

//无法根据解码器创建上下文
#define ALLOC_CODEC_CONTEXT_FAIL (ERROR_CODE_PREPARE - 5)

//根据流信息 配置上下文参数失败
#define CODEC_CONTEXT_PARAMETERS_FAIL (ERROR_CODE_PREPARE - 6)

//打开解码器失败
#define OPEN_DECODER_FAIL (ERROR_CODE_PREPARE - 7)

#define MEDIA_ALREAD_START (ERROR_CODE_PLAY -1)
//读取媒体数据包失败
#define READ_PACKETS_FAIL (ERROR_CODE_PLAY - 2)

//播放过程中包处理失败
#define PASER_PACKETS_FAIL (ERROR_CODE_PLAY - 3)

#endif //FFMEDIAPLAYER_MACRO_H
