#include <Windows.h>
#include <iostream>

const int DOUBLE_CLICK_THRESHOLD = 250; // Time in milliseconds
const int DOUBLE_CLICK_DISTANCE = 5;    // Distance in pixels

HHOOK mouse_hook = NULL;
POINT last_click_pos;
DWORD last_click_time;

// Handle to the console window, used for logging
HANDLE console_handle;

// Callback function for the mouse hook
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  // Check if the event is a mouse button down event
  if (nCode == HC_ACTION && wParam == WM_LBUTTONDOWN) {
    // Get the current time and mouse position
    DWORD current_time = GetTickCount();
    POINT current_pos;
    GetCursorPos(&current_pos);

    // Check if the event is a double-click
    if (current_time - last_click_time <= DOUBLE_CLICK_THRESHOLD &&
        abs(current_pos.x - last_click_pos.x) <= DOUBLE_CLICK_DISTANCE &&
        abs(current_pos.y - last_click_pos.y) <= DOUBLE_CLICK_DISTANCE) {
      // Log the double-click event
      std::cout << "Double-click prevented" << std::endl;

      // Ignore the event by returning zero
      return 0;
    }

    // Update the last click time and position
    last_click_time = current_time;
    last_click_pos = current_pos;
  }

  // Pass the event to the next hook in the chain
  return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

int main()
{
  // Get the handle to the console window
  console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

  // Set the mouse hook
  mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

  // Loop until the user presses a key
  std::cout << "Press any key to exit..." << std::endl;
  while (!_kbhit()) {
    // Process messages in the queue
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  // Unhook the mouse hook
  UnhookWindowsHookEx(mouse_hook);

  return 0;
}
