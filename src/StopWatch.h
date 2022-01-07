// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <chrono>

class StopWatch {
  public:
    void start() const {
      m_is_started = true;
      m_is_stoped = false;
      m_start_time = std::chrono::high_resolution_clock::now();
    }

    void stop() const {
      m_end_time = std::chrono::high_resolution_clock::now();
      m_is_started = false;
      m_is_stoped = true;
    }

    std::chrono::duration<double, std::milli> elapsed_time() const {
      if(!m_is_started && m_is_stoped) {
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_end_time - m_start_time);
      } else if(m_is_started && !m_is_stoped) {
        auto intervall_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(intervall_time - m_start_time);
      } else {
        return std::chrono::duration<double, std::milli>{0};
      }
    }

  private:
    mutable std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    mutable std::chrono::time_point<std::chrono::high_resolution_clock> m_end_time;

    mutable bool m_is_started = false;
    mutable bool m_is_stoped = false;
};
