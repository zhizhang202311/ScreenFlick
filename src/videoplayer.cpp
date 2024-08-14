#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

/**
 * @brief 播放视频
 *
 * 根据给定的视频文件路径，使用SDL和FFmpeg库来播放视频。
 *
 * @param videoPath 视频文件路径
 *
 * @return 如果视频播放完成且未收到退出信号，则返回true；否则返回false
 * @throw std::runtime_error 当发生错误时抛出，包含错误描述
 */
bool playVideo(const std::string& videoPath) {
    const char *filename = videoPath.c_str();       // 将std::string类型的视频路径转换为C风格的字符串
    
    // 初始化SDL的视频、音频和定时器模块
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        throw std::runtime_error("Could not initialize SDL - " + std::string(SDL_GetError()));  // 如果初始化失败，抛出异常
    }

    // 打开视频文件
    AVFormatContext *pFormatCtx = nullptr;          // 定义一个格式上下文指针，初始为nullptr
    if (avformat_open_input(&pFormatCtx, filename, nullptr, nullptr) != 0)                      // 打开视频文件，填充格式上下文
    {
        SDL_Quit();// 如果打开失败，退出SDL
        throw std::runtime_error("Couldn't open file.");
    }

    // 检索视频流信息
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        avformat_close_input(&pFormatCtx);          // 如果检索失败，关闭格式上下文
        SDL_Quit();
        throw std::runtime_error("Couldn't find stream information.");
    }

    // 查找视频流
    int videoStream = -1;                           // 初始化视频流索引为-1
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)               // 遍历所有流
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {               // 如果流类型是视频
            videoStream = i;                        // 记录视频流索引
            break;                                  // 找到视频流后退出循环
        }
    }
    if (videoStream == -1) {                        // 如果没有找到视频流
        avformat_close_input(&pFormatCtx);
        SDL_Quit();
        throw std::runtime_error("Didn't find a video stream.");
    }

    // 获取视频流的编解码参数
    AVCodecParameters *pCodecPar = pFormatCtx->streams[videoStream]->codecpar;
    // 查找解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);            // 查找对应的解码器
    if (pCodec == nullptr) {
        avformat_close_input(&pFormatCtx);
        SDL_Quit();
        throw std::runtime_error("Unsupported codec!");
    }

    // 分配编解码器上下文
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);             // 分配编解码器上下文
    if (avcodec_parameters_to_context(pCodecCtx, pCodecPar) < 0) {          // 将编解码参数复制到上下文
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        SDL_Quit();
        throw std::runtime_error("Couldn't copy codec parameters to context.");
    }

    // 打开编解码器
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {    
        avcodec_free_context(&pCodecCtx);           // 如果打开失败，释放编解码器上下文
        avformat_close_input(&pFormatCtx);          // 关闭格式上下文
        SDL_Quit();
        throw std::runtime_error("Could not open codec.");
    }

    // 创建SDL窗口
    SDL_Window *screen = SDL_CreateWindow(videoPath,                                // 设置窗口标题
                                          SDL_WINDOWPOS_UNDEFINED,                  // 设置窗口位置
                                          SDL_WINDOWPOS_UNDEFINED,                  // 设置窗口位置
                                          pCodecCtx->width, pCodecCtx->height,      // 设置窗口大小
                                          SDL_WINDOW_SHOWN);                        // 创建窗口
    if (!screen) {
        avcodec_free_context(&pCodecCtx);       
        avformat_close_input(&pFormatCtx);
        SDL_Quit();
        throw std::runtime_error("SDL: could not set video mode - exiting");
    }

    // 创建SDL渲染器
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);                     
    // 创建SDL纹理
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV,        // 设置纹理格式
                                             SDL_TEXTUREACCESS_STREAMING,           // 设置纹理访问模式
                                             pCodecCtx->width, pCodecCtx->height);  // 设置纹理大小

    // 分配帧内存
    AVFrame *pFrame = av_frame_alloc();             // 分配帧内存
    AVFrame *pFrameYUV = av_frame_alloc();          // 分配帧内存
    if (!pFrame || !pFrameYUV) {
        SDL_DestroyTexture(texture);            
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        SDL_Quit();
        throw std::runtime_error("Could not allocate frame.");
    }

    // 分配YUV帧数据缓冲区
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 32);               // 计算缓冲区大小
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));                                                 // 分配缓冲区    
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 32);        // 填充YUV帧数据

    // 设置图像缩放上下文
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,                // 设置图像缩放上下文
                                                pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,                // 设置输出格式
                                                SWS_BILINEAR, nullptr, nullptr, nullptr);                               // 设置缩放算法

    int ret;            // 返回值
    AVPacket packet;    // 包
    SDL_Event event;    // 事件
    bool quit = false;  // 退出标志

    // 读取视频帧
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // 检查帧是否来自视频流
        if (packet.stream_index == videoStream) {
            // 发送包给解码器
            ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret < 0) {
                throw std::runtime_error("Error sending packet for decoding.");
            }

            // 接收解码后的帧
            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    throw std::runtime_error("Error during decoding.");
                }

                // 转换图像格式
                sws_scale(sws_ctx, (uint8_t const *const *)pFrame->data, pFrame->linesize, 0,
                          pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

                // 更新纹理数据
                SDL_UpdateYUVTexture(texture, nullptr,
                                     pFrameYUV->data[0], pFrameYUV->linesize[0],
                                     pFrameYUV->data[1], pFrameYUV->linesize[1],
                                     pFrameYUV->data[2], pFrameYUV->linesize[2]);

                // 渲染纹理
                SDL_RenderClear(renderer);          // 清空渲染器
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);        // 渲染纹理
                SDL_RenderPresent(renderer);        // 渲染

                // 在渲染帧后添加延迟，控制视频的帧率
                int desired_fps = 30;               // 视频的帧率
                SDL_Delay(1000 / desired_fps);      // 延迟

                // 处理SDL事件
                while (SDL_PollEvent(&event)) {     // 获取事件
                    if (event.type == SDL_QUIT) {
                        quit = true;
                    }
                }
            }
        }

        if (quit) {
            break;
        }

        // 释放包
        av_packet_unref(&packet);
    }

    // 释放资源
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    av_frame_free(&pFrame);
    av_frame_free(&pFrameYUV);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    av_free(buffer);
    SDL_Quit();

    return !quit;
}
