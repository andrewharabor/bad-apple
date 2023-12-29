# bad-apple

## Description

A 20 FPS ASCII animation of the famous [Bad Apple video](https://www.youtube.com/watch?v=FtutLA63Cp8).

[bad-apple.webm](https://github.com/andrewharabor/bad-apple/assets/120438036/1e40df4a-147e-4849-8c5c-d9c6f2619f0e)
_The first 25 seconds or so of the animation._

## Implementation

The video was split into frames and each one converted to a [BMP](https://en.wikipedia.org/wiki/BMP_file_format) file. This format was used as opposed toa  more popular one like [JPEG](https://en.wikipedia.org/wiki/JPEG) due to the fact that it is uncompressed and easier to read from.

Each image is then converted to ASCII simply by averaging the grayscale values across blocks of pixels and mapping them to characters according to the scale factor. Note that [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code) are used to clear the screen meaning that this implementation is **not necessarily portable**.

A sleep function is used to ensure that a frame is displayed once every 1/20th of a second.
