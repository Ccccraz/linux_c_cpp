#include "hello_shared.h"

auto HelloShared::sayHello() -> void {
  std::cout << "Hello from shared library!" << std::endl;
}