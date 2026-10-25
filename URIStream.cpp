/******************************************************************************
 * taglib_URIStream Implementation
 ******************************************************************************
 * This is the implementation of the URIStream class. I tried to keep it as
 * simple as possible while still being useful.
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
#include "URIStream.h"
#include <boost/numeric/conversion/cast.hpp>

using namespace std::string_literals;

static size_t uristream_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	size_t totalChunk = nmemb * size;
	auto* thisChunk = reinterpret_cast<ByteVector*>(userdata);

	thisChunk->setData(ptr, (size * nmemb));

	return totalChunk;
}

size_t URIStream::populate_headers(char* buf, size_t size, size_t nitems, void* userdata) {
	size_t totalSize = size * nitems;
	std::string delim = ": ";
	auto *headers = reinterpret_cast<std::map<std::string, std::string>*>(userdata);
	char* delim_in_buf = std::search(buf, buf + totalSize, std::boyer_moore_searcher(delim.begin(),
			delim.end()));
	
	if (delim_in_buf != buf + totalSize) {
		(*headers)[std::string(buf, delim_in_buf)] =
				std::string(delim_in_buf + delim.length(), buf + totalSize);
	}
	
	return totalSize;
}

URIStream::URIStream(String const& uri) : _position(0), _uri(uri), _curl(nullptr) {
	_curl = curl_easy_init();
	if (_curl == nullptr) {
		throw CURLE_FAILED_INIT;
	}
	setCurlOptions();
}

void URIStream::setCurlOptions() {
	if (_curl == nullptr) throw std::logic_error("cURL must be initialized before calling this.");
	CURLcode rc = curl_easy_setopt(_curl, CURLOPT_URL, _uri.toCString());
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, uristream_write_callback);
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, true);
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, URIStream::populate_headers);
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_headers);
	if (rc != CURLE_OK) throw rc;
}

URIStream::~URIStream() {
	if (_curl != nullptr) {
		curl_easy_cleanup(_curl);
		_curl = nullptr;
	}
}

FileName URIStream::name() const {
	return _uri.toCString();
}

ByteVector URIStream::readBlock(size_t length) {
	CURLcode rc;
	/* The -1 on the right side of the range string is because these are
	 * inclusive byte indexes. So if we're asking for a read at 1000 of
	 * length 8 and we put 1000-1008 it will give us 9 bytes (bytes 0-8), so we
	 * need to subtract one to ensure our range is right.
	 */
	std::string rangeString = std::to_string(_position) + "-"s + std::to_string(_position + length - 1);
	ByteVector blockData(boost::numeric_cast<unsigned int>(length));
	
	rc = curl_easy_setopt(_curl, CURLOPT_RANGE, rangeString.c_str());
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &blockData);
	if (rc != CURLE_OK) throw rc;
	rc = curl_easy_perform(_curl);
	if (rc != CURLE_OK) throw rc;
	_position += blockData.size();
	return blockData;
}

void URIStream::writeBlock(const ByteVector& data) {
	throw std::invalid_argument("Not Implemented");
}

void URIStream::insert(const ByteVector& data, off_t start, size_t replace) {
	throw std::invalid_argument("Not Implemented");
}

void URIStream::removeBlock(off_t start, size_t length) {
	throw std::invalid_argument("Not Implemented");
}

bool URIStream::readOnly() const {
	return true;
}

bool URIStream::isOpen() const {
	return true;
}

void URIStream::seek(long offset, Position p) {
	switch (p) {
		case Beginning:
			_position = boost::numeric_cast<size_t>(offset);
			break;
		case Current:
			_position += offset;
			break;
		case End:
			if (offset < 0) {
				_position = length() + offset;
			} else {
				_position = length() - offset;
			}
			break;
		default:
			throw std::logic_error("Unknown seek position: " + std::to_string(static_cast<int>(p)));
	}
	if (_position > boost::numeric_cast<size_t>(length()))
			throw std::range_error("Seekd past end of file.");
}

off_t URIStream::tell() const {
	return _position;
}

off_t URIStream::length() {
	if (boost::none == _length) {
		off_t len;
		CURLcode rc = curl_easy_getinfo(_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &len);
		if (rc != CURLE_OK) throw rc;
		if (len < 0) {
			// We need to do a head and try again.
			rc = curl_easy_setopt(_curl, CURLOPT_NOBODY, 1);
			if (rc != CURLE_OK) throw rc;
			rc = curl_easy_perform(_curl);
			if (rc != CURLE_OK) throw rc;
			CURLcode rc = curl_easy_getinfo(_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &len);
			if (rc != CURLE_OK) throw rc;
			rc = curl_easy_setopt(_curl, CURLOPT_NOBODY, 0);
			if (rc != CURLE_OK) throw rc;
			if (len < 0) throw std::runtime_error("cURL can't seem to give us a content length.");
			_length = len;
		}
	}
	return *_length;
}

void URIStream::truncate(long t) {
	throw std::invalid_argument("Not Implemented");
}
