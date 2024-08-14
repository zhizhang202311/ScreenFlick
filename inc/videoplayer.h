#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

bool playVideo(const std::string& videoPath);

#endif