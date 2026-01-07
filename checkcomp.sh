#!/bin/bash

# Script to diagnose slow compilation

echo "=== Compilation Diagnostic Tool ==="
echo ""

# Check if temp files exist
if [ -f "src/main.mg.cpp" ]; then
    echo "✓ Found generated C++ file: src/main.mg.cpp"
    lines=$(wc -l < src/main.mg.cpp)
    size=$(du -h src/main.mg.cpp | cut -f1)
    echo "  Lines: $lines"
    echo "  Size: $size"
    echo ""
fi

# Check for SlateDB generated file
if [ -f "src/Slate/DB.mg.cpp" ]; then
    echo "✓ Found generated C++ file: src/Slate/DB.mg.cpp"
    lines=$(wc -l < src/Slate/DB.mg.cpp)
    size=$(du -h src/Slate/DB.mg.cpp | cut -f1)
    echo "  Lines: $lines"
    echo "  Size: $size"
    echo ""
fi

# Check running g++ processes
echo "=== Checking for active g++ processes ==="
ps aux | grep g++ | grep -v grep

echo ""
echo "=== Recommendations ==="
echo ""

# If file is very large
if [ -f "src/main.mg.cpp" ]; then
    lines=$(wc -l < src/main.mg.cpp)
    if [ $lines -gt 10000 ]; then
        echo "⚠ Generated C++ file is very large ($lines lines)"
        echo "  This can cause slow compilation. Consider:"
        echo "  1. Using --debug flag (disables heavy optimization)"
        echo "  2. Splitting code into multiple files"
        echo "  3. Reducing template usage"
    fi
fi

echo ""
echo "To see what's happening, run:"
echo "  magolor build --verbose --emit-cpp --debug"
echo ""
echo "To force stop compilation:"
echo "  pkill -9 g++"
