QT       += core gui
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cvcamera.cpp \
    glwidget.cpp \
    main.cpp \
    qtglwidget.cpp

HEADERS += \
    cvcamera.h \
    glwidget.h \
    qtglwidget.h

FORMS += \
    cvcamera.ui \
    glwidget.ui \
    qtglwidget.ui

#添加库
INCLUDEPATH += $$PWD/ffmpeg/include

LIBS    +=  $$PWD/ffmpeg/lib/avcodec.lib \
            $$PWD/ffmpeg/lib/avdevice.lib \
            $$PWD/ffmpeg/lib/avfilter.lib \
            $$PWD/ffmpeg/lib/avformat.lib \
            $$PWD/ffmpeg/lib/avutil.lib \
            $$PWD/ffmpeg/lib/postproc.lib \
            $$PWD/ffmpeg/lib/swresample.lib \
            $$PWD/ffmpeg/lib/swscale.lib \

INCLUDEPATH += D:\opencv\build\include

LIBS += D:/opencv/cvbuild/bin/libopencv_videoio440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_video440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_stitching440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_photo440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_objdetect440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_ml440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_imgproc440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_imgcodecs440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_highgui440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_gapi440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_flann440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_features2d440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_dnn440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_core440.dll
LIBS += D:/opencv/cvbuild/bin/libopencv_calib3d440.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
