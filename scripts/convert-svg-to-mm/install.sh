#!/bin/bash

# Install script for convert_svg_to_mm.js
# This script installs the SVG converter to make it available system-wide

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_SCRIPT="$SCRIPT_DIR/convert_svg_to_mm.js"
TARGET_NAME="convert-svg-to-mm"

# Check if source script exists
if [ ! -f "$SOURCE_SCRIPT" ]; then
    echo "Error: convert_svg_to_mm.js not found in $SCRIPT_DIR"
    exit 1
fi

# Determine installation directory
if [ -d "$HOME/.local/bin" ]; then
    INSTALL_DIR="$HOME/.local/bin"
elif [ -d "$HOME/bin" ]; then
    INSTALL_DIR="$HOME/bin"
else
    echo "Creating $HOME/.local/bin directory..."
    mkdir -p "$HOME/.local/bin"
    INSTALL_DIR="$HOME/.local/bin"
fi

TARGET_PATH="$INSTALL_DIR/$TARGET_NAME"

echo "Installing SVG converter..."
echo "Source: $SOURCE_SCRIPT"
echo "Target: $TARGET_PATH"

# Copy the script
cp "$SOURCE_SCRIPT" "$TARGET_PATH"

# Make it executable
chmod +x "$TARGET_PATH"

echo "✓ Script installed successfully!"
echo ""
echo "Usage:"
echo "  $TARGET_NAME                          # Process all SVG files in current directory"
echo "  $TARGET_NAME file1.svg file2.svg     # Process specific files"
echo "  $TARGET_NAME folder1/ folder2/       # Process all SVG files in specified folders"
echo "  $TARGET_NAME file.svg folder/        # Mix files and folders"
echo ""

# Check if the install directory is in PATH
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "⚠️  Warning: $INSTALL_DIR is not in your PATH"
    echo "   Add this line to your ~/.zshrc or ~/.bash_profile:"
    echo "   export PATH=\"\$PATH:$INSTALL_DIR\""
    echo ""
    echo "   Then run: source ~/.zshrc"
    echo ""
    echo "   Or you can run the script directly: $TARGET_PATH"
else
    echo "✓ $INSTALL_DIR is already in your PATH"
    echo "✓ You can now run: $TARGET_NAME"
fi