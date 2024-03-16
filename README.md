# Toolchain setup

#### Install Homebrew package manager (for MAC)
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### Install OpenOCD for ST-Link Debugging
```
brew install openocd
```

#### Install ARM Embedded Toolchain
```
# not sure about the brew tap, but it works.
brew tap PX4/homebrew-px4
brew update
brew install gcc-arm-none-eabi
arm-none-eabi-gcc --version
```

#### [Install VSCode](https://code.visualstudio.com/)

#### VSCode Extensions
- [C/C++](https://github.com/Microsoft/vscode-cpptools) C/C++ for Visual Studio Code
- [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) ARM Cortex-M GDB Debugger support for VSCode

# Project Setup
Create a new folder for you project and structure it like this:
```
MyProject/
    .vscode/
    app/
        Inc/
            main.h
        Src/
            main.cpp
    Makefile
    README.md
```

#### add the ok-STM32F4 repo as a submodule to your new project
```
git submodule add https://github.com/scottc11/ok-STM32F4.git
git submodule init
git submodule update
```

#### Your new project needs its own make Makefile which includes the ok-STM32F4 Makefile. It should look something like this:
```

######################################
# target
######################################
TARGET = my-project-name

FLASH_SIZE = $$((256 * 1024)) # 256 kB (Sector 6 and 7 used for config data)
RAM_SIZE = $$((128 * 1024)) # 128 kB

######################################
# building variables
######################################
# debug build?
DEBUG = 1

SERIAL_DEBUG ?= 0

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build
LIB_PATH = ok-STM32F4


CPP_SOURCES += $(shell find app -name '*.cpp')

# C includes
C_INCLUDES += \
-Iapp/Inc \
-Iapp/Tasks/Inc

include $(LIB_PATH)/Makefile
```

### Making changes to git submodules
First, `cd` into the submodule directory and checkout a new branch with `git checkout -b myBranchName`

You can now commit changes and push to the remote

## USB Bootloader
No external pull-up resistor is required

Bootloader ID: 0x90

Memory location: 0x1FFF76DE

### Install [dfu-util](http://dfu-util.sourceforge.net/)
```
brew install dfu-util
```

To enter bootloader mode, hold down `BOOT`, then, hold down `RESET`. Release `RESET` button (before releasing `BOOT`)

Run `dfu-util -l` and you should see the following output:
```
dfu-util 0.11

Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
Copyright 2010-2021 Tormod Volden and Stefan Schmidt
This program is Free Software and has ABSOLUTELY NO WARRANTY
Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

Found DFU: [0483:df11] ver=2200, devnum=2, cfg=1, intf=0, path="64-1.2", alt=3, name="@Device Feature/0xFFFF0000/01*004 e", serial="STM32FxSTM32"
Found DFU: [0483:df11] ver=2200, devnum=2, cfg=1, intf=0, path="64-1.2", alt=2, name="@OTP Memory /0x1FFF7800/01*512 e,01*016 e", serial="STM32FxSTM32"
Found DFU: [0483:df11] ver=2200, devnum=2, cfg=1, intf=0, path="64-1.2", alt=1, name="@Option Bytes  /0x1FFFC000/01*016 e", serial="STM32FxSTM32"
Found DFU: [0483:df11] ver=2200, devnum=2, cfg=1, intf=0, path="64-1.2", alt=0, name="@Internal Flash  /0x08000000/04*016Kg,01*064Kg,03*128Kg", serial="STM32FxSTM32"
```

### Using `dfuse-pack.py` (not yet tested)
This file was pulled from the `dfu-util` repo, and is meant to convert `.hex` files into `.dfu` files.
Note: Make sure you have the `IntelHex` python module installed.

### Using a `.bin` file (tested and working)

`dfu-util -a 0 -s 0x08000000:leave -D ./path/to/file.bin`


## GCC Reference:

The `__attribute((unused))` is a gcc attribute to indicate to the compiler that the argument args is unused, and it prevents warnings about it.

### Preventing compiler optimazation for debugging
Use this `__attribute__((optimize("O0")))` to disable optimizations for a function.
ex.
```
void __attribute__((optimize("O0"))) myFunc()
{
    // this will not be optimized
}
```
