#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace crash {
namespace render {

class MatrixStack {
public:
   MatrixStack(const MatrixStack& matrixStack);
   MatrixStack();

   void push(const glm::mat4& m);
   void pop();
   const glm::mat4& top() const;

private:
   std::vector< glm::mat4 > _stack;
};

} // namespace render
} // namespace crash
