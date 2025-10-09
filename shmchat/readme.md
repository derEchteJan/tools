### ShmChat

How to use:

in c++ application:
```
#define PLATFORM_LINUX // or PLATFORM_WINDOWS
#include "<path-to>/ShmChat.h"
#include "<path-to>/ShmChat.cpp"

auto shmc = shmc_open("my-id", true);

shmc_println(shmc, "Hello World!");
```

in the command line:

```
shmchat read "my-id"
```