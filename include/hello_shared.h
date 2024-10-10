#ifndef HELLO_SHARED_H
#define HELLO_SHARED_H
#include <iostream>

class HelloShared {
public:
  HelloShared() = default;
  auto sayHello() -> void;
};

#endif // HELLO_SHARED_H