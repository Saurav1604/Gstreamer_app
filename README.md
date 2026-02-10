# GStreamer Media Player

A basic command-line media player built with GStreamer that supports video and audio playback without a GUI.

## Features

- Play video and audio files
- Display playback position and duration
- Automatic format detection
- Error handling and status messages
- Support for seeking-enabled streams

## Prerequisites

### Windows
Install GStreamer development packages:
1. Download GStreamer from https://gstreamer.freedesktop.org/download/
2. Install both runtime and development installers
3. Add GStreamer bin directory to PATH

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

# Fedora
sudo dnf install gstreamer1-devel gstreamer1-plugins-base-devel
```

### macOS
```bash
brew install gstreamer
```

## Building

### Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Using GCC directly (Linux/macOS)
```bash
gcc main.c -o gst_player `pkg-config --cflags --libs gstreamer-1.0`
```

### Using MSVC (Windows)
```bash
cl main.c /I"C:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0" /I"C:\gstreamer\1.0\msvc_x86_64\include\glib-2.0" /I"C:\gstreamer\1.0\msvc_x86_64\lib\glib-2.0\include" /link gstreamer-1.0.lib glib-2.0.lib gobject-2.0.lib
```

## Usage

```bash
# Using file path
./gst_player /path/to/video.mp4

# Using URI
./gst_player file:///path/to/video.mp4

# HTTP streaming
./gst_player http://example.com/video.mp4
```

## Supported Formats

The player supports any format that GStreamer can decode, including:
- Video: MP4, AVI, MKV, MOV, WebM, FLV, etc.
- Audio: MP3, AAC, FLAC, OGG, WAV, etc.
- Streaming: HTTP, RTSP, etc.

## Controls

- The player will automatically start playback
- Press Ctrl+C to stop playback
- Position and duration are displayed every second during playback

## Troubleshooting

If you encounter errors:
1. Ensure GStreamer is properly installed
2. Check that the media file path is correct
3. Verify the file format is supported
4. Install additional GStreamer plugins if needed:
   - gstreamer1.0-plugins-good
   - gstreamer1.0-plugins-bad
   - gstreamer1.0-plugins-ugly
