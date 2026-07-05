#include "Keyboard.h"
#include "../Unity/Il2Cpp.h"
#include <string>

extern uintptr_t il2cpp;
extern MonoString* (*String_CreateString)(void* instance, const char* value, int startIndex, int length);

namespace Keyboard
{
    void* openedKeyboard = nullptr;
    std::function<void(const std::string &)> lastCallback = nullptr;
    std::string lastText = "";
    bool callbackOnDoneOnly = false;

    typedef void* (*OpenFunc)(MonoString*, int, bool, bool, bool, bool, MonoString*, int);
    typedef MonoString* (*GetTextFunc)(void*);
    typedef int (*GetStatusFunc)(void*);
    typedef void (*DestroyFunc)(void*);

    static OpenFunc OpenMethod = nullptr;
    static GetTextFunc GetTextMethod = nullptr;
    static GetStatusFunc GetStatusMethod = nullptr;
    static DestroyFunc DestroyMethod = nullptr;

    void Init()
    {
        OpenMethod = (OpenFunc)AOV_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "TouchScreenKeyboard", "Open", 8);
        GetTextMethod = (GetTextFunc)AOV_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "TouchScreenKeyboard", "get_text", 0);
        GetStatusMethod = (GetStatusFunc)AOV_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "TouchScreenKeyboard", "get_status", 0);
        DestroyMethod = (DestroyFunc)AOV_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "TouchScreenKeyboard", "Destroy", 0);
    }

    void Open(const std::function<void(const std::string &)> &callback, bool onDoneOnly)
    {
        Open("", callback, onDoneOnly);
    }

    void Open(const char *text, const std::function<void(const std::string &)> &callback, bool onDoneOnly)
    {
        if (!OpenMethod || !String_CreateString) return;
        MonoString* textStr = String_CreateString(nullptr, text, 0, strlen(text));
        MonoString* emptyStr = String_CreateString(nullptr, "", 0, 0);
        openedKeyboard = OpenMethod(textStr, 0, false, false, false, false, emptyStr, 0);
        lastCallback = callback;
        lastText = text;
        callbackOnDoneOnly = onDoneOnly;
    }

    void Reset()
    {
        if (openedKeyboard && DestroyMethod) {
            DestroyMethod(openedKeyboard);
        }
        openedKeyboard = nullptr;
        lastCallback = nullptr;
        lastText = "";
        callbackOnDoneOnly = false;
    }

    void Update()
    {
        if (!openedKeyboard || !GetStatusMethod)
            return;

        int status = GetStatusMethod(openedKeyboard);

        if (status == 0 || status == 1)
        {
            if (GetTextMethod)
            {
                MonoString* textStr = GetTextMethod(openedKeyboard);
                if (textStr)
                {
                    std::string currentText = textStr->toString();
                    if (lastCallback && currentText != lastText && !callbackOnDoneOnly)
                    {
                        lastText = currentText;
                        lastCallback(currentText);
                    }

                    if (status == 1)
                    {
                        if (lastCallback && callbackOnDoneOnly)
                        {
                            lastCallback(currentText);
                        }
                        Reset();
                    }
                    return;
                }
            }
        }
        else
        {
            Reset();
        }
    }

    bool IsOpen()
    {
        return openedKeyboard != nullptr;
    }
} // namespace Keyboard
