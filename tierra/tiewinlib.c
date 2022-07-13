/* tierradll.c - hook filter function library source file */
#include <windows.h>
#include <time.h>

#pragma data_seg(".tierra")
__declspec(dllexport) time_t tmlstact=0;
#pragma data_seg()
#pragma comment(linker, "-section:.tierra,rws")

__declspec(dllexport) int  HookInstall(void);
__declspec(dllexport) void HookUnInstall(void);

static HHOOK  hKeyboardHook, hMouseHook;
static HINSTANCE hinstDLL;
static LRESULT CALLBACK KeyBFilter(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MouseFilter(int nCode, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{   BOOL rtnval=FALSE;
    switch (dwReason)
    {   case DLL_PROCESS_ATTACH:
            hinstDLL=hDLL;
            rtnval=TRUE;
            break;
        case DLL_PROCESS_DETACH:
            break;
        default:
            break;
    }
    return rtnval;
}

int HookInstall()
{   int rtnval=1;
    if(hKeyboardHook=SetWindowsHookEx(WH_KEYBOARD, KeyBFilter, hinstDLL, 0))
        if(hMouseHook=SetWindowsHookEx(WH_MOUSE, MouseFilter, hinstDLL, 0))
            rtnval=0;
    return rtnval;
}

void HookUnInstall()
{   UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}

LRESULT CALLBACK KeyBFilter(int nCode, WPARAM wParam, LPARAM lParam)
{   tmlstact=time((time_t *)0);
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseFilter(int nCode, WPARAM wParam, LPARAM lParam)
{   tmlstact=time((time_t *)0);
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}
