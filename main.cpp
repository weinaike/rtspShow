
// 利用ffmpeg 库解码rtsp流，并将解码后的图片用opencv显示

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    char filepath[] = "rtsp://admin:ENAOLU@192.168.0.106:554/h264/1/main/av_stream";
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    // 提高缓冲区大小，解决rtsp播放卡顿问题
    AVDictionary *options = NULL;
    av_dict_set(&options, "buffer_size", "131072", 0);
    av_dict_set(&options, "max_delay", "500000", 0);

    if (avformat_open_input(&pFormatCtx, filepath, NULL, &options) != 0)
    {
        cout << "Couldn't open input stream." << endl;
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        cout << "Couldn't find stream information." << endl;
        return -1;
    }

    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }
    }

    if (videoindex == -1)
    {
        cout << "Couldn't find a video stream." << endl;
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        cout << "Codec not found." << endl;
        return -1;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        cout << "Could not open codec." << endl;
        return -1;
    }
    
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, 
            pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    av_frame_set_colorspace(pFrameYUV, AVCOL_SPC_BT709);
    av_frame_set_color_range(pFrameYUV, AVCOL_RANGE_JPEG);
    cv::Mat img;
    cv::namedWindow("Output");
    frame_cnt = 0;
    time_start = clock();
    
    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        if (packet->stream_index == videoindex)
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0)
            {
                cout << "Decode Error." << endl;
                return -1;
            }
            if (got_picture)
            {
                sws_scale(img_convert_ctx, (const uint8_t *const *)pFrame->data, pFrame->linesize, 0, 
                        pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                y_size = pCodecCtx->width * pCodecCtx->height;
                // print width hegiht
                // cout << "width:" << pCodecCtx->width << " height:" << pCodecCtx->height << endl;    
                // 将解码后的图片转换为opencv格式
                img.create(pCodecCtx->height * 3 / 2, pCodecCtx->width, CV_8UC1);
                memcpy(img.data, pFrameYUV->data[0], y_size);
                memcpy(img.data + y_size, pFrameYUV->data[1], y_size / 4);
                memcpy(img.data + y_size * 5 / 4, pFrameYUV->data[2], y_size / 4);

                // 将图片转换为BGR格式
                cv::cvtColor(img, img, CV_YUV2BGR_I420);

                // 图像缩小一半
                cv::resize(img, img, cv::Size(pCodecCtx->width / 2, pCodecCtx->height / 2));
                

                // 显示图像
                cv::imshow("Output", img);
                // esc 退出
                if (cv::waitKey(1) == 27)
                {
                    break;
                }

                // 间隔25帧打印一次
                if (frame_cnt % 25 == 0)
                {
                    cout << "Frame:" << frame_cnt << endl;
                }
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }
    time_finish = clock();
    time_duration = (double)(time_finish - time_start);
    cout << "Play time:" << time_duration / 1000 / 1000 << "s" << endl;
    sws_freeContext(img_convert_ctx);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}


