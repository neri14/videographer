# Tools
List of tools that make life easier and make overlay generator simpler (and faster)

## Alignment tool
Allows to quickly figure out telemetry offset required.

## ToDo
Tools to be wrapped in nice interface

### Concat tool
```ffmpeg -f concat -safe 0 -i mylist.txt -c copy output.mp4```

example mylist.txt:
```
file '/home/neri/videos/gopro/raw/2024-03-27/GX010009.MP4'
file '/home/neri/videos/gopro/raw/2024-03-27/GX020009.MP4'
file '/home/neri/videos/gopro/raw/2024-03-27/GX030009.MP4'
```

Very fast - much easier than concating in app.

### Trim tool
```ffmpeg -ss 00:00:30.0 -to 00:01:00.0 -i input.mp4 -c copy output.mp4```


### Upscale tool
```ffmpeg -hwaccel cuda -hwaccel_output_format cuda -i input.mp4 -c:a copy -vf "scale_cuda=w=3840:h=2160:format=yuv420p" -c:v h264_nvenc -profile:v high -b:v 60M output.mp4```

From quick test has around 3.5x speed
