#pragma once

#include <functional>
#include <string>
#include "quoctoandevlor/Tools/MonoString.h"

enum TouchScreenKeyboardStatus
{
    Visible = 0,
    Done = 1,
    Canceled = 2,
    LostFocus = 3
};

namespace Keyboard
{
    void Init();
    void Open(const std::function<void(const std::string &)> &callback, bool onDoneOnly = false);
    void Open(const char *text, const std::function<void(const std::string &)> &callback, bool onDoneOnly = false);
    void Reset();
    void Update();
    bool IsOpen();
} // namespace Keyboard
