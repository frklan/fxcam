// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: camera-BY-NC-4.0

#pragma once

template <typename T>
concept ICameraSource = requires(T t) {
  { t.get_video_width() } -> std::same_as<int>;
  { t.get_video_height() } -> std::same_as<int>;
  { t.is_opened() } -> std::same_as<bool>;
  { static_cast<cv::Mat>(t) } -> std::same_as<cv::Mat>;
};