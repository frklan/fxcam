// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>

#include <opencv4/opencv2/opencv.hpp>

#include "StopWatch.h"

class CameraDevice {
  public:
    struct FrameStatistics {
        std::chrono::duration<double, std::milli> frame_time;
        uint64_t frame_count = 0;
    };

    CameraDevice(int device_id = 0, int frame_width = 1920, int frame_height = 1080) {
      m_cap.open(device_id);

      m_cap.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(frame_width));
      m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(frame_height));
      m_cap.set(cv::CAP_PROP_FPS, 60.0);

      if(!m_cap.isOpened()) {
        throw std::runtime_error("Could not open camera!");
      }
    }

    ~CameraDevice() {
      stop();
    }

    void start() {
      if(m_running) {
        return;
      }

      m_running = true;
      m_process = std::thread([&]() {
        cv::Mat frame;
        StopWatch s;
        while(m_running) {
          s.start();
          m_cap.grab();
          m_cap.retrieve(frame);
          s.stop();

          {
            std::lock_guard<std::mutex> guard(m_mutex);
            std::swap(frame, m_last_frame);
            m_frame_stat.frame_time = s.elapsed_time();
            m_frame_stat.frame_count++;
          }
        }
      });
    }

    void stop() {
      if(!m_running) {
        return;
      }

      m_running = false;
      m_process.join();
      m_cap.release();
    }

    [[nodiscard]] FrameStatistics get_frame_stat() const {
      return m_frame_stat;
    }

    bool is_opened() const {
      return m_cap.isOpened();
    }

    int get_video_width() {
      return static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_WIDTH));
    }

    int get_video_height() {
      return static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    }

    operator cv::Mat() const {
      std::lock_guard<std::mutex> guard(m_mutex);
      return m_last_frame.clone();
    }

  private:
    cv::VideoCapture m_cap;
    mutable std::mutex m_mutex;
    cv::Mat m_last_frame;
    bool m_running = false;
    std::thread m_process;
    FrameStatistics m_frame_stat;
};
