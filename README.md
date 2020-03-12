# WARNING: Deprecated!
This is no longer maintained, check [rtsp-server](https://github.com/telmomarques/x360h1080p-rtsp-server) submodule instead.

This repo will not me deleted only for archival purposes.

# xiaomi-360-1080p-video-stream
This a submodule of [xiaomi-360-1080p-hacks](https://github.com/telmomarques/xiaomi-360-1080p-hacks). You may want to go there instead.

## What is this?
C project to grab a video stream from the Xiaomi Mi 360 1080p camera using direct memory access. Output of the video stream goes to standard output, so you can pipe it wherever you want.

## Current state
Still not very good. Much artifacts, such buggy, very 640x360. Wow.
![](video.gif)

## Compiling
Cross compiling for armhf:

`arm-linux-gnueabihf-gcc video_direct_memory_access.c pipe.c -pthread -std=gnu99 -o video_direct_memory_access`

## Example

1. Download the `video_direct_memory_access` binary - or - compile as per instructions above
2. Copy `video_direct_memory_access` to the camera (e.g. using the SD Card)
3. Also copy a armhf build of FFMPEG to the camera (this might be useful: https://ffbinaries.com/downloads)
4. Access the camera through telnet (refer to [this project](https://github.com/telmomarques/xiaomi-360-1080p-hacks))
5. Pipe the output to FFMPEG and stream it (you may see a lot of errors/warnings, ignore them and give it a second):

`./video_direct_memory_access  | ./ffmpeg -f hevc -i pipe:0 -c:v copy -f mpegts udp://<your_computer_ip>:12345`

6. To play the video on your PC:

`ffplay udp://<your_computer_ip>:12345`
