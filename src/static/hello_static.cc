#include "hello_static.h"

auto HelloStatic::sayHello() -> void {
  std::cout << "Hello from static library!" << std::endl;
}