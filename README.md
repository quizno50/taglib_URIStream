# taglib\_URIStream

This is a simple side library for [taglib](https://github.com/taglib/taglib)
that allows loading metadata from a URI instead of a file.

## Requirements
This depends on:
 * TagLib 2.3.2 or greater (note the default Ubuntu version won't work, it ships with 1.13.1)
 * libCURL (any recent version should be fine, must support CURLOPT_RANGE)
 * An HTTP server that supports [ranges](https://www.rfc-editor.org/info/rfc7233/) (The sample
   server from [cpp-httplib](https://github.com/yhirose/cpp-httplib) works fine. The built-in
	Python3 http.server does not.)

## Build
    mkdir build; cd build; cmake CMAKE_BUILD_TYPE=Release ..; make

## Usage
Here is a sample of how to use the example program.

    ./example 'https://archive.org/download/78_shaving-cream_paul-wynn_gbia0001215b/Shaving%20Cream%20-%20Paul%20Wynn-restored.mp3'

If you'd like to use the library in your own program you can follow the
example.cpp file for a simple use case. You can probably integrate this very
quickly into any software that already uses TagLib the only change that should
be required is where the file is opened.

## Further Improvements

* Name spacing would probably be a good idea.

* It works pretty well, but it could definitely use some improvements. It looks
like it takes about 13 requests to get the tags from a file. Most of those are
within the first 8k of the file. I think dividing up the file into ~8k chunks
and having it fetch 8k at a time in the chunk the position variable indicates
while caching the last 2 or 3 chunks would help considerably.

* Tag Writing. There are many ways that this could probably be implemented. I'm
not terribly interested in figuring them out though.

## License

"THE BEER-WARE LICENSE" (Revision 42):
<quizno50@gmail.com> wrote this file. As long as you retain this notice you
can do whatever you want with this stuff. If we meet some day, and you think
this stuff is worth it, you can buy me a beer in return

Ron Moore

