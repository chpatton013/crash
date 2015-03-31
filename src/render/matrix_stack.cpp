#include <crash/render/matrix_stack.hpp>

using namespace crash::render;

MatrixStack::MatrixStack(const MatrixStack& matrixStack) :
   _stack(matrixStack._stack)
{}

MatrixStack::MatrixStack() {
   this->_stack.push_back(glm::mat4());
}

void MatrixStack::push(const glm::mat4& m) {
   this->_stack.push_back(this->top() * m);
}

void MatrixStack::pop() {
   this->_stack.pop_back();
}

const glm::mat4& MatrixStack::top() const {
   return this->_stack.back();
}
