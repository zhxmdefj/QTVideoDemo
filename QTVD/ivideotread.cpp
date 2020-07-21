#include "ivideotread.h"
#include <QApplication>
#include <QTime>
#include <QDebug>
#include <QThread>
#include <QImage>
#include <QCameraInfo>

#define P_USE_FILTER 1
// 一定要加这个 不然会报未定义
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#if P_USE_FILTER
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#endif
}

//const char *filter_descr = "crop=iw/2:ih:0:0,split[left][tmp];[tmp]hflip[right];                                                                                                                                                                                                                     												[left]pad=iw*2[a];[a][right]overlay=w";
//const char *filter_descr = "negate[out]";
//const char *filter_descr = "scale=iw/2:ih/2[in_tmp];[in_tmp]split=4[in_1][in_2][in_3][in_4];[in_1]pad=iw*2:ih*2[a];[a][in_2]overlay=w[b];[b][in_3]overlay=0:h[d];[d][in_4]overlay=w:h[out]";
const char *filter_descr = "drawtext=fontfile=simhei.ttf: text='zhxmdefj.':x=80:y=20:fontsize=24:fontcolor=red:shadowy=2";


AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
const AVFilter *buffersrc;
const AVFilter *buffersink;

static int initFilters(AVFormatContext *ifmt_ctx)
{
    int ret;
    char args[512];
    AVFilterGraph *filter_graph = NULL;

    //初始化滤镜
    avfilter_register_all();
    buffersrc = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");

    AVFilterInOut *outputs = avfilter_inout_alloc();
    if (!outputs)
    {
        printf("Cannot alloc output\n");
        return -1;
    }
    AVFilterInOut *inputs = avfilter_inout_alloc();
    if (!inputs)
    {
        printf("Cannot alloc input\n");
        return -1;
    }

    if (filter_graph)
        avfilter_graph_free(&filter_graph);
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph)
    {
        printf("Cannot create filter graph\n");
        return -1;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        ifmt_ctx->streams[0]->codec->width, ifmt_ctx->streams[0]->codec->height, ifmt_ctx->streams[0]->codec->pix_fmt,
        ifmt_ctx->streams[0]->time_base.num, ifmt_ctx->streams[0]->time_base.den,
        ifmt_ctx->streams[0]->codec->sample_aspect_ratio.num, ifmt_ctx->streams[0]->codec->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
        args, NULL, filter_graph);
    if (ret < 0) {
        printf("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
        NULL, NULL, filter_graph);
    if (ret < 0) {
        printf("Cannot create buffer sink\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
        &inputs, &outputs, NULL)) < 0)
        return ret;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        return ret;

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return 0;
}

IVideoTread::IVideoTread(QObject *parent)
    : QThread(parent)
{
}

void IVideoTread::openCamera()
{
    QCameraInfo camerainfo = QCameraInfo::defaultCamera();
    QString devicename = camerainfo.description();
    _cameraName = "video=" + devicename;
    qDebug() << devicename;

    start();
}

void IVideoTread::run()
{
    AVFormatContext *pFormatCtx;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    //注册ffmpeg音视频设备
    av_register_all();
    avformat_network_init();
    avdevice_register_all();

    //参数设置
    AVDictionary* pOptions = NULL;
    //设置缓存
    av_dict_set(&pOptions, "buffer_size", "102400", 0);
    //设置编码格式
    av_dict_set(&pOptions, "input_format", "mjpeg", 0);
    //设置分辨率
    av_dict_set(&pOptions,"video_size","800x600",0);
    //设置帧率
    av_dict_set(&pOptions,"framerate","30",0);

    std::string str = _cameraName.toStdString();
    // 打开摄像头
    pFormatCtx = avformat_alloc_context();
    //AVInputFormat *ifmt = av_find_input_format("vfwcap");
    //if(avformat_open_input(&pFormatCtx, 0, ifmt, &pOptions))
    AVInputFormat *ifmt = av_find_input_format("dshow");
    if(avformat_open_input(&pFormatCtx,str.data(),ifmt,&pOptions)!=0)
    {
        qDebug("Couldn't open input stream.\n");
        return;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0)
    {
        qDebug("Couldn't find stream information.\n");
        return;
    }
    // 查找视频流id
    int videoindex = -1;
    for(int i=0; i < pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }
    if(videoindex==-1)
    {
        qDebug("Couldn't find a video stream.\n");
        return;
    }
    // 获取对应视频流编码格式
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL)
    {
        qDebug("Codec not found.\n");
        return;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
    {
        qDebug("Could not open codec.\n");
        return;
    }

#if P_USE_FILTER
    AVFrame *pFrameFilter;
    pFrameFilter = av_frame_alloc();
    initFilters(pFormatCtx);
#endif

    AVFrame *pFrame,*pFrameRGB;
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    // 创建图片缓存
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);
    uint8_t * rgbBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, rgbBuffer, AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height);

    // 创建包缓存
    int ret, got_picture;
    AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    // 创建图片转换格式结构体RGB32
    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    while(1)// 循环读取视频帧数据、解析为QIamge 发送到主线程UI显示
    {
        if(av_read_frame(pFormatCtx, packet) >= 0){
            if(packet->stream_index==videoindex){
                ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
                if(ret < 0)
                {
                    qDebug("Decode Error.\n");
                    return;
                }
                if(got_picture)
                {
#if P_USE_FILTER
                    pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);
                    if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
                        qDebug("Error while feeding the filtergraph\n");
                        break;
                    }
                    while (1) {
                        ret = av_buffersink_get_frame_flags(buffersink_ctx, pFrameFilter, 0);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                            break;
                        if (ret < 0)
                            return;
                        if(pFrameFilter)
                        {
                            sws_scale(img_convert_ctx, (const uint8_t* const*)pFrameFilter->data, pFrameFilter->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                            QImage tmpImg((uchar *)rgbBuffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                            QImage img = tmpImg.copy();
                            emit sgImage(img);
                        }
                        av_frame_unref(pFrameFilter);
                    }
#else
                    // AVFrame 转RGB32数据
                    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                    QImage tmpImg((uchar *)rgbBuffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                    qDebug() << "one framedata" << tmpImg.size() << QTime::currentTime().toString("hhmmss_zzz");
                    QImage img = tmpImg.copy();
                    emit sgImage(img);
                    QThread::msleep(10);
                    qDebug() <<QThread::currentThreadId();
#endif
                }
            }
            av_free_packet(packet);
        }
    }
    // 释放内存
    sws_freeContext(img_convert_ctx);
    av_free(rgbBuffer);
    av_free(pFrame);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
