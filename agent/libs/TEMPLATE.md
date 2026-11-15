# Library Template — How to Add a New Module

## Directory Structure

```
agent/libs/LibYourName/
├── Makefile          (optional — for standalone builds)
└── src/
    ├── yourname.h    — declarations only (no static functions)
    └── yourname.c    — implementations with DFR declarations
```

## Step 1: Create the Header

Declare your functions in `yourname.h`. These are the public API.

```c
#ifndef YOURNAME_H
#define YOURNAME_H

#include <windows.h>
#include <stddef.h>

int yourname_do_thing(const char *arg, void **out, size_t *out_len);

#endif
```

## Step 2: Implement with DFR

In `yourname.c`, declare every Win32 import using Crystal Palace DFR:

```c
#include <windows.h>
#include "yourname.h"
#include "../../LibDebug/src/debug.h"     // for dbg() calls

// DFR declarations — Crystal Palace resolves these at load time
WINBASEAPI void * WINAPI MSVCRT$malloc(SIZE_T);
WINBASEAPI void   MSVCRT$free(void *);
WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, ...);

int yourname_do_thing(const char *arg, void **out, size_t *out_len) {
    dbg("yourname", "do_thing called");
    // ... implementation using DFR symbols ...
    return 0;
}
```

## Step 3: Register Commands

Add a registration function that maps command names to exec functions:

```c
#include "../../src/agent.h"

static int yourname_exec(AgentConfig *config, const void *args,
                         size_t args_len, AgentOutput *output) {
    // ... call your library functions ...
    // fill output->data, output->data_len, output->type
    return 0;
}

int yourname_register(AgentConfig *config) {
    Module m = {0};
    m.name        = "your-command";
    m.category    = "yourname";
    m.description = "Does the thing";
    m.exec        = yourname_exec;
    m.opsec_safe  = TRUE;
    return module_register(config, &m);
}
```

## Step 4: Wire Into the Build

1. **Makefile** — add compile + zip step:
   ```makefile
   @$(CC_64) -DWIN_X64 $(CFLAGS) -c agent/libs/LibYourName/src/yourname.c -o bin/yourname.x64.o
   ```
   Add to the zip step in the python3 one-liner.

2. **loader.spec** — add mergelib in the PICO block:
   ```
   load "bin/pico.x64.o"
       make object
       mergelib "bin/libdebug.x64.zip"
       mergelib "bin/libtransport.x64.zip"
       mergelib "bin/libyourname.x64.zip"    <-- add here
       export
       link "my_pico"
   ```

3. **pico_combined.c** — add the header include:
   ```c
   #include "../libs/LibYourName/src/yourname.h"
   ```

4. **agent_init()** — call registration:
   ```c
   yourname_register(config);
   ```

## Key Rules

- **No static functions** in library .c files — Crystal Palace mergelib needs visible symbols
- **No CRT** — use DFR for everything (MSVCRT$malloc, MSVCRT$memcpy, etc.)
- **No switch statements** — GCC may generate jump tables → .rdata relocations Crystal Palace can't handle
- **No stack frames >4KB** without `-mno-stack-arg-probe` (avoids ___chkstk_ms)
- **Use if/else chains** instead of switch for enum dispatch
- **DFR format**: `MODULE$Function` (e.g., KERNEL32$VirtualAlloc, WS2_32$send)
- The loader pre-loads the DLL; your code just declares and calls
