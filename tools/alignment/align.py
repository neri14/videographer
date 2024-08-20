import argparse
import os
import signal
import sys
from datetime import timedelta
import math

from garmin_fit_sdk import Decoder, Stream
from datetime import datetime

import moviepy.editor as mpy
from moviepy.video.io.bindings import PIL_to_npimage
import numpy as np
from PIL import Image,ImageDraw,ImageFont
from matplotlib import font_manager

OUTPUT_VIDEO = 'alignment.mp4'

DEFAULT_WIDTH = 3840
DEFAULT_HEIGHT = 2160

STALE_INTERVAL = 5


## FIT FILE HANDLING ##
def parse_fit_file(path):
    stream = Stream.from_file(path)
    decoder = Decoder(stream)
    messages, errors = decoder.read()

    data = {}

    for msg in messages["record_mesgs"]:
        if "timestamp" not in msg or "position_lat" not in msg or "position_long" not in msg:
            print("Missing required data in datapoint - ignoring")
            continue

        frame = {}
        frame['timestamp'] = msg['timestamp']
        frame['latitude'] = msg['position_lat'] / 11930465   # magic division to degrees
        frame['longitude'] = msg['position_long'] / 11930465 # magic division to degrees
        
        if 'altitude' in msg:
            frame['altitude'] = msg['altitude']
        if 'temperature' in msg:
            frame['temperature'] = msg['temperature']
        if 'cadence' in msg:
            frame['cadence'] = msg['cadence']
        if 'power' in msg:
            frame['power'] = msg['power']
        if 'heart_rate' in msg:
            frame['heart_rate'] = msg['heart_rate']
        if 'speed' in msg:
            frame['speed'] = msg['speed'] * 3.6 # meters/s to kilometers/h
        if 'distance' in msg:
            frame['distance'] = msg['distance'] / 1000 # meters to kilometers

        data[frame['timestamp']] = frame

    return data


## VIDEO GENERATION ##

class VideoGenerator:
    def __init__(self, vidfile, data):
        self.vidfile = vidfile
        self.data = data

        self.width = DEFAULT_WIDTH
        self.height = DEFAULT_HEIGHT

        timestamps = list(data.keys())
        self.first_stamp = timestamps[0]
        self.last_stamp = timestamps[-1]


    def generate(self, begin, length):
        video = mpy.VideoFileClip(self.vidfile)
        self.width = video.w
        self.height = video.h

        video = video.fl(self.draw) # apply overlay
        video = video.subclip(begin, begin+length) # trim output

        video.write_videofile(OUTPUT_VIDEO, threads=os.cpu_count())


    def draw(self, get_frame, t):
        f = get_frame(t).astype(np.uint8)
        img = Image.fromarray(f)
        img_ov = self.draw_overlay(self.data, t)
        img.paste(img_ov, (0, 0), img_ov)
        return PIL_to_npimage(img)


    def draw_overlay(self, data, vid_time):
        img = Image.new('RGBA', (self.width, self.height), (255, 0, 0, 0))
        canvas = ImageDraw.Draw(img)
        
        font_big = ImageFont.FreeTypeFont(font_manager.findfont('monospace'), 40)
        font = ImageFont.FreeTypeFont(font_manager.findfont('monospace'), 18)

        canvas.text((50, 10),
                    self.timecode(vid_time),
                    font=font_big, fill='white', stroke_width=5, stroke_fill='black', anchor='lt')

        canvas.text((50, 65),
                    "      UTC time              Speed         Power      Latitude        Longitude           Offset",
                    font=font, fill='white', stroke_width=2, stroke_fill='black', anchor='lt')

        lines = 20
        dist = 23
        x = 50
        y = 95

        for df in data.values():
            offset = vid_time - (df['timestamp'] - self.first_stamp).total_seconds()


            line = "{}   {:5.2f} km/h   {:6.1f} W    {:12.9f}    {:12.9f}      {:11.6f}".format(
                df['timestamp'].strftime('%Y-%m-%d %H:%M:%S.%f')[:-3],
                df['speed'], df['power'] if 'power' in df else 0, df['latitude'], df['longitude'], offset
            )

            canvas.text((x, y), line, font=font, fill='white', stroke_width=2, stroke_fill='black', anchor='lt')
            y += dist

            lines -= 1
            if not lines:
                break

        return img
    

    def timecode(self, vidtime):
        h = math.floor(vidtime / 3600)
        m = math.floor(vidtime / 60)
        s = vidtime % 60

        return "{:02d}:{:02d}:{:09.6f}".format(h,m,s)


## GENERAL RUNTIME ##
def parse_args():
    parser = argparse.ArgumentParser(
        description='VideoTelemetryAlignmentTool',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('fit',
                        metavar='FIT',
                        nargs=1,
                        help='Path to fit file')
    
    parser.add_argument('vid',
                        metavar='VID',
                        nargs=1,
                        help='Path to video file')

    parser.add_argument('-b', '--begin',
                        metavar='BEGIN',
                        nargs='?',
                        default=0,
                        const=0,
                        type=int,
                        help='Trimmed video begin time in seconds')
    parser.add_argument('-t', '--time',
                        metavar='TIME',
                        nargs='?',
                        default=60,
                        const=60,
                        type=int,
                        help='Trimmed video length in seconds')

    return parser.parse_args()


def main():
    args = parse_args()
    data = parse_fit_file(args.fit[0])

    generator = VideoGenerator(args.vid[0], data)
    generator.generate(args.begin, args.time)


def signal_handler(sig, frame):
    print("Signal received - exitting")
    sys.exit(0)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    main()
