/******************************************************************************
 * taglib_URIStream example program
 ******************************************************************************
 * This is an example program that demonstrates how to use the taglib_URIStream
 * to have TagLib open and read tags from a file on an HTTP server. It may work
 * with other server protocols since it uses libcurl, but I haven't tried this.
 * 
 * Usage: 
 * ./example 'https://archive.org/download/78_shaving-cream_paul-wynn_gbia0001215b/Shaving%20Cream%20-%20Paul%20Wynn-restored.mp3'
 * 
 ******************************************************************************
 * License
 ******************************************************************************
 * 
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <quizno50@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return
 *
 * Ron Moore
 * 
 *****************************************************************************/
#include <iostream>
#include "URIStream.h"
#include <taglib/fileref.h>

int main(int argc, char** argv) {
	try {
		URIStream us(argv[1]);
		TagLib::FileRef f(&us);
		TagLib::StringList props;

		props.append(TagLib::String("TITLE"));
		props.append(TagLib::String("ALBUM"));
		props.append(TagLib::String("ARTIST"));
		props.append(TagLib::String("DATE"));
		

		if (f.tag() == nullptr) {
			throw std::runtime_error("Got a nullptr on .tag() cannot continue.\n");
		}

		for (auto const& prop : props) {
			if (f.tag()->properties().contains(prop) && f.tag()->properties()[prop].size() > 0) {
				std::cout << prop << ": " << f.tag()->properties()[prop][0] << "\n";
			} else {
				std::cout << prop << " is not set.\n";
			}
		}
	} catch (CURLcode e) {
		std::cerr << "Failed with: " << curl_easy_strerror(e) << "\n";
		return -1;
	} catch (std::exception const& e) {
		std::cerr << "Failed with: " << e.what() << "\n";
		return -1;
	}
	return 0;
}

