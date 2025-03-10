﻿/*
 * Copyright (c) 2016-present The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/ZLMediaKit/ZLMediaKit).
 *
 * Use of this source code is governed by MIT-like license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef FFMPEG_SOURCE_H
#define FFMPEG_SOURCE_H

#include <mutex>
#include <memory>
#include <functional>
#include "Process.h"
#include "Util/TimeTicker.h"
#include "Common/MediaSource.h"

namespace FFmpeg {
    extern const std::string kSnap;
    extern const std::string kBin;
}

class FFmpegSnap {
public:
    using onSnap = std::function<void(bool success, const std::string &err_msg)>;
    /**
     * 创建截图  [AUTO-TRANSLATED:6d334c49]
     * Create a screenshot
     * @param async 是否使用异步截图方式(非ffmpeg命令行，而是使用zlm api，但是仅限于zlm播放器支持的拉流协议)
     * @param play_url 播放url地址，只要FFmpeg支持即可  [AUTO-TRANSLATED:609d4de4]
     * @param play_url The playback URL address, as long as FFmpeg supports it
     * @param save_path 截图jpeg文件保存路径  [AUTO-TRANSLATED:0fc0ac0d]
     * @param save_path The path to save the screenshot JPEG file
     * @param timeout_sec 生成截图超时时间(防止阻塞太久)  [AUTO-TRANSLATED:0dcc0095]
     * @param timeout_sec Timeout for generating the screenshot (to prevent blocking for too long)
     * @param cb 生成截图成功与否回调  [AUTO-TRANSLATED:5b4b93c9]
     * @param cb Callback for whether the screenshot was generated successfully
     */
    static void makeSnap(bool async, const std::string &play_url, const std::string &save_path, float timeout_sec, const onSnap &cb);

private:
    FFmpegSnap() = delete;
    ~FFmpegSnap() = delete;
};

class FFmpegSource : public std::enable_shared_from_this<FFmpegSource> , public mediakit::MediaSourceEventInterceptor{
public:
    using Ptr = std::shared_ptr<FFmpegSource>;
    using onPlay = std::function<void(const toolkit::SockException &ex)>;

    FFmpegSource();
    ~FFmpegSource();

    /**
     * 设置主动关闭回调
     * Set the active close callback
     
     * [AUTO-TRANSLATED:2134a5b3]
     */
    void setOnClose(const std::function<void()> &cb);

    /**
     * 开始播放url
     * @param ffmpeg_cmd_key FFmpeg拉流命令配置项key，用户可以在配置文件中同时设置多个命令参数模板
     * @param src_url FFmpeg拉流地址
     * @param dst_url FFmpeg推流地址
     * @param timeout_ms 等待结果超时时间，单位毫秒
     * @param cb 成功与否回调
     * Start playing the URL
     * @param ffmpeg_cmd_key FFmpeg stream command configuration item key, users can set multiple command parameter templates in the configuration file at the same time
     * @param src_url FFmpeg stream address
     * @param dst_url FFmpeg push stream address
     * @param timeout_ms Timeout for waiting for the result, in milliseconds
     * @param cb Success or failure callback
     
     * [AUTO-TRANSLATED:2c35789e]
     */
    void play(const std::string &ffmpeg_cmd_key, const std::string &src_url, const std::string &dst_url, int timeout_ms, const onPlay &cb);

    const std::string& getSrcUrl() const { return _src_url; }
    const std::string& getDstUrl() const { return _dst_url; }
    const std::string& getCmd() const { return _cmd; }
    const std::string& getCmdKey() const { return _ffmpeg_cmd_key; }
    const mediakit::MediaInfo& getMediaInfo() const { return _media_info; }

    /**
     * 设置录制
     * @param enable_hls 是否开启hls直播或录制
     * @param enable_mp4 是否录制mp4
     * Set recording
     * @param enable_hls Whether to enable HLS live streaming or recording
     * @param enable_mp4 Whether to record MP4
     
     * [AUTO-TRANSLATED:9f28d5c2]
     */
    void setupRecordFlag(bool enable_hls, bool enable_mp4);

private:
    void findAsync(int maxWaitMS ,const std::function<void(const mediakit::MediaSource::Ptr &src)> &cb);
    void startTimer(int timeout_ms);
    void onGetMediaSource(const mediakit::MediaSource::Ptr &src);

    ///////MediaSourceEvent override///////
    // 关闭  [AUTO-TRANSLATED:92392f02]
    // Close
    bool close(mediakit::MediaSource &sender) override;
    // 获取媒体源类型  [AUTO-TRANSLATED:34290a69]
    // Get the media source type
    mediakit::MediaOriginType getOriginType(mediakit::MediaSource &sender) const override;
    // 获取媒体源url或者文件路径  [AUTO-TRANSLATED:d6d885b8]
    // Get the media source URL or file path
    std::string getOriginUrl(mediakit::MediaSource &sender) const override;

private:
    bool _enable_hls = false;
    bool _enable_mp4 = false;
    Process _process;
    toolkit::Timer::Ptr _timer;
    toolkit::EventPoller::Ptr _poller;
    mediakit::MediaInfo _media_info;
    std::string _src_url;
    std::string _dst_url;
    std::string _ffmpeg_cmd_key;
    std::string _cmd;
    std::function<void()> _onClose;
    toolkit::Ticker _replay_ticker;
};


#endif //FFMPEG_SOURCE_H
