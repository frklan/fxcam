// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <algorithm>
#include <exception>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <opencv4/opencv2/dnn.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include "ICameraSource.h"
#include "StopWatch.h"

template <typename Camera>
requires ICameraSource<Camera>
class FaceDetector {
  public:
    struct FrameStatistics {
        std::chrono::duration<double, std::milli> config_time;
        std::chrono::duration<double, std::milli> detect_time;
        std::chrono::duration<double, std::milli> draw_time;
        size_t count_faces_found;
    };

    FaceDetector(Camera &camera) : m_camera(camera) {
      m_network = cv::dnn::readNetFromCaffe("deploy.prototxt", "res10_300x300_ssd_iter_140000_fp16.caffemodel");

      if(m_network.empty()) {
        throw std::runtime_error("Could not create face detector");
      }
    }

    ~FaceDetector() {
      stop();
    }

    operator cv::Mat() const {
      return get_faces().second.clone();
    }

    bool is_opened() const {
      return m_camera.is_opened();
    }

    int get_video_width() {
      return m_camera.get_video_width();
    }

    int get_video_height() {
      return m_camera.get_video_height();
    }

    void start() {
      if(m_running) {
        return;
      }
      m_running = true;

      m_process = std::thread([&]() {
        while(m_running) {
          cv::Mat image = m_camera;
          find_faces(image);
        }
      });
    }

    void stop() {
      if(!m_running) {
        return;
      }

      m_running = false;
      m_process.join();
    }

    [[nodiscard]] std::pair<std::vector<cv::Rect>, cv::Mat> get_faces() const {
      std::lock_guard<std::mutex> guard(m_mutex);
      return m_found_faces;
    }

    [[nodiscard]] FrameStatistics get_stat() const {
      std::lock_guard<std::mutex> guard(m_mutex);
      return m_frame_stat;
    }

  private:
    Camera &m_camera;
    cv::dnn::Net m_network;
    FrameStatistics m_frame_stat;
    StopWatch m_config_watch;
    StopWatch m_detection_watch;
    StopWatch m_draw_watch;
    mutable std::mutex m_mutex;
    bool m_running = false;
    std::thread m_process;
    std::pair<std::vector<cv::Rect>, cv::Mat> m_found_faces;

    const float m_confidence_threshold{0.5F};// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const cv::Size m_blob_size{300, 300};// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const double m_input_scale{1.0};// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const cv::Scalar m_mean_values{104.0, 177.0, 123.0};// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const cv::Scalar m_face_color{0, 105, 205};// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers


    void find_faces(const cv::Mat image) {
      if(image.empty()) {
        return;
      }

      m_config_watch.start();
      auto image_blob = cv::dnn::blobFromImage(image, m_input_scale, m_blob_size, m_mean_values, false, false);
      m_config_watch.stop();
      m_detection_watch.start();
      m_network.setInput(image_blob, "data");
      auto detection = m_network.forward("detection_out");
      cv::Mat detected_matrix(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
      m_detection_watch.stop();

      m_draw_watch.start();
      auto image_with_faces = image.clone();
      std::vector<cv::Rect> face_coords;
      for(auto row = 0; row < detected_matrix.rows; row++) {
        auto confidence = detected_matrix.at<float>(row, 2);

        if(confidence >= m_confidence_threshold) {
          auto x_left_bottom = static_cast<int>(detected_matrix.at<float>(row, 3) * static_cast<float>(image.cols));
          auto y_left_bottom = static_cast<int>(detected_matrix.at<float>(row, 4) * static_cast<float>(image.rows));
          auto x_right_top = static_cast<int>(detected_matrix.at<float>(row, 5) * static_cast<float>(image.cols));
          auto y_right_top = static_cast<int>(detected_matrix.at<float>(row, 6) * static_cast<float>(image.rows));

          cv::Rect face{x_left_bottom, y_left_bottom, (x_right_top - x_left_bottom), (y_right_top - y_left_bottom)};
          face_coords.push_back(face);

          cv::rectangle(image_with_faces, face, m_face_color, 3);
        }
      }
      m_draw_watch.stop();

      {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_found_faces = std::make_pair(face_coords, image_with_faces);
        m_frame_stat = {
          .config_time = m_config_watch.elapsed_time(),
          .detect_time = m_detection_watch.elapsed_time(),
          .draw_time = m_draw_watch.elapsed_time(),
          .count_faces_found = face_coords.size(),
        };
      }
    }
};