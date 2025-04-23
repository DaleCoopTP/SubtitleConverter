# Subtitle Converter

A C++ application for converting and processing subtitles in various formats: SRT, SAMI, SSA/ASS, and a custom format.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Architecture](#architecture)
  - [Class Diagram](#class-diagram)
  - [Design Patterns](#design-patterns)
- [Build](#build)
- [Examples](#examples)
- [Testing](#testing)



## Features

- Convert subtitles between supported formats (SRT, SAMI, SSA/ASS, TTML).
- Add or remove style tags (for SAMI and SSA/ASS).
- Shift subtitle timestamps by arbitrary offsets.
- Detect and report time-based collisions between subtitle entries.

## Requirements

- C++ compiler with C++11 support or later (e.g., GCC 5.0+, Clang 3.4+, MSVC 2015+).
- CMake 3.5 or newer.
- No external libraries required; all data structures (except `std::string`) are implemented manually.

## Architecture

The system is built around a base `Subtitle` class, with each format implemented as a derived class. A factory pattern handles format detection and instantiation. Operations like conversion, timestamp shifting, and collision detection are provided via polymorphism.

### Class Diagram

![Class Diagram](docs/class_diagram.png)

### Design Patterns

- **Factory**: Creates the appropriate `Subtitle` subclass based on file extension or content.  
- **Strategy**: Applies different processing strategies (e.g., timestamp shifting, tag removal) to subtitle entries.  
- **Template Method**: Defines the skeleton of subtitle parsing and formatting algorithms in the base class.

## Build

```bash
mkdir build
cd build
cmake ..
make
```


**Options:**

- `--shift <+|->HH:MM:SS.mmm`  Shift all timestamps by the specified offset.
- `--remove-tags`             Remove all style tags from output.
- `--help`                    Display help message with all options.

**Example:**

```bash
./subtitle_converter --shift +00:00:02.500 input.srt output.ass
```

## Examples

**Input (SRT):**
```srt
1
00:00:01,000 --> 00:00:04,000
Hello, world!
```

**Output (ASS):**
```ass
[Script Info]
Title: Converted

[Events]
Dialogue: 0,0:00:03.500,0:00:06.500,Default,,0,0,0,,Hello, world!
```

## Testing

Unit tests cover key components and edge cases, with a focus on format-specific parsing and formatting behaviors.

To run tests:

```bash
cd build
make test
```

