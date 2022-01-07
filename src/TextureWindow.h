// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <SDL2/SDL.h>
#include <imgui.h>

#include "Window.h"

template <typename Texture>
concept ITexture = requires(Texture t) {
  { static_cast<SDL_Texture *>(t) } -> std::same_as<SDL_Texture *>;
};

template <typename Texture>
requires ITexture<Texture>
class TextureWindow : public Window {
  public:
    TextureWindow(
      std::string_view title,
      Texture &texture) : m_title(title),
                          m_texture(texture) {
    }

    void display() {
      ImGui::Begin(m_title.data(), nullptr, ImGuiWindowFlags_NoCollapse);
      const ImVec2 window_size = ImGui::GetContentRegionAvail();
      ImVec2 texture_size{};

      if(window_size.x <= window_size.y * TextureWindow::TEXTURE_HEIGHT_RATIO) {
        texture_size.x = window_size.x;
        texture_size.y = window_size.x * TextureWindow::TEXTURE_WIDTH_RATIO;
      } else {
        texture_size.x = window_size.y * TextureWindow::TEXTURE_HEIGHT_RATIO;
        texture_size.y = window_size.y;
      }

      ImGui::Image(m_texture, texture_size);
      ImGui::End();
    }

  private:
    const std::string m_title;
    const Texture &m_texture;

    static constexpr float TEXTURE_HEIGHT_RATIO = 16.0F / 9.0F;
    static constexpr float TEXTURE_WIDTH_RATIO = 9.0F / 16.0F;
};
