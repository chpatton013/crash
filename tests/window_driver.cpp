#include <crash/math/math.hpp>
#include <crash/window/keyboard.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/mouse.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

static bool windowQuadrant1Reached = false;
static bool windowQuadrant2Reached = false;
static bool windowQuadrant3Reached = false;
static bool windowQuadrant4Reached = false;
static bool normalButtonClicked = false;
static bool shiftButtonClicked = false;
static bool controlButtonClicked = false;
static bool altButtonClicked = false;
static bool superButtonClicked = false;
static bool mouseQuadrant1Reached = false;
static bool mouseQuadrant2Reached = false;
static bool mouseQuadrant3Reached = false;
static bool mouseQuadrant4Reached = false;
static bool pointerEntered = false;
static bool pointerExited = false;
static float scrollAmount = 0.0f;
static int lastKey = GLFW_KEY_UNKNOWN;
static std::string spelledWord;

static void positionCallback(const Window&, const glm::ivec2& position) {
   boost::optional< Monitor* > monitorOpt = Monitor::primaryMonitor();
   if (!monitorOpt) {
      printf("No primary monitor detected!\n");
      return;
   }

   Monitor* monitor = monitorOpt.get();

   const GLFWvidmode* mode = monitor->mode();
   glm::ivec2 size = glm::ivec2(mode->width, mode->height);
   glm::ivec2 topLeft = glm::ivec2(200, 200);
   glm::ivec2 bottomRight = size - glm::ivec2(200, 200);

   if (position.x < topLeft.x) {
      if (position.y < topLeft.y) {
         // Top left
         windowQuadrant1Reached = true;
      } else if (position.y > bottomRight.y) {
         // Bottom left
         windowQuadrant2Reached = true;
      }
   } else if (position.x > bottomRight.x) {
      if (position.y > bottomRight.y) {
         // Bottom right
         windowQuadrant3Reached = true;
      } else if (position.y < topLeft.y) {
         // Top right
         windowQuadrant4Reached = true;
      }
   }
}

static void closeCallback(Window& window) {
   window.destroy();
}

static void buttonCallback(const Mouse&, int, int action, int mods) {
   if (action != GLFW_RELEASE) {
      return;
   }

   if (mods == 0) {
      normalButtonClicked = true;
   } else if (mods & GLFW_MOD_SHIFT) {
      shiftButtonClicked = true;
   } else if (mods & GLFW_MOD_CONTROL) {
      controlButtonClicked = true;
   } else if (mods & GLFW_MOD_ALT) {
      altButtonClicked = true;
   } else if (mods & GLFW_MOD_SUPER) {
      superButtonClicked = true;
   }
}

static void scrollCalback(const Mouse&, const glm::vec2& scroll) {
   scrollAmount += scroll.y;
}

static void positionCallback(const Mouse& mouse, const glm::vec2& position) {
   const Window& window = mouse.window();
   const glm::ivec2& size = window.windowSize();
   glm::ivec2 center = size / 2;

   if (position.x < center.x) {
      if (position.y < center.y) {
         // Top left
         mouseQuadrant1Reached = true;
      } else {
         // Bottom left
         mouseQuadrant2Reached = true;
      }
   } else {
      if (position.y < center.y) {
         // Bottom right
         mouseQuadrant4Reached = true;
      } else {
         // Top right
         mouseQuadrant3Reached = true;
      }
   }
}

static void enteredCallback(const Mouse&, bool entered) {
   if (entered) {
      pointerEntered = true;
   } else {
      pointerExited = true;
   }
}

static void keyCallback(const Keyboard&, int key, int, int action, int) {
   if (action != GLFW_RELEASE) {
      return;
   }

   lastKey = key;
}

static void characterCallback(const Keyboard&, unsigned int character) {
   if (spelledWord == "") {
      if (character == 'c') {
         spelledWord = "c";
      }
   } else if (spelledWord == "c") {
      if (character == 'r') {
         spelledWord = "cr";
      }
   } else if (spelledWord == "cr") {
      if (character == 'a') {
         spelledWord = "cra";
      }
   } else if (spelledWord == "cra") {
      if (character == 's') {
         spelledWord = "cras";
      }
   } else if (spelledWord == "cras") {
      if (character == 'h') {
         spelledWord = "crash";
      }
   }
}

static void demoMonitors() {
   printf("Detecting attached monitors...\n");

   auto monitorsOpt = Monitor::availableMonitors();
   if (!monitorsOpt) {
      printf("No monitors detected!\n\n");
      return;
   }

   auto monitors = monitorsOpt.get();
   if (monitors.size() == 0) {
      printf("No monitors detected!\n\n");
      return;
   }

   printf("Listing information about all detected monitors:\n");
   for (auto monitor : monitors) {
      printf("   ");
      print(*monitor);

      printf("   Available color modes:\n");
      for (auto mode : monitor->availableModes()) {
         printf("      ");
         print(mode);
      }

      printf("   Current color mode:\n      ");
      print(monitor->mode());

      printf("\n");
   }

   auto primaryMonitorOpt = Monitor::primaryMonitor();
   if (!primaryMonitorOpt) {
      printf("No primary monitor detected!\n\n");
      return;
   }

   auto primaryMonitor = primaryMonitorOpt.get();
   printf("Primary monitor:\n   ");
   print(*primaryMonitor);

   printf("\n");
}

static void demoWindowPosition() {
   Window window = Window(glm::ivec2(400, 300));
   window.setPositionCallback(positionCallback);

   window.visible(true);

   printf("Move the window to the top left quadrant.\n");
   window.title("Window Position: Top Left");
   do {
      windowQuadrant1Reached = windowQuadrant2Reached =
       windowQuadrant3Reached = windowQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!windowQuadrant1Reached);

   printf("Move the window to the bottom left quadrant.\n");
   window.title("Window Position: Bottom Left");
   do {
      windowQuadrant1Reached = windowQuadrant2Reached =
       windowQuadrant3Reached = windowQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!windowQuadrant2Reached);

   printf("Move the window to the bottom right quadrant.\n");
   window.title("Window Position: Bottom Right");
   do {
      windowQuadrant1Reached = windowQuadrant2Reached =
       windowQuadrant3Reached = windowQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!windowQuadrant3Reached);

   printf("Move the window to the top right quadrant.\n");
   window.title("Window Position: Top Right");
   do {
      windowQuadrant1Reached = windowQuadrant2Reached =
       windowQuadrant3Reached = windowQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!windowQuadrant4Reached);
}

static void demoWindowFocus() {
   Window window1 = Window(glm::ivec2(400, 300));
   Window window2 = Window(glm::ivec2(400, 300));

   window1.position(window1.position() - glm::ivec2(100, 100));
   window2.position(window2.position() + glm::ivec2(100, 100));

   window1.visible(true);
   window2.visible(true);

   printf("Focus window 1.\n");
   window1.title("Window Focus: Click on this window");
   window2.title("Window Focus: Click on the other window");
   do {
      glfwWaitEvents();
   } while (!window1.focused());

   printf("Focus window 2.\n");
   window1.title("Window Focus: Click on the other window");
   window2.title("Window Focus: Click on this window");
   do {
      glfwWaitEvents();
   } while (!window2.focused());
}

static void demoWindowMinimize() {
   Window window = Window(glm::ivec2(400, 300));

   window.visible(true);

   printf("Minimize window.\n");
   window.title("Window Minimize");
   do {
      glfwWaitEvents();
   } while (!window.minimized());
}

static void demoWindowClose() {
   printf("Creating three windows with varying sizes.\n");
   printf("Close each as they appear.\n");

   Window window1 = Window(glm::ivec2(400, 300), "Window Close");
   Window window2 = Window(glm::ivec2(800, 600), "Window Close");
   Window window3 = Window(glm::ivec2(800, 450), "Window Close");

   window1.setCloseCallback(closeCallback);
   window2.setCloseCallback(closeCallback);
   window3.setCloseCallback(closeCallback);

   window1.visible(true);
   while (!window1.destroyed()) {
      glfwWaitEvents();
   }

   window2.visible(true);
   while (!window2.destroyed()) {
      glfwWaitEvents();
   }

   window3.visible(true);
   while (!window3.destroyed()) {
      glfwWaitEvents();
   }
}

static void demoWindow() {
   demoWindowPosition();
   demoWindowFocus();
   demoWindowMinimize();
   demoWindowClose();

   printf("\n");
}

static void demoMouseButtons(Window& window) {
   printf("Click the mouse button to continue.\n");
   window.title("Mouse button: Click");
   do {
      normalButtonClicked = shiftButtonClicked = controlButtonClicked = 
       altButtonClicked = superButtonClicked = false;
      glfwWaitEvents();
   } while (!normalButtonClicked);

   printf("Hold shift and click the mouse button to continue.\n");
   window.title("Mouse button: Shift Click");
   do {
      normalButtonClicked = shiftButtonClicked = controlButtonClicked = 
       altButtonClicked = superButtonClicked = false;
      glfwWaitEvents();
   } while (!shiftButtonClicked);

   printf("Hold control and click the mouse button to continue.\n");
   window.title("Mouse button: Control Click");
   do {
      normalButtonClicked = shiftButtonClicked = controlButtonClicked = 
       altButtonClicked = superButtonClicked = false;
      glfwWaitEvents();
   } while (!controlButtonClicked);

   printf("Hold alt and click the mouse button to continue.\n");
   window.title("Mouse button: Alt Click");
   do {
      normalButtonClicked = shiftButtonClicked = controlButtonClicked = 
       altButtonClicked = superButtonClicked = false;
      glfwWaitEvents();
   } while (!altButtonClicked);

   printf("Hold super and click the mouse button to continue.\n");
   window.title("Mouse button: Super Click");
   do {
      normalButtonClicked = shiftButtonClicked = controlButtonClicked = 
       altButtonClicked = superButtonClicked = false;
      glfwWaitEvents();
   } while (!superButtonClicked);
}

static void demoMouseScroll(Window& window) {
   printf("Scroll up.\n");
   window.title("Mouse Scroll: Up");
   do {
      glfwWaitEvents();
   } while (scrollAmount < 1.0f);

   scrollAmount = 0.0f;

   printf("Scroll down.\n");
   window.title("Mouse Scroll: Down");
   do {
      glfwWaitEvents();
   } while (scrollAmount > -1.0f);
}

static void demoMousePosition(Window& window) {
   printf("Move the cursor to the top left quadrant.\n");
   window.title("Mouse Position: Top Left");
   do {
      mouseQuadrant1Reached = mouseQuadrant2Reached =
       mouseQuadrant3Reached = mouseQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!mouseQuadrant1Reached);

   printf("Move the cursor to the bottom left quadrant.\n");
   window.title("Mouse Position: Bottom Left");
   do {
      mouseQuadrant1Reached = mouseQuadrant2Reached =
       mouseQuadrant3Reached = mouseQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!mouseQuadrant2Reached);

   printf("Move the cursor to the bottom right quadrant.\n");
   window.title("Mouse Position: Bottom Right");
   do {
      mouseQuadrant1Reached = mouseQuadrant2Reached =
       mouseQuadrant3Reached = mouseQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!mouseQuadrant3Reached);

   printf("Move the cursor to the top right quadrant.\n");
   window.title("Mouse Position: Top Right");
   do {
      mouseQuadrant1Reached = mouseQuadrant2Reached =
       mouseQuadrant3Reached = mouseQuadrant4Reached = false;
      glfwWaitEvents();
   } while (!mouseQuadrant4Reached);
}

static void demoMouseEnterExit(Window& window) {
   printf("Move the cursor off screen.\n");
   window.title("Mouse Position: Off Screen");
   do {
      pointerEntered = pointerExited = false;
      glfwWaitEvents();
   } while (!pointerExited);

   printf("Move the cursor on screen.\n");
   window.title("Mouse Position: On Screen");
   do {
      pointerEntered = pointerExited = false;
      glfwWaitEvents();
   } while (!pointerEntered);
}

static void demoMouse() {
   printf("Creating a window with mouse handlers.\n");

   Window window = Window(glm::ivec2(400, 300));

   Mouse* mouse = Mouse::factory(window);
   mouse->setButtonCallback(buttonCallback);
   mouse->setPositionCallback(positionCallback);
   mouse->setEnterCallback(enteredCallback);
   mouse->setScrollCallback(scrollCalback);

   window.visible(true);

   demoMouseButtons(window);
   demoMouseScroll(window);
   demoMousePosition(window);
   demoMouseEnterExit(window);

   printf("\n");
}

static void demoKeyboard() {
   printf("Creating a window with keyboard handlers.\n");

   Window window = Window(glm::ivec2(400, 300));

   Keyboard* keyboard = Keyboard::factory(window);
   keyboard->setKeyCallback(keyCallback);
   keyboard->setCharCallback(characterCallback);

   window.visible(true);

   printf("Press the Escape key.\n");
   window.title("Keyboard: Press Escape");
   do {
      lastKey = GLFW_KEY_UNKNOWN;
      glfwWaitEvents();
   } while (lastKey != GLFW_KEY_ESCAPE);

   printf("Press the Up Arrow key.\n");
   window.title("Keyboard: Press Up Arrow");
   do {
      lastKey = GLFW_KEY_UNKNOWN;
      glfwWaitEvents();
   } while (lastKey != GLFW_KEY_UP);

   printf("Press the Down Arrow key.\n");
   window.title("Keyboard: Press Down Arrow");
   do {
      lastKey = GLFW_KEY_UNKNOWN;
      glfwWaitEvents();
   } while (lastKey != GLFW_KEY_DOWN);

   printf("Press the Left Arrow key.\n");
   window.title("Keyboard: Press Left Arrow");
   do {
      lastKey = GLFW_KEY_UNKNOWN;
      glfwWaitEvents();
   } while (lastKey != GLFW_KEY_LEFT);

   printf("Press the Right Arrow key.\n");
   window.title("Keyboard: Press Right Arrow");
   do {
      lastKey = GLFW_KEY_UNKNOWN;
      glfwWaitEvents();
   } while (lastKey != GLFW_KEY_RIGHT);

   printf("Spell the word 'crash'.\n");
   window.title("Keyboard: Spell 'crash'");
   do {
      glfwWaitEvents();
   } while (spelledWord != "crash");

   printf("\n");
}

int main(int, char**) {
   demoMonitors();
   demoWindow();
   demoMouse();
   demoKeyboard();

   return 0;
}
