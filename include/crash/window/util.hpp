#pragma once

struct GLFWgammaramp;
struct GLFWvidmode;

namespace crash {
namespace window {

class Monitor;

void print(const Monitor& monitor);

void print(const Monitor& monitor, bool newline);

void print(const GLFWvidmode* mode);

void print(const GLFWvidmode* mode, bool newline);

void print(const GLFWgammaramp* gamma);

void print(const GLFWgammaramp* gamma, bool newline);

} // namespace window
} // namespace crash
