#include <iostream>
#include "hello_static.h"
#include "hello_shared.h"

int main() {
  HelloShared shared_lib{};
  HelloStatic static_lib{};

  shared_lib.sayHello();
  static_lib.sayHello();

  std::cout << "Hello, World from executable!" << std::endl;
  return 0;
}