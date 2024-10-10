# 概述

在现代 c/cpp 开发中, 一般会选择 cmake 作为项目的构建系统. 在这个例子中我会使用一个简单的基于 cmake 的项目介绍你可能会遇到的 shared library, static library, executable 项目之间的区别和相关问题的解决方法.

# 项目结构

一个常见的 c/cpp 项目通常由以下结构组成

```plain
example_project
├── bin
├── build
├── CMakeLists.txt
├── include
│  ├── Foo.h
│  └── Bar.h
├── lib
├── README.md
└── src
   ├── main.cc
   ├── Foo
   │  ├── Foo.cc
   └── Bar
      └── Bar.cc
```

我们需要从一个 `CMakeLists.txt` 文件开始构建我们的项目, 在这个文件的开头是一些关于你的项目的设置信息:

```cmake
# 设置 cmake 的最小需求
cmake_minimum_required(VERSION 3.10)

# 生成 clangd 配置文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 创建项目并设置项目版本
project(LinuxCpp VERSION 1.0)

# 根据谷歌 cpp 规范, 新的 cpp 项目应该至少面向 cpp20 构建
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

在 c/cpp 的世界中, 你的编译产物可以大致分为三种:
- executable 也就是可执行的二进制文件, 也是我们所最为熟悉的一种开发体验
  > 在 windows 环境下, 可执行文件通常以 `.exe` 结尾, 而在 linux 环境下, 一个可执行文件通常是一个没有后缀的文件
- shared library 也就是动态链接库, 这是大多数时候导致新手的程序不能运行的原因. 动态链接库为我们提供了一种在不直接将依赖的代码打包进入我们自己的 executable binary 的情况下, 也可以使用第三方代码的方法
  > 在 windows 环境下, 动态链接库通常以 `.dll` 结尾, 在 linux 环境下, 一个 shared lib 通常一个 `.so` 作为后缀名
  > 由于一些历史原因, linux 和 windows 对动态链接库的管理方式完全不同, windows 环境下我们常见的是一个软件项目往往包含了它所需要的所以依赖, 但是 linux 倾向于将所有的动态链接库统一管理并鼓励开发者通过链接到动态库来使用第三方函数而不是将所有的依赖都打包进自己的项目中.
- static library 静态链接库, 它一般不会导致什么问题

在 `cmake` 你可以这样创建 shared lib, static lib 和 executable:

```cmake
# 创建一个动态链接库
add_library(SayHelloShared SHARED src/shared/hello_shared.cc)
target_include_directories(SayHelloShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 创建一个静态链接库
add_library(SayHelloStatic STATIC src/static/hello_static.cc)
target_include_directories(SayHelloStatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 创建一个可执行文件
add_executable(SayHello src/main.cc)
target_include_directories(SayHello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 将动态链接库和静态链接库链接到可执行文件中
target_link_libraries(SayHello SayHelloShared SayHelloStatic)
```

## 项目实现

我们在 `hello_shared.cc`, `hello_static.cc` 中分别定义了一个 `SayHello` 方法, 并在 `main.cc` 中调用了它们:

```cpp
// hello_shared.cc
#include "hello_shared.h"

auto HelloShared::sayHello() -> void {
  std::cout << "Hello from shared library!" << std::endl;
}
```

```cpp
// hello_static.cc
#include "hello_static.h"

auto HelloStatic::sayHello() -> void {
  std::cout << "Hello from static library!" << std::endl;
}
```

```cpp
// main.cc
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
```

在如上的代码中, 我们可以清晰的看到, 我们在 `main.cc` 中分别调用了 `hello_shared.cc` 和 `hello_static.cc` 中所定义的 `sayHello` 方法

## 编译项目并运行

现在让我们来编译这段代码, 看一看执行的效果:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

在编译后, 你的 build 文件夹下的内容应该如下:

```plain
build
├── cmake_install.cmake
├── CMakeCache.txt
├── CMakeFiles
├── compile_commands.json
├── libSayHelloShared.so
├── libSayHelloStatic.a
├── Makefile
└── SayHello
```

其中 `SayHello` 就是我们最终所需要的 executable 文件, 而 `libSayHelloShared.so` 和 `libSayHellStatic.a` 分别是 `SayHello` 依赖的动态链接库和静态链接库

最后让我们执行我们刚刚构建的 `SayHello`

```bash
./SayHello
```

你将看到以下输出:

```plain
Hello from shared library!
Hello from static library!
Hello, World from executable!
```

## 动态链接库的使用

现在让我们做一个实验:

### Step 1
将静态链接库转移出项目文件夹, 然后执行 `SayHello`
```bash
mv libSayHelloStatic.a ~
./SayHello
```

你将看到这样的结果:
```plain
Hello from shared library!
Hello from static library!
Hello, World from executable!
```
说明静态链接库是否存在对 `SayHello` 的运行没有影响, 这是因为静态链接库的函数定义已经被包含在了 executable 文件的 binary 中.

### Step 1
将动态链接库转移出项目文件夹, 然后执行 `SayHello`
```bash
mkdir ~/lib
mv libSayHelloShared.so ~/lib
./SayHello
```

你会看到如下输出:

```plain
./SayHello: error while loading shared libraries: libSayHelloShared.so: cannot open shared object file: No such file or directory
```

我们的 `SayHello` 文件在执行时无法找到所依赖的动态链接库, 所有无法执行. 这是为什呢? 让我们回到 `main.cc` 文件的定义中:

```cpp
// main.cc
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
```

可以看到, 我们使用了定义在 `hello_shared.h` 的方法, 而 hello_shared 的实现实际上包含在 `libSayHelloShared.so` 这个动态库中, 而由于我们刚刚将 `libSayHelloShared.so` 移动到了 `~/lib` 中, `SayHello` 自然就无法找到 `shared_lib.sayHello();` 的定义, 这就会导致上面的错误

而要解决这个问题, 我们可以将我们的动态库添加到我们的库文件搜索路径中:

```bash
export LD_LIBRARY_PATH=~/lib:$LD_LIBRARY_PATH
./SayHello
```

此时我们可发现, `SayHello` 的输出回复了正常:

```plain
Hello from shared library!
Hello from static library!
Hello, World from executable!
```

这是因为, 当在程序默认的路径下找不到所需要的动态链接库时, 系统首先会在用户定义的搜索路径下寻找缺失的动态库, 我们可以将当前的 LD_LIBRARY_PATH 打印出来观察:

```bash
echo $LD_LIBRARY_PATH
```

我们可以看到输出:

```bash
/home/ccccr/lib:
```

正是我们刚刚所手动添加的搜索路径, 至此关于 linux 下动态链接库的旅程便可以暂时告一段落了

> 作为参考, Linux下搜索路径的次序：
    1. ELF可执行文件中动态段中DT_RPATH所指定的路径，不常用但是比较实用的方法；
    2. 编译目标代码时指定的动态库搜索路径（-WI,-rpath=./）；
    3. 环境变量LD_LIBRARY_PATH指定的动态库搜索路径；
    4. 配置文件/etc/ld.so.conf（或ld.so.cache）中指定的动态库搜索路径；
    5. 默认的动态库搜索路径/lib；
    6. 默认的动态库搜索路径/usr/lib。