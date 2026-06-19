// Copyright 2025 Skate 3 ReXGlue Recompiled Project
// SPDX-License-Identifier: MIT
//
// Shared utilities for installer wizard dialogs.
// Extracts common event loop patterns from ISO and Title Update installers.

#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

#include <rex/ui/window.h>
#include <rex/ui/windowed_app_context.h>
#include <rex/logging.h>

namespace skate3 {

/// Result structure for blocking wizard operations.
struct WizardInstallResult {
  bool done = false;
  bool ok = false;
};

/// Runs a platform-specific event pump while waiting for a wizard operation to complete.
/// This extracts the common pattern used by both ISO and Title Update installers.
///
/// @param app_context The application context for UI thread operations.
/// @param window Optional window for paint requests.
/// @param result Shared atomic result structure to check for completion.
/// @return true if the pump exited normally, false if the app is quitting.
inline bool RunWizardEventPumpBlocking(rex::ui::WindowedAppContext& app_context,
                                       rex::ui::Window* window,
                                       const std::shared_ptr<WizardInstallResult>& result) {
#if defined(_WIN32)
  HWND hwnd = nullptr;
  if (auto* win32_window = dynamic_cast<rex::ui::Win32Window*>(window)) {
    hwnd = win32_window->hwnd();
  }
#endif

  while (!result->done && !app_context.HasQuitFromUIThread()) {
    app_context.ExecutePendingFunctionsFromUIThread();

#if defined(_WIN32)
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        app_context.QuitFromUIThread();
        break;
      }
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
    if (app_context.HasQuitFromUIThread()) {
      break;
    }
    if (window) {
      window->RequestPaint();
    }
    if (hwnd) {
      RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
    }
#else
    if (window) {
      window->RequestPaint();
    }
#if !defined(__APPLE__)
    while (gtk_events_pending()) {
      gtk_main_iteration_do(FALSE);
    }
#endif
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  if (!result->ok) {
    return false;
  }

  return true;
}

}  // namespace skate3
