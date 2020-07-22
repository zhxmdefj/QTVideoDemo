#include "videodata.h"
#include <QDebug>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavutil/frame.h>
    #include <libswscale/swscale.h>
    #include <libavutil/pixfmt.h>
    #include <libavutil/imgutils.h>
}

VideoData::VideoData(QObject *parent):
    QThread(parent)
{

}

void VideoData::setUrl(QString s)
{
    m_url = s;
}

void VideoData::run()
{
    av_register_all();
    avformat_network_init();
    AVFormatContext *pAVFomatContext = avformat_alloc_context();
    AVCodecContext* pAVCodecContext = nullptr;
    AVFrame *pAVFrame = av_frame_alloc();

    AVDictionary *opt = nullptr;
//    av_dict_set(&opt,"probesize","4096",0);
//    av_dict_set(&opt,"buffer_size","1024000",0);
//    av_dict_set(&opt,"stimeout","5000000",0); //wei miao 5000000
//    av_dict_set(&opt,"max_delay","0",0);
    av_dict_set(&opt,"rtsp_transport","udp",0);
    int result = avformat_open_input(&pAVFomatContext,m_url.toStdString().data(),nullptr,nullptr);
    if(result < 0){
        qDebug() << QStringLiteral("open video failed");
        return;
    }
    result = avformat_find_stream_info(pAVFomatContext,nullptr);
    if(result < 0){
        qDebug() << QStringLiteral("get source video failed");
        return;
    }

    int videoStreamIndex = -1;
    for(uint i = 0; i < pAVFomatContext->nb_streams; i++){
        if(pAVFomatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamIndex = i;
            break;
        }
    }

    if(videoStreamIndex == -1){
        qDebug() << QStringLiteral("get video source index failed");
        return;
    }

    int vden, vnum, fps;
    if(vden <= 0 || vnum <= 0){
        fps = 25;
        qDebug() << "get video fps error";
        qDebug() << "use default " << fps;
    }else{
        fps = vnum/vden;
        qDebug() << "video fps:" << fps;
    }

    pAVCodecContext = pAVFomatContext->streams[videoStreamIndex]->codec;
    int videoWidth = pAVCodecContext->width;
    int videoHeight = pAVCodecContext->height;

    AVCodec *pAVCodec;

//    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    pAVCodec = avcodec_find_decoder_by_name("h264_cuvid");
    if(!pAVCodec){
        qDebug() << "find h264_cuvid failed";
        return;
    }

    int numBytes = avpicture_get_size(AV_PIX_FMT_NV12,videoWidth,videoHeight);
    uint8_t *out_buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

    int y_size = pAVCodecContext->width * pAVCodecContext->height;
    AVPacket* pAVPacket = (AVPacket*) malloc(sizeof(AVPacket));
    av_new_packet(pAVPacket,y_size);
    av_dump_format(pAVFomatContext,0,m_url.toStdString().data(),0);
    result = avcodec_open2(pAVCodecContext,pAVCodec,nullptr);
    if(result < 0){
        qDebug() << QStringLiteral("open decoder failed");
        return;
    }

    qDebug() << QStringLiteral("video source init success");
    int got_picture = 0;
    while (!isInterruptionRequested()) {
        if(av_read_frame(pAVFomatContext,pAVPacket) <0){
            break;
        }
            avcodec_decode_video2(pAVCodecContext,pAVFrame,&got_picture,pAVPacket);
            if(got_picture){
                if(!is_started){
                    emit sigStarted(out_buffer,videoWidth,videoHeight);
                    is_started = true;
                }
                int bytes = 0;
                for(int i = 0; i < videoHeight; i++){
                    ::memcpy(out_buffer + bytes,pAVFrame->data[0] + pAVFrame->linesize[0] * i,videoWidth);
                    bytes += videoWidth;
                }
                int uv = videoHeight >> 1;
                for(int i = 0; i < uv; i++){
                    ::memcpy(out_buffer + bytes,pAVFrame->data[1] + pAVFrame->linesize[1] * i,videoWidth);
                    bytes += videoWidth;
                }
                emit sigNewFrame();
                msleep(fps);
            }
        av_free_packet(pAVPacket);
    }
//    av_free(pAVPicture);

    is_started = false;
    emit(is_started);
    avcodec_close(pAVCodecContext);
    avformat_close_input(&pAVFomatContext);
    av_free(out_buffer);
}
