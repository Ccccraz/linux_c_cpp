#ifndef HELLO_STATIC_H
#define HELLO_STATIC_H
#include <iostream>

class HelloStatic {
public:
  HelloStatic() = default;
  auto sayHello() -> void;
  ~HelloStatic() = default;
};

#endif // HELLO_STATIC_H