#ifndef FFMPEGHEAD_H
#define FFMPEGHEAD_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libavutil/opt.h>
    #include <libavutil/time.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/avutil.h>
    #include <libavutil/ffversion.h>
    #include <libavutil/frame.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/imgutils.h>
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavdevice/avdevice.h>
}

#endif // FFMPEGHEAD_H
