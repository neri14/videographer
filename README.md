# Videographer

[![Build and Test](https://github.com/neri14/videographer/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/neri14/videographer/actions/workflows/build-and-test.yml)


## Building and testing

1. ```cmake -B build``` generate cmake files to build directory
1. ```cmake --build build --target vgraph``` build main binary
1. ```cmake --build build --target vgraph_test``` build unit tests
1. ```build/vgraph_test``` run unit tests


## Pipeline

1. Get video and telemetry
1. Run concat tool to combine the clips
1. Run trim tool to create final raw video
1. Run alignment tool and figure out offset
1. Run generator app to generate final video (for YT - set upscale to 4K (```-r 3840x2160```))
1. Upload


## Dependencies

Packages required in system to build the application

- CMake >= 3.23
- Make / Ninja
- GCC >= 14.x
- googlemock
- gstreamer >= 1.24
- cairo
- pango
- pugixml


## Alignment Tool

Python alignment tool is located in tools/alignment.

Basic usage based on speed visible on video:

1. Create and enter virtualenv
1. Install requirements from requirements.txt
1. determine rough clip ```START``` and ```DURATION``` that will havevisible changes in speed on bike computer (within first 30s of FIT file track) 
1. run ```python align.py -b <START> -t <DURATION> path_to_fit_file.fit path_to_video.mp4``` to generate alignment clip
1. open video with your favorite video player and find exact frame at which speed changes
1. locate data frame with same speed change in alignment data and remember the ```OFFSET```
1. use that ```OFFSET``` as input for main application to align the overlay
