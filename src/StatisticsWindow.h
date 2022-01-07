// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <deque>
#include <memory>

#include <SDL2/SDL.h>
#include <imgui.h>

#include "Window.h"

constexpr size_t MAX_HISTORY_SIZE = 200;

class StatisticsWindow : public Window {
  public:
    void display() {
      m_fps_history.push_back(ImGui::GetIO().Framerate);
      if(m_fps_history.size() > MAX_HISTORY_SIZE) {
        m_fps_history.erase(m_fps_history.begin());
      }

      ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
      ImGui::Text("%.3f ms/frame", static_cast<double>(1000.0F / ImGui::GetIO().Framerate));
      ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
      auto max_y_scale = std::max_element(m_fps_history.begin(), m_fps_history.end());
      auto min_y_scale = std::min_element(m_fps_history.begin(), m_fps_history.end());

      assert(m_fps_history.size() < std::numeric_limits<int>::max);
      ImGui::PlotLines(
        "",
        [](void *data, int idx) -> float {
          auto fps = static_cast<std::deque<float> *>(data);
          assert(idx >= 0);
          return fps->at(static_cast<size_t>(idx));
        },
        static_cast<void *>(&m_fps_history),
        static_cast<int>(m_fps_history.size()),
        0,
        nullptr,
        *max_y_scale,
        *min_y_scale,
        ImVec2(0, 160.0f));

      ImGui::End();
    }

  private:
    std::deque<float> m_fps_history{};
};