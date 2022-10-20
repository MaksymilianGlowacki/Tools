# Tools
Useful tools for developing console applications in C++, such as command line parser, CMD runner, etc.
## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Features](#features)
  * [InputParser](#inputparser)
  * [RunCMD](#runcmd)

## General info
This project is a collection of header files. Each header file contains a separate feature and has no other dependencies than C++17 standard library


## Technologies
Project is created with C++ 17


## Setup
To run this project, simple download it and add it to your include directories, e.g. using CMake
```cmake
include_directories(/path/to/folder/with/tools)
```

## Features
### InputParser
#### Usage
Include it in your project using `#include <tools/InputParser.h>`.
The example below shows a simple use of InputParser.
```c++
// TestApp.cpp
#include <tools/InputParser.h>

int main(int argc, char *argv[])
{
    tools::InputParser app(argc, argv);
    app.add_argument("n", "number", "Number to show");
    app.add_option("f", "file", "Redirect output to file 'val'");
    app.add_option("i", "input", "Read input from 'val'");
    app.add_flag("v", "verbose", "Shows more detailed output");
    // Flag "-h, --help, Show this help message" included by default
    std::map<std::string, std::string> params = app.parse();
    
    std::cout << params["n"] << '\n';
    return 0;
}
```
The usage below is equvialent to `TestApp --help` or `TestApp`:
```
$ TestApp -h
Usage: TestApp [flags/options] number 
                n, number               Number to show
Flags/options:
                -i, --input     val     Read input from 'val'
                -f, --file      val     Redirect output to file 'val'
                -h, --help              Show this help message
```
```
$ TestApp 3
3
```
`params` is a map with keys being short names of flags/options/arguments, and values are read from the command line
(empty strings for flags). If number of arguments does not match the defined number of arguments, error is returned.
```
$ TestApp -i input.txt
Too few arguments!
```
### RunCMD