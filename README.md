# Videographer

[![Build and Test](https://github.com/neri14/videographer/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/neri14/videographer/actions/workflows/build-and-test.yml)


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
