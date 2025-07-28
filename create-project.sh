#!/bin/bash

# STM32F4 Project Generator Script
# This script creates a complete project structure based on the ok-STM32F4 library

set -e

# Default project name
PROJECT_NAME="my-stm32-project"

# Check if project name is provided
if [ "$#" -eq 1 ]; then
    PROJECT_NAME="$1"
fi

echo "Creating STM32F4 project: $PROJECT_NAME"
echo "=================================="

# Create main directory structure
echo "Creating directory structure..."
mkdir -p "$PROJECT_NAME"
cd "$PROJECT_NAME"

# Create firmware directories
mkdir -p firmware/Inc
mkdir -p firmware/Src

# Create .vscode directory
mkdir -p .vscode

echo "Generating template files..."

# Create main.h
cat > firmware/Inc/main.h << 'EOF'
#pragma once

#include "cmsis_os.h"
#include "common.h"

#ifndef FIRMWARE_VERSION // this will be passed in as a flag by compiler
#define FIRMWARE_VERSION "default"
#endif

#define PPQN 24
EOF

# Create main.cpp
cat > firmware/Src/main.cpp << 'EOF'
#include "main.h"
#include "AnalogIn.h"
#include "InterruptIn.h"
#include "DigitalOut.h"

TaskHandle_t th_main;
DigitalOut led(PD_2);

void taskMain(void *pvParameters)
{
    led.write(1);
    while (1)
    {
        vTaskDelay(100);
        led.toggle();
    }
}

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    HAL_Delay(5);
    // ADC sample rate should be at least 2x the speed of the multiplexer switching rate
    // AnalogIn::initialize(20000); // 20Khz ADC sample rate (@ 100KHz, there is a bug causing the gpio expander interrupt BUTTONS_INT to fail... not sure why)
    HAL_Delay(5);
    // InterruptIn::initialize();
    DigitalOut::initialize();
    HAL_Delay(90);

    xTaskCreate(taskMain, "taskMain", 512, NULL, 1, &th_main);

    vTaskStartScheduler();

    while (1)
    {
    }
}
EOF

# Create Makefile
cat > Makefile << EOF

######################################
# target
######################################
TARGET = $PROJECT_NAME

FLASH_SIZE = \$\$((256 * 1024)) # 256 kB (Sector 6 and 7 used for config data)
RAM_SIZE = \$\$((128 * 1024)) # 128 kB

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


CPP_SOURCES += \$(shell find firmware -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/MPR121 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/DAC8554 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/IS31FL3246 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/SX1509 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/MCP23017 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/M24256 -name '*.cpp')
CPP_SOURCES += \$(shell find \$(LIB_PATH)/drivers/CD4051 -name '*.cpp')


# C includes
C_INCLUDES += \\
-Ifirmware/Inc \\
-I\$(LIB_PATH)/drivers/MPR121 \\
-I\$(LIB_PATH)/drivers/DAC8554 \\
-I\$(LIB_PATH)/drivers/SX1509 \\
-I\$(LIB_PATH)/drivers/MCP23017 \\
-I\$(LIB_PATH)/drivers/IS31FL3246 \\
-I\$(LIB_PATH)/drivers/M24256 \\
-I\$(LIB_PATH)/drivers/CD4051

include \$(LIB_PATH)/Makefile
EOF

# Create .gitignore
cat > .gitignore << 'EOF'
build/

.DS_Store
EOF

# Create VS Code settings.json
cat > .vscode/settings.json << 'EOF'
{
    "files.associations": {
        "*.json.liquid": "json",
        "*.yaml.liquid": "yaml",
        "*.md.liquid": "markdown",
        "*.js.liquid": "liquid-javascript",
        "*.css.liquid": "liquid-css",
        "*.scss.liquid": "liquid-scss",
        "istream": "cpp",
        "memory": "cpp",
        "algorithm": "cpp",
        "bitset": "cpp",
        "optional": "cpp",
        "*.tcc": "cpp",
        "random": "cpp",
        "cmath": "cpp",
        "array": "cpp"
    },
    "C_Cpp.errorSquiggles": "disabled",
    "cortex-debug.variableUseNaturalFormat": true
}
EOF

# Create VS Code tasks.json
cat > .vscode/tasks.json << 'EOF'
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Upload",
            "type": "shell",
            "command": "make program",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "make clean",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Build and Upload",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "dependsOn": [
                "Build",
                "Upload"
            ],
            "dependsOrder": "sequence",
            "problemMatcher": []
        }
    ]
}
EOF

# Create VS Code launch.json
cat > .vscode/launch.json << 'EOF'
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex Debug",
            "cwd": "${workspaceRoot}",
            "executable": "build/firmware.elf",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32f4x.cfg",
            ],
            "openOCDLaunchCommands": [
                "init",
                "reset init"
            ],
            "svdFile": "./.vscode/STM32F446.svd",
            "preLaunchTask": "Build"
        }
    ]
}
EOF

# Create VS Code c_cpp_properties.json
cat > .vscode/c_cpp_properties.json << 'EOF'
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/firmware/**",
                "${workspaceFolder}/ok-STM32F4/CMSIS/Device/ST/STM32F4xx/Include/**",
                "${workspaceFolder}/ok-STM32F4/CMSIS/Device/ST/STM32F4xx/Source/**",
                "${workspaceFolder}/ok-STM32F4/HAL/Inc/*",
                "${workspaceFolder}/ok-STM32F4/HAL/Src/*",
                "${workspaceFolder}/ok-STM32F4/HAL/rtos/*",
                "${workspaceFolder}/ok-STM32F4/HAL/rtos/Inc/*",
                "${workspaceFolder}/ok-STM32F4/cxxsupport/*",
                "${workspaceFolder}/ok-STM32F4/middleware/**",
                "${workspaceFolder}/ok-STM32F4/STM32F4xx_HAL_Driver/**",
                "${workspaceFolder}/ok-STM32F4/STM32F4xx_HAL_Driver/Inc/**",
                "${workspaceFolder}/ok-STM32F4/STM32F4xx_HAL_Driver/Src/**",
                "${workspaceFolder}/ok-drivers/**",
                "${workspaceFolder}/ok-STM32F4/System/*",
                "${workspaceFolder}/ok-STM32F4/System/Inc/*",
                "${workspaceFolder}/ok-STM32F4/System/Src/*"
            ],
            "defines": [
                "USE_HAL_DRIVER",
                "STM32F446xx"
            ],
            "macFrameworkPath": [
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/opt/homebrew/bin/arm-none-eabi-gcc",
            "cStandard": "c11",
            "cppStandard": "c++11",
            "intelliSenseMode": "macos-gcc-arm",
            "configurationProvider": "ms-vscode.makefile-tools"
        }
    ],
    "version": 4
}
EOF

# Create README.md
cat > README.md << EOF
# $PROJECT_NAME

STM32F4 project created using the ok-STM32F4 library.

## Getting Started

### Prerequisites

- ARM GCC Toolchain
- OpenOCD
- VS Code with Cortex-Debug extension

### Building

\`\`\`bash
make
\`\`\`

### Uploading

\`\`\`bash
make program
\`\`\`

### Debugging

Open the project in VS Code and press F5 to start debugging.

## Project Structure

- \`firmware/\` - Application source code
  - \`Inc/\` - Header files
  - \`Src/\` - Source files
- \`ok-STM32F4/\` - STM32F4 library (git submodule)
- \`.vscode/\` - VS Code configuration files
- \`Makefile\` - Build configuration

## Adding the ok-STM32F4 Submodule

If you haven't already added the ok-STM32F4 library as a submodule:

\`\`\`bash
git submodule add https://github.com/scottc11/ok-STM32F4.git
git submodule init
git submodule update
\`\`\`
EOF

echo "Copying STM32F446.svd file..."
# Copy the SVD file from the ok-STM32F4 root directory
if [ -f "../STM32F446.svd" ]; then
    cp "../STM32F446.svd" .vscode/
else
    echo "Warning: STM32F446.svd file not found in ok-STM32F4 root directory"
fi

echo ""
echo "✅ Project '$PROJECT_NAME' created successfully!"
echo ""
echo "Next steps:"
echo "1. cd $PROJECT_NAME"
echo "2. git init"
echo "3. git submodule add https://github.com/scottc11/ok-STM32F4.git"
echo "4. git submodule init && git submodule update"
echo "5. make"
echo ""
echo "Happy coding! 🚀" 