#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include <crash/window/monitor.hpp>
#include <crash/window/util.hpp>

using namespace crash::window;

void crash::window::print(const Monitor& monitor) {
   print(monitor, true);
}

void crash::window::print(const Monitor& monitor, bool newline) {
   const char* formatString;
   if (newline) {
      formatString = "Monitor; name(%s); position(%d,%d); size(%d,%d)\n";
   } else {
      formatString = "Monitor; name(%s); position(%d,%d); size(%d,%d)";
   }
   printf(formatString, monitor.getName().data(),
    monitor.getPosition().x, monitor.getPosition().y,
    monitor.getSize().x, monitor.getSize().y);
}

void crash::window::print(const GLFWvidmode* mode) {
   print(mode, true);
}

void crash::window::print(const GLFWvidmode* mode, bool newline) {
   const char* formatString;
   if (newline) {
      formatString = "GLFWvidmode; size(%d,%d); color(%d,%d,%d); refresh(%d)\n";
   } else {
      formatString = "GLFWvidmode; size(%d,%d); color(%d,%d,%d); refresh(%d)";
   }
   printf(formatString, mode->width, mode->height,
    mode->redBits, mode->greenBits, mode->blueBits,
    mode->refreshRate);
}

void crash::window::print(const GLFWgammaramp* gamma) {
   print(gamma, true);
}

void crash::window::print(const GLFWgammaramp* gamma, bool newline) {
   std::cout << "GLFWgammaramp; size(" << gamma->size << ");";

   if (gamma->size > 0) {
      std::cout << " red(" << gamma->red[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->red[ndx];
         }
      }
      std::cout << ");";

      std::cout << " green(" << gamma->green[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->green[ndx];
         }
      }
      std::cout << ");";

      std::cout << " blue(" << gamma->blue[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->blue[ndx];
         }
      }
      std::cout << ");";
   }

   if (newline) {
      std::cout << std::endl;
   }
}
