// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "Window.h"

class DockSpaceWindow final {
  public:
    explicit DockSpaceWindow(std::string_view title, std::shared_ptr<SDL_Renderer> renderer) : m_window_title(title),
                                                                                               m_renderer(renderer) {}

    ~DockSpaceWindow() = default;

    void add_window(std::shared_ptr<Window> w) {
      m_windows.push_back(w);
    }

    void display() {
      begin_dockspace();
      for(const auto &w : m_windows) {
        w->display();
      }
      end_dockspace();
    };


  private:
    const std::string m_window_title;
    const std::shared_ptr<SDL_Renderer> m_renderer;
    std::vector<std::shared_ptr<Window>> m_windows;

    void begin_dockspace() {
      auto *viewport = ImGui::GetMainViewport();

      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);

      bool is_open = true;
      ImGui::Begin("DockSpace",
        &is_open,
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

      ImGui::Text("");

      const auto dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), ImGuiDockNodeFlags_None /*| ImGuiDockNodeFlags_PassthruCentralNode*/);
      // main_menu_show();
    }

    void end_dockspace() {
      ImGui::End();
    }
};