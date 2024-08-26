# GPU vs CPU measurements

## Tests
On 1minute 1080p file

### Base measurements
#### Original impl
46.866s / 45.314s

#### x264enc -> nvh264enc
30.006s


### After removing overlay
#### Original impl
34.834s
35.933s

#### x264enc -> nvh264enc
6.099s
6.008s


### Added upscale
Upscaling 1080p->4k

Bitrate 16->80mbit

#### Original impl
2m42.928s

#### x264enc -> nvh264enc
17.390s

#### Moved all video to gpu
15.469s
15.564s
15.484s

#### added crude overlay
54.255s (29.196s of measured drawing time (runs on CPU))

i.e.:
- about 10s is overhead from passing memory to GPU and GPU applying the overlay
- about 29s is overhead from crude CPU drawing without caching that can be improved

Note the overlay is now generated after scaling - i.e. CPU measurements were in 1080p genration, GPU is in 4K


## Test on raw GoPro video
- Video length: 39m30s
- Resolution: 1080p
- Full GPU impl
- no overlays

Time: 
8m45.467s (4x speedup!)

