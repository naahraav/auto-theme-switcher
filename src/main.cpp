#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define IDT_TIMER          1001

#define IDC_EDIT_LIGHT     2001
#define IDC_EDIT_DARK      2002
#define IDC_COMBO_DAY_WIN  2003
#define IDC_COMBO_DAY_APP  2004
#define IDC_COMBO_NIT_WIN  2005
#define IDC_COMBO_NIT_APP  2006
#define IDC_BTN_SAVE       2007
#define IDC_BTN_TOGGLE     2008
#define IDC_LBL_STATUS     2009
#define IDC_BOX_INFO       2010
#define IDC_CHK_STARTUP    2011

#define WM_TRAYICON        (WM_USER + 1)
#define IDM_TRAY_OPEN      3001
#define IDM_TRAY_TOGGLE    3002
#define IDM_TRAY_STARTUP   3003
#define IDM_TRAY_EXIT      3004

// Global Variables
HWND hEditLight, hEditDark;
HWND hCmbDayWin, hCmbDayApp, hCmbNitWin, hCmbNitApp;
HWND hBtnSave, hBtnToggle, hLblStatus, hBoxInfo, hChkStartup;
HFONT hFontSegoe, hFontBold, hFontSmall, hFontEdit;
NOTIFYICONDATAW nid = {};

// Schedule & Mode Configurations (1 = Light, 0 = Dark)
int lightHour = 6, lightMin = 0;
int darkHour = 18, darkMin = 0;
int dayWinMode = 1;   // Default Day Windows: Light
int dayAppMode = 1;   // Default Day App: Light
int nightWinMode = 0; // Default Night Windows: Dark
int nightAppMode = 0; // Default Night App: Dark

bool isRunning = true;
int lastAppliedWinMode = -1;
int lastAppliedAppMode = -1;

// Load Saved Settings from Registry
void LoadSettings() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\AutoThemeSwitcher", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwSize = sizeof(DWORD);
        DWORD val;
        if (RegQueryValueExW(hKey, L"LightHour", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) lightHour = val;
        if (RegQueryValueExW(hKey, L"LightMin", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) lightMin = val;
        if (RegQueryValueExW(hKey, L"DarkHour", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) darkHour = val;
        if (RegQueryValueExW(hKey, L"DarkMin", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) darkMin = val;

        if (RegQueryValueExW(hKey, L"DayWinMode", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) dayWinMode = val;
        if (RegQueryValueExW(hKey, L"DayAppMode", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) dayAppMode = val;
        if (RegQueryValueExW(hKey, L"NightWinMode", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) nightWinMode = val;
        if (RegQueryValueExW(hKey, L"NightAppMode", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) nightAppMode = val;

        if (RegQueryValueExW(hKey, L"IsRunning", NULL, NULL, (LPBYTE)&val, &dwSize) == ERROR_SUCCESS) isRunning = (val != 0);
        RegCloseKey(hKey);
    }
}

// Save Settings to Registry
void SaveSettings() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\AutoThemeSwitcher", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        DWORD val;
        val = lightHour; RegSetValueExW(hKey, L"LightHour", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = lightMin;  RegSetValueExW(hKey, L"LightMin", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = darkHour;  RegSetValueExW(hKey, L"DarkHour", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = darkMin;   RegSetValueExW(hKey, L"DarkMin", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));

        val = dayWinMode;   RegSetValueExW(hKey, L"DayWinMode", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = dayAppMode;   RegSetValueExW(hKey, L"DayAppMode", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = nightWinMode; RegSetValueExW(hKey, L"NightWinMode", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        val = nightAppMode; RegSetValueExW(hKey, L"NightAppMode", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));

        val = isRunning ? 1 : 0; RegSetValueExW(hKey, L"IsRunning", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

bool IsAutoStartEnabled() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t path[MAX_PATH];
        DWORD pathLen = sizeof(path);
        LONG res = RegQueryValueExW(hKey, L"AutoThemeSwitcher", NULL, NULL, (LPBYTE)path, &pathLen);
        RegCloseKey(hKey);
        return (res == ERROR_SUCCESS);
    }
    return false;
}

void SetAutoStart(bool enable) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (enable) {
            wchar_t szPath[MAX_PATH];
            GetModuleFileNameW(NULL, szPath, MAX_PATH);
            std::wstring cmd = L"\"" + std::wstring(szPath) + L"\" --startup";
            RegSetValueExW(hKey, L"AutoThemeSwitcher", 0, REG_SZ, (const BYTE*)cmd.c_str(), (DWORD)((cmd.length() + 1) * sizeof(wchar_t)));
        } else {
            RegDeleteValueW(hKey, L"AutoThemeSwitcher");
        }
        RegCloseKey(hKey);
    }
}

// Instantly apply Custom Windows & App Themes
void ApplyWindowsTheme(int winMode, int appMode) {
    DWORD dwWin = (DWORD)winMode;
    DWORD dwApp = (DWORD)appMode;

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"SystemUsesLightTheme", 0, REG_DWORD, (const BYTE*)&dwWin, sizeof(dwWin));
        RegSetValueExW(hKey, L"AppsUseLightTheme", 0, REG_DWORD, (const BYTE*)&dwApp, sizeof(dwApp));
        RegCloseKey(hKey);
    }

    DWORD_PTR dwResult;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 200, &dwResult);
}

void CheckAndApplySchedule() {
    if (!isRunning) return;

    SYSTEMTIME st;
    GetLocalTime(&st);

    int curMinutes = st.wHour * 60 + st.wMinute;
    int lightMinutes = lightHour * 60 + lightMin;
    int darkMinutes = darkHour * 60 + darkMin;

    bool isDayPeriod = false;
    if (lightMinutes < darkMinutes) {
        isDayPeriod = (curMinutes >= lightMinutes && curMinutes < darkMinutes);
    } else {
        isDayPeriod = (curMinutes >= lightMinutes || curMinutes < darkMinutes);
    }

    int targetWin = isDayPeriod ? dayWinMode : nightWinMode;
    int targetApp = isDayPeriod ? dayAppMode : nightAppMode;

    if (lastAppliedWinMode != targetWin || lastAppliedAppMode != targetApp) {
        lastAppliedWinMode = targetWin;
        lastAppliedAppMode = targetApp;
        ApplyWindowsTheme(targetWin, targetApp);
    }
}

void UpdateInfoBox() {
    std::wstringstream ss;
    ss << std::setfill(L'0') 
       << L"  Day Schedule   (" << std::setw(2) << lightHour << L":" << std::setw(2) << lightMin << L")  ->  Windows: " 
       << (dayWinMode ? L"Light" : L"Dark") << L" | Apps: " << (dayAppMode ? L"Light" : L"Dark") << L"\n\n"
       << L"  Night Schedule (" << std::setw(2) << darkHour << L":" << std::setw(2) << darkMin << L")  ->  Windows: " 
       << (nightWinMode ? L"Light" : L"Dark") << L" | Apps: " << (nightAppMode ? L"Light" : L"Dark");
    SetWindowTextW(hBoxInfo, ss.str().c_str());
}

bool ParseTimeText(HWND hEdit, int& hour, int& min) {
    wchar_t buf[32];
    GetWindowTextW(hEdit, buf, 32);
    int h = -1, m = -1;
    if (swscanf(buf, L"%d:%d", &h, &m) == 2) {
        if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
            hour = h;
            min = m;
            return true;
        }
    }
    return false;
}

void InitCombo(HWND hCombo, int modeValue) {
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Light");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Dark");
    // Mode 1 = Light (Index 0), Mode 0 = Dark (Index 1)
    SendMessageW(hCombo, CB_SETCURSEL, (modeValue == 1) ? 0 : 1, 0);
    SendMessageW(hCombo, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
}

int GetComboValue(HWND hCombo) {
    int curSel = (int)SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
    return (curSel == 0) ? 1 : 0;
}

void ToggleService(HWND hwnd) {
    isRunning = !isRunning;
    SaveSettings();
    InvalidateRect(hBtnToggle, NULL, TRUE);

    if (isRunning) {
        SetWindowTextW(hLblStatus, L"Status: ACTIVE (Theme Auto-switch is running in background)");
        lastAppliedWinMode = -1;
        lastAppliedAppMode = -1;
        CheckAndApplySchedule();
    } else {
        SetWindowTextW(hLblStatus, L"Status: INACTIVE (Auto-switch is paused)");
    }

    InvalidateRect(hLblStatus, NULL, TRUE);
    UpdateWindow(hLblStatus);
}

void ShowTrayNotification() {
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        nid.uFlags |= NIF_INFO;
        wcscpy(nid.szInfo, L"App is running in background. Double-click tray icon to open.");
        wcscpy(nid.szInfoTitle, L"Auto Theme Switcher");
        nid.dwInfoFlags = NIIF_INFO;
        Shell_NotifyIconW(NIM_MODIFY, &nid);
        nid.uFlags &= ~NIF_INFO;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        LoadSettings();

        hFontSegoe = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        hFontBold = CreateFontW(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        hFontSmall = CreateFontW(13, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        hFontEdit = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        // --- LEFT COLUMN: DAY / LIGHT SCHEDULE ---
        HWND lblDay = CreateWindowW(L"STATIC", L"Day Schedule Start (HH:MM):", WS_VISIBLE | WS_CHILD, 30, 15, 215, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblDay, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        std::wstringstream s1;
        s1 << std::setfill(L'0') << std::setw(2) << lightHour << L":" << std::setw(2) << lightMin;
        hEditLight = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", s1.str().c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL, 30, 36, 215, 28, hwnd, (HMENU)IDC_EDIT_LIGHT, NULL, NULL);
        SendMessage(hEditLight, WM_SETFONT, (WPARAM)hFontEdit, TRUE);

        HWND lblDW = CreateWindowW(L"STATIC", L"Windows mode:", WS_VISIBLE | WS_CHILD, 30, 72, 105, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblDW, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
        hCmbDayWin = CreateWindowExW(0, WC_COMBOBOXW, L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 140, 69, 105, 120, hwnd, (HMENU)IDC_COMBO_DAY_WIN, NULL, NULL);
        InitCombo(hCmbDayWin, dayWinMode);

        HWND lblDA = CreateWindowW(L"STATIC", L"App mode:", WS_VISIBLE | WS_CHILD, 30, 102, 105, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblDA, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
        hCmbDayApp = CreateWindowExW(0, WC_COMBOBOXW, L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 140, 99, 105, 120, hwnd, (HMENU)IDC_COMBO_DAY_APP, NULL, NULL);
        InitCombo(hCmbDayApp, dayAppMode);

        // --- RIGHT COLUMN: NIGHT / DARK SCHEDULE ---
        HWND lblNit = CreateWindowW(L"STATIC", L"Night Schedule Start (HH:MM):", WS_VISIBLE | WS_CHILD, 275, 15, 215, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblNit, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        std::wstringstream s2;
        s2 << std::setfill(L'0') << std::setw(2) << darkHour << L":" << std::setw(2) << darkMin;
        hEditDark = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", s2.str().c_str(), WS_VISIBLE | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL, 275, 36, 215, 28, hwnd, (HMENU)IDC_EDIT_DARK, NULL, NULL);
        SendMessage(hEditDark, WM_SETFONT, (WPARAM)hFontEdit, TRUE);

        HWND lblNW = CreateWindowW(L"STATIC", L"Windows mode:", WS_VISIBLE | WS_CHILD, 275, 72, 105, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblNW, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
        hCmbNitWin = CreateWindowExW(0, WC_COMBOBOXW, L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 385, 69, 105, 120, hwnd, (HMENU)IDC_COMBO_NIT_WIN, NULL, NULL);
        InitCombo(hCmbNitWin, nightWinMode);

        HWND lblNA = CreateWindowW(L"STATIC", L"App mode:", WS_VISIBLE | WS_CHILD, 275, 102, 105, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblNA, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
        hCmbNitApp = CreateWindowExW(0, WC_COMBOBOXW, L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 385, 99, 105, 120, hwnd, (HMENU)IDC_COMBO_NIT_APP, NULL, NULL);
        InitCombo(hCmbNitApp, nightAppMode);

        // Save Button (Blue)
        hBtnSave = CreateWindowW(L"BUTTON", L"+ Save Schedule", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 30, 138, 460, 36, hwnd, (HMENU)IDC_BTN_SAVE, NULL, NULL);

        // Active Schedule Box
        HWND lblList = CreateWindowW(L"STATIC", L"Active Custom Schedule:", WS_VISIBLE | WS_CHILD, 30, 185, 200, 18, hwnd, NULL, NULL, NULL);
        SendMessage(lblList, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        hBoxInfo = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", L"", WS_VISIBLE | WS_CHILD, 30, 205, 460, 62, hwnd, (HMENU)IDC_BOX_INFO, NULL, NULL);
        SendMessage(hBoxInfo, WM_SETFONT, (WPARAM)hFontSmall, TRUE);
        UpdateInfoBox();

        // Checkbox Auto Startup
        hChkStartup = CreateWindowW(L"BUTTON", L"  Run on Windows Startup (Start Minimized to Tray)", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 30, 275, 460, 24, hwnd, (HMENU)IDC_CHK_STARTUP, NULL, NULL);
        SendMessage(hChkStartup, WM_SETFONT, (WPARAM)hFontSegoe, TRUE);
        SendMessage(hChkStartup, BM_SETCHECK, IsAutoStartEnabled() ? BST_CHECKED : BST_UNCHECKED, 0);

        // Action Button (STOP / START)
        hBtnToggle = CreateWindowW(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 30, 310, 460, 44, hwnd, (HMENU)IDC_BTN_TOGGLE, NULL, NULL);

        // Status Text
        const wchar_t* initStatus = isRunning ? L"Status: ACTIVE (Theme Auto-switch is running in background)" : L"Status: INACTIVE (Auto-switch is paused)";
        hLblStatus = CreateWindowW(L"STATIC", initStatus, WS_VISIBLE | WS_CHILD | SS_CENTER, 30, 362, 460, 22, hwnd, (HMENU)IDC_LBL_STATUS, NULL, NULL);
        SendMessage(hLblStatus, WM_SETFONT, (WPARAM)hFontSmall, TRUE);

        // Setup Tray Icon
        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcscpy(nid.szTip, L"Auto Theme Switcher");
        Shell_NotifyIconW(NIM_ADD, &nid);

        // Check schedule every 5 seconds
        SetTimer(hwnd, IDT_TIMER, 5000, NULL);
        CheckAndApplySchedule();
        break;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        if (pDIS->CtlID == IDC_BTN_SAVE) {
            HBRUSH hBrush = CreateSolidBrush(RGB(30, 100, 235));
            FillRect(pDIS->hDC, &pDIS->rcItem, hBrush);
            DeleteObject(hBrush);

            SetTextColor(pDIS->hDC, RGB(255, 255, 255));
            SetBkMode(pDIS->hDC, TRANSPARENT);
            SelectObject(pDIS->hDC, hFontBold);
            DrawTextW(pDIS->hDC, L"+ Save Schedule", -1, &pDIS->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return TRUE;
        }
        else if (pDIS->CtlID == IDC_BTN_TOGGLE) {
            COLORREF btnColor = isRunning ? RGB(214, 48, 49) : RGB(46, 125, 50);
            HBRUSH hBrush = CreateSolidBrush(btnColor);
            FillRect(pDIS->hDC, &pDIS->rcItem, hBrush);
            DeleteObject(hBrush);

            SetTextColor(pDIS->hDC, RGB(255, 255, 255));
            SetBkMode(pDIS->hDC, TRANSPARENT);
            SelectObject(pDIS->hDC, hFontBold);
            const wchar_t* text = isRunning ? L"STOP (DISABLE AUTO-SWITCH)" : L"START (ENABLE AUTO-SWITCH)";
            DrawTextW(pDIS->hDC, text, -1, &pDIS->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return TRUE;
        }
        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hStaticWnd = (HWND)lParam;
        if (hStaticWnd == hLblStatus) {
            SetTextColor(hdcStatic, isRunning ? RGB(0, 130, 80) : RGB(140, 140, 140));
            SetBkColor(hdcStatic, GetSysColor(COLOR_BTNFACE));
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }
        break;
    }

    case WM_CLOSE: {
        ShowWindow(hwnd, SW_HIDE);
        ShowTrayNotification();
        return 0;
    }

    case WM_SIZE: {
        if (wParam == SIZE_MINIMIZED) {
            ShowWindow(hwnd, SW_HIDE);
            ShowTrayNotification();
            return 0;
        }
        break;
    }

    case WM_TRAYICON: {
        if (lParam == WM_LBUTTONDBLCLK || lParam == WM_LBUTTONUP) {
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
        } 
        else if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, IDM_TRAY_OPEN, L"Open Application");
            AppendMenuW(hMenu, MF_STRING, IDM_TRAY_TOGGLE, isRunning ? L"Disable Auto-switch" : L"Enable Auto-switch");
            AppendMenuW(hMenu, MF_STRING | (IsAutoStartEnabled() ? MF_CHECKED : MF_UNCHECKED), IDM_TRAY_STARTUP, L"Run on Windows Startup");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING, IDM_TRAY_EXIT, L"Exit");

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        break;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_BTN_SAVE) {
            int newLH, newLM, newDH, newDM;
            if (!ParseTimeText(hEditLight, newLH, newLM) || !ParseTimeText(hEditDark, newDH, newDM)) {
                MessageBoxW(hwnd, L"Invalid time format!\nPlease use 24-hour format HH:MM (e.g., 06:00 or 18:30).", L"Format Error", MB_OK | MB_ICONWARNING);
                return 0;
            }

            lightHour = newLH; lightMin = newLM;
            darkHour = newDH;  darkMin = newDM;

            dayWinMode   = GetComboValue(hCmbDayWin);
            dayAppMode   = GetComboValue(hCmbDayApp);
            nightWinMode = GetComboValue(hCmbNitWin);
            nightAppMode = GetComboValue(hCmbNitApp);

            SaveSettings();
            lastAppliedWinMode = -1;
            lastAppliedAppMode = -1;

            UpdateInfoBox();
            CheckAndApplySchedule();
            MessageBoxW(hwnd, L"Custom schedule successfully applied & saved!", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else if (wmId == IDC_CHK_STARTUP) {
            LRESULT state = SendMessage(hChkStartup, BM_GETCHECK, 0, 0);
            SetAutoStart(state == BST_CHECKED);
        }
        else if (wmId == IDM_TRAY_STARTUP) {
            bool currentState = IsAutoStartEnabled();
            SetAutoStart(!currentState);
            SendMessage(hChkStartup, BM_SETCHECK, !currentState ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        else if (wmId == IDC_BTN_TOGGLE || wmId == IDM_TRAY_TOGGLE) {
            ToggleService(hwnd);
        }
        else if (wmId == IDM_TRAY_OPEN) {
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
        }
        else if (wmId == IDM_TRAY_EXIT) {
            DestroyWindow(hwnd);
        }
        break;
    }

    case WM_TIMER: {
        if (wParam == IDT_TIMER) {
            CheckAndApplySchedule();
        }
        break;
    }

    case WM_DESTROY:
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DeleteObject(hFontSegoe);
        DeleteObject(hFontBold);
        DeleteObject(hFontSmall);
        DeleteObject(hFontEdit);
        KillTimer(hwnd, IDT_TIMER);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"ThemeAutoSwitcherClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Auto Theme Switcher",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 535, 435,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    bool startMinimized = (strstr(lpCmdLine, "--startup") != nullptr);
    if (!startMinimized) {
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
