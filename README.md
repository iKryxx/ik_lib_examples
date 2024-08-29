
# ik_lib.h

A library that eases your C-style C++ experience.




## Authors

- [@iKryxx](https://www.github.com/iKryxx)
- [@pkurras](https://github.com/pkurras)


## Installation

In your projects directory, move into a directory that is being included by your project files:
```bash
  (Windows) chdir <dir>
  (Linux) cd <dir>
```

Install ik_lib with git:
```bash
  git clone https://github.com/iKryxx/ik_lib
```

Move ik_lib into your folder, delete installation directory:
```bash
  (Windows) move ik_lib/utils/* ./
            deltree ik_lib/
  (Linux) mv ik_lib/utils/* ./
          rm -rf ik_lib/
```

    
## Usage

Including the library
```cpp
#include "ik_lib.h"

int main(int argc, char** argv) {
    i32 ret = 1;
    return ret;
}
```


## Documentation

[Documentation](https://ik-lib.rtfd.io)
