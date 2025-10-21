#!/bin/bash
set -e
cd "$(dirname "$0")/.."
mkdir -p bin

echo "Building Replay-UDP-Packet (Ubuntu)..."
g++ -std=c++17 -O2 -Wall -I. \
  mod-config/*.cpp mod-interface/*.cpp mod-ose/*.cpp mod-ose/tinyXML/*.cpp *.cpp \
  -lpthread -o bin/Replay-UDP-Packet

echo "Build succeeded: bin/Replay-UDP-Packet"
