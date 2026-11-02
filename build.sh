#!/bin/bash
echo "[Linux] Pulling latest changes from Git..."
git pull origin main
echo "[Linux] Configuring CMake..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
echo "[Linux] Building project..."
cmake --build build