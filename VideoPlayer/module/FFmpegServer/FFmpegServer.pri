INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/FFmpegServer.h \
    $$PWD/audiooutputcontext.h \
    $$PWD/audiooutputstrategy.h \
    $$PWD/ffmpegHead.h \
    $$PWD/ffmpegsynclist.h \
    $$PWD/qaudiooutputstrategy.h \
    $$PWD/sdlaudiooutputstrategy.h
#    $$PWD/datactl.h \
#    $$PWD/globalhelper.h \
#    $$PWD/videoctl.h


SOURCES += \
    $$PWD/FFmpegServer.cpp \
    $$PWD/audiooutputcontext.cpp \
    $$PWD/audiooutputstrategy.cpp \
    $$PWD/ffmpegsynclist.cpp \
    $$PWD/qaudiooutputstrategy.cpp \
    $$PWD/sdlaudiooutputstrategy.cpp
#    $$PWD/globalhelper.cpp \
#    $$PWD/videoctl.cpp


win32{
    win32-msvc*{ #win32 msvc 编译器
        contains(QMAKE_HOST.arch, i386):{ #系统构架，可能存在系统构架与编译器位数不一致，要注意
            message("win32 msvc ffmpeg 32")
#            #FFmpeg Sdk 32
#            INCLUDEPATH += $$PWD/ffmpeg-4.0.2-win32-dev/include
#            LIBS += -L$$PWD/ffmpeg-4.0.2-win32-dev/lib \
#                        -lavcodec \
#                        -lavdevice \
#                        -lavfilter \
#                        -lavformat \
#                        -lavutil \
#                        -lpostproc \
#                        -lswscale
        }else{
            message("win32 msvc ffmpeg 64")
            #FFmpeg Sdk 64
            INCLUDEPATH += $$PWD/ffmpeg-4.2.1-win64-dev/include
            LIBS += -L$$PWD/ffmpeg-4.2.1-win64-dev/lib \
                        -lavcodec \
                        -lavdevice \
                        -lavfilter \
                        -lavformat \
                        -lavutil \
                        -lpostproc \
                        -lswresample \
                        -lswscale
            # SDL2 sdk
            INCLUDEPATH += $$PWD/SDL2-2.30.0/include
            LIBS += -L$$PWD/SDL2-2.30.0/lib/x64 \
                        -lSDL2 \
#                        -lSDL2main \
#                        -lSDL2test

        }
    }else{  #win32 g++编译器
        message("win32 g++ ffmpeg 32")
#            #FFmpeg Sdk 64
#            INCLUDEPATH += $$PWD/ffmpeg-master-latest-win64-gpl-shared/include
#            LIBS += -L$$PWD/ffmpeg-master-latest-win64-gpl-shared/lib \
#                        -lavcodec \
#                        -lavdevice \
#                        -lavfilter \
#                        -lavformat \
#                        -lavutil \
#                        -lpostproc \
#                        -lswresample \
#                        -lswscale
#            # SDL2 sdk
#            INCLUDEPATH += $$PWD/SDL2-2.30.0/include
#            LIBS += -L$$PWD/SDL2-2.30.0/lib/x64 \
#                        -lSDL2 \
#                        -lSDL2main \
#                        -lSDL2test
    }
}

#unix {
#LIBS += \
#    -lSDL2 \
#    -lavcodec \
#    -lavdevice \
#    -lavfilter \
#    -lavformat \
#    -lavutil \
#    -lswresample \
#    -lswscale
#}

