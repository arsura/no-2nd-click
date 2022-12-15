#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

const int DOUBLE_CLICK_THRESHOLD = 250; // Time in milliseconds
const int DOUBLE_CLICK_DISTANCE = 5;    // Distance in pixels

Display* display = NULL;
int screen = 0;
Window root_window = 0;

// Callback function for the mouse hook
Bool MouseProc(Display* d, XEvent* e, XPointer p)
{
  // Check if the event is a mouse button press event
  if (e->type == ButtonPress) {
    // Get the current time and mouse position
    Time current_time = e->xbutton.time;
    int current_x = e->xbutton.x_root;
    int current_y = e->xbutton.y_root;

    // Check if the event is a double-click
    static Time last_click_time = 0;
    static int last_click_x = 0;
    static int last_click_y = 0;
    if (current_time - last_click_time <= DOUBLE_CLICK_THRESHOLD &&
        abs(current_x - last_click_x) <= DOUBLE_CLICK_DISTANCE &&
        abs(current_y - last_click_y) <= DOUBLE_CLICK_DISTANCE) {
      // Log the double-click event
      std::cout << "Double-click prevented" << std::endl;

      // Ignore the event by returning False
      return False;
    }

    // Update the last click time and position
    last_click_time = current_time;
    last_click_x = current_x;
    last_click_y = current_y;
  }

  // Pass the event to the next handler in the chain
  return True;
}

int main()
{
  // Open a connection to the X server
  display = XOpenDisplay(NULL);
  screen = DefaultScreen(display);
  root_window = RootWindow(display, screen);

  // Set the mouse hook
  XSelectInput(display, root_window, ButtonPressMask);
  XSync(display, False);
  XEvent event;
  while (true) {
    // Wait for the next event
    XNextEvent(display, &event);

    // Process the event
    if (!MouseProc(display, &event, NULL)) {
      // Ignore the event if MouseProc returns False
      continue;
    }

    // Pass the event to the default handler
    XPutBackEvent(display, &event);
  }

  // Close the connection to the X server
  XCloseDisplay(display);

  return 0;
}
