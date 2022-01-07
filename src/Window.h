// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

#include <memory>

#include <SDL2/SDL.h>

class Window {
  public:
    virtual ~Window() = default;

    virtual void display() = 0;
};