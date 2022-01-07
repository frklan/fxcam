// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <memory>

#include <SDL2/SDL.h>
#include <opencv4/opencv2/opencv.hpp>

#include "ICameraSource.h"

template <typename Camera>
requires ICameraSource<Camera>
class VideoTexture {
  public:
    VideoTexture(Camera &camera,
      std::shared_ptr<SDL_Renderer> renderer) : m_camera(camera),
                                                m_renderer(renderer),
                                                m_texture(nullptr) {
      if(!m_camera.is_opened()) {
        throw std::runtime_error("Video device unavilable");
      }
      auto video_height = m_camera.get_video_height();
      auto video_width = m_camera.get_video_width();
      m_texture = SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, video_width, video_height);
      if(m_texture == nullptr) {
        throw std::runtime_error("Could not allocate texture");
      }
    }

    ~VideoTexture() {
      if(m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);
      }
      m_texture = nullptr;
    }

    operator SDL_Texture *() const {
      cv::Mat image{m_camera};
      const size_t image_size{static_cast<size_t>(image.cols) * static_cast<size_t>(image.rows) * static_cast<size_t>(image.channels())};
      int texture_pitch{0};
      void *texture_data{nullptr};

      auto r = SDL_LockTexture(m_texture, nullptr, &texture_data, &texture_pitch);
      if(r == 0) {// Texture locked
        std::memcpy(texture_data, image.data, image_size);
        SDL_UnlockTexture(m_texture);
      }
      return m_texture;
    }

  private:
    Camera &m_camera;
    std::shared_ptr<SDL_Renderer> m_renderer;
    SDL_Texture *m_texture = nullptr;
};
