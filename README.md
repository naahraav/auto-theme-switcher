# Auto Theme Switcher

Automatically switch between Light and Dark themes on Windows according to your custom schedule.

Auto Theme Switcher is a lightweight Windows desktop application written in C++ that automates system and application theme changes based on the time of day. It runs in the background, remembers your preferences, and lets you customize how Windows and applications look during the day and at night.

## Screenshot

Here's a preview of Auto Theme Switcher in action:

![Auto Theme Switcher application screenshot](assets/auto-theme-switcher.png)

## Features

- **Automatic Theme Switching** — Automatically applies your selected themes according to your schedule.
- **Customizable Schedule** — Set separate start times for your day and night schedules using the 24-hour format (`HH:MM`).
- **Independent Theme Settings** — Configure Windows system and application themes independently for both schedules.
- **System Tray Integration** — Minimize the application to the system tray and manage it without keeping the main window open.
- **Windows Startup Support** — Optionally launch the application when Windows starts, minimized to the system tray.
- **Persistent Configuration** — Saves your schedule, theme preferences, and running state in the Windows Registry.
- **Quick Enable/Disable** — Pause or resume automatic theme switching whenever you want.
- **Lightweight and Native** — Built with C++ and the Windows API without requiring a separate runtime framework.

## How It Works

Auto Theme Switcher checks the current local time every five seconds and determines which schedule should be active.

For example:

| Schedule | Start Time | Windows Theme | App Theme |
|---|---|---|---|
| Day | 06:00 | Light | Light |
| Night | 18:00 | Dark | Dark |

These times and theme preferences are fully customizable. You can also mix and match themes, such as using Light Mode for Windows while keeping applications in Dark Mode.

## Getting Started

### Option 1: Run the Executable

1. Download `ThemeSwitcher.exe` from the repository's **Releases** page, if a release is available.
2. Run `ThemeSwitcher.exe` on your Windows PC.
3. Set your preferred day and night start times.
4. Choose the Windows and application themes for each schedule.
5. Click **Save Schedule** to apply and save your settings.
6. Enable **Run on Windows Startup** if you want the application to launch automatically when you sign in to Windows.

### Option 2: Build from Source

**Requirements**
- Windows operating system
- A C++ compiler with Windows SDK support, such as Microsoft Visual C++ (MSVC)
- Windows SDK and common controls libraries

**Steps**

1. Clone the repository:

   ```bash
   git clone https://github.com/YOUR-USERNAME/YOUR-REPOSITORY.git
   ```

2. Navigate to the project directory:

   ```bash
   cd YOUR-REPOSITORY
   ```

3. Compile `main.cpp` as a Windows GUI application using MSVC:

   ```bat
   cl /EHsc /std:c++17 /DUNICODE /D_UNICODE main.cpp /link /SUBSYSTEM:WINDOWS
   ```

4. Run the generated `main.exe`.

*Note: Run the compilation command from a Visual Studio Developer Command Prompt with the required Windows SDK installed.*

## Using the System Tray

When you close or minimize the main window, the application continues running in the background.

Right-click the system tray icon to access these options:

- **Open Application** — Reopen the main window.
- **Disable Auto-switch / Enable Auto-switch** — Pause or resume automatic theme switching.
- **Run on Windows Startup** — Toggle automatic startup.
- **Exit** — Completely close the application.

## Configuration

Your settings are stored in the following Windows Registry key:

```text
HKEY_CURRENT_USER\Software\AutoThemeSwitcher
```

The application stores schedule times, day and night theme preferences, and the automatic switching state. Startup preferences are managed separately through the Windows startup registry key.

## Technology Stack

- **Language:** C++
- **Platform:** Microsoft Windows
- **API:** Windows API (Win32)
- **UI:** Native Windows controls
- **Configuration:** Windows Registry
- **Background Integration:** Windows System Tray and Timer API

## Project Goals

The goal of Auto Theme Switcher is to make desktop personalization more convenient by automating theme changes throughout the day. Instead of manually switching between Light and Dark Mode, users can define their preferred schedule once and let the application handle the rest.

## Contributing

Suggestions, bug reports, and improvements are welcome. Feel free to fork the repository and submit a pull request.

## License

No license has been specified yet. Please check the repository for licensing information before redistributing or modifying this project.

