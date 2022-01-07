// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#include <chrono>
#include <deque>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <SDL2/SDL.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <spdlog/spdlog.h>

#include "CameraDevice.h"
#include "DockSpaceWindow.h"
#include "FaceDetector.h"
#include "StatisticsWindow.h"
#include "StopWatch.h"
#include "TextureWindow.h"
#include "VideoTexture.h"

using namespace std::chrono_literals;
using namespace std::string_view_literals;

constexpr int WIDTH = 1920 /*2560*/ /*3840*/;
constexpr int HEIGHT = 1080 /*1440*/ /*2160 */;
constexpr int VIDEO_DEVICE = 0;
constexpr float UI_SCALE = 1.5F;

void set_theme() {
  // TODO: a) set all segements, b) read theme from disk.

  const ImVec4 bg = ImColor(0x28, 0x2a, 0x36);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  const ImVec4 bg_darker = ImColor(0x19, 0x1a, 0x21);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  const ImVec4 purple = ImColor(0xbd, 0x93, 0xf9, 0x55);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, {1.0F, 1.0F});// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0F);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0F);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

  ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, bg_darker);
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, bg_darker);
  ImGui::PushStyleColor(ImGuiCol_Border, purple);
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg, bg_darker);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, bg_darker);
}

using shared_renderer_t = std::shared_ptr<SDL_Renderer>;
using shared_texture_t = std::shared_ptr<SDL_Texture>;
using shared_window_t = std::shared_ptr<SDL_Window>;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);

  spdlog::info("--- Connect Debugger NOW! ---");
  std::this_thread::sleep_for(10s);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  auto window = shared_window_t{SDL_CreateWindow("fxcam", 0, 0, WIDTH, HEIGHT, /*SDL_WINDOW_ALLOW_HIGHDPI |*/ SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE), &SDL_DestroyWindow};
  auto renderer = shared_renderer_t{SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), &SDL_DestroyRenderer};
  SDL_ShowWindow(window.get());

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui::GetStyle().ScaleAllSizes(UI_SCALE);
  ImGui_ImplSDL2_InitForSDLRenderer(window.get());
  ImGui_ImplSDLRenderer_Init(renderer.get());

  ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  try {
    CameraDevice camera{VIDEO_DEVICE};
    camera.start();
    FaceDetector face_detector(camera);
    face_detector.start();

    VideoTexture camera_texture(camera, renderer);
    VideoTexture face_detector_texture(face_detector, renderer);

    auto stat_window = std::make_shared<StatisticsWindow>();
    auto camera_window = std::make_shared<TextureWindow<decltype(camera_texture)>>("Camera feed"sv, camera_texture);
    auto face_detector_window = std::make_shared<TextureWindow<decltype(face_detector_texture)>>("Faces"sv, face_detector_texture);

    DockSpaceWindow main_dockspace{"dock space 1"sv, renderer};
    main_dockspace.add_window(stat_window);
    main_dockspace.add_window(camera_window);
    main_dockspace.add_window(face_detector_window);

    // Main loop
    bool should_quit{false};
    SDL_Event event{};
    while(!should_quit) {
      while(SDL_PollEvent(&event) != 0) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch(event.type) {
        case SDL_QUIT:
          should_quit = true;
          break;
        default:
          break;
        }
      }

      /* Draw stuff */
      ImGui_ImplSDLRenderer_NewFrame();
      ImGui_ImplSDL2_NewFrame(window.get());
      ImGui::NewFrame();

      set_theme();

      StopWatch gui_draw_clock;
      gui_draw_clock.start();
      main_dockspace.display();
      gui_draw_clock.stop();

      ImGui::Begin("stat", nullptr);
      if(ImGui::BeginTable("Stats", 2)) {
        ImGui::TableNextColumn();
        ImGui::Text("-- GUI --");
        ImGui::Text("time to draw GUI: %.3f mS", gui_draw_clock.elapsed_time().count());
        ImGui::Separator();
        ImGui::Text("-- Camera --");
        ImGui::Text("Camera frame time: %.3f mS", camera.get_frame_stat().frame_time.count());
        ImGui::Text("Camera frame #: %llu", camera.get_frame_stat().frame_count);

        ImGui::TableNextColumn();

        ImGui::Text("-- Face detection --");
        ImGui::Text("FaceDetector config time: %.3f mS", face_detector.get_stat().config_time.count());
        ImGui::Text("Time to detect faces: %.3f mS", face_detector.get_stat().detect_time.count());
        ImGui::Text("Time to draw faces: %.3f mS", face_detector.get_stat().draw_time.count());
        ImGui::Text("Number of faces found: %lu", face_detector.get_stat().count_faces_found);
        ImGui::Separator();

        const auto &[faces, _] = face_detector.get_faces();
        if(faces.empty()) {
          ImGui::Text("-No face detected-");
        } else {
          if(ImGui::BeginTable("DetectedFaces", 4)) {
            ImGui::TableSetupColumn("x");
            ImGui::TableNextColumn();
            ImGui::TableSetupColumn("y");
            ImGui::TableNextColumn();
            ImGui::TableSetupColumn("height");
            ImGui::TableNextColumn();
            ImGui::TableSetupColumn("width");
            ImGui::TableHeadersRow();
            for(const auto &face : faces) {
              ImGui::TableNextColumn();
              ImGui::Text("%d", face.x);
              ImGui::TableNextColumn();
              ImGui::Text("%d", face.y);
              ImGui::TableNextColumn();
              ImGui::Text("%d", face.height);
              ImGui::TableNextColumn();
              ImGui::Text("%d", face.width);
            }
            ImGui::EndTable();
          }
        }
        ImGui::EndTable();
      }
      ImGui::End();

      // ImGui::ShowMetricsWindow();
      // ImGui::ShowStackToolWindow();
      // ImGui::ShowDemoWindow();
      // ImGui::ShowStyleEditor(nullptr);

      ImGui::Render();
      SDL_RenderClear(renderer.get());
      ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
      SDL_RenderPresent(renderer.get());

      std::this_thread::sleep_for(1ns);
    };
    camera.stop();
  } catch(cv::Exception &e) {
    spdlog::error("%s, %s, %d", e.what(), e.file, e.line);
  } catch(std::exception &e) {
    spdlog::error(e.what());
  } catch(...) {
    spdlog::error("Ohter error occured..");
  }

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_Quit();

  return 0;
}
