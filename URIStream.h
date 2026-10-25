/******************************************************************************
 * taglib_URIStream Definition
 ******************************************************************************
 * This is the definition of the URIStream class. Many of the virtual methods
 * here are not implemented, or just throw an exception, but they are required
 * to be defined.
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
#include <taglib/tiostream.h>
#include <taglib/tstring.h>
#include <taglib/tpropertymap.h>
#include <curl/curl.h>
#include <boost/optional.hpp>

using namespace TagLib;

class URIStream : public IOStream {
	public:
	URIStream(String const& uri);
	URIStream(URIStream& toCopy) = delete;
	URIStream& operator=(URIStream& toAssign) = delete;

	virtual ~URIStream();
	
	virtual FileName name() const;
	virtual ByteVector readBlock(size_t length);
	virtual void writeBlock(const ByteVector& data);
	virtual void insert(const ByteVector& data, off_t start = 0, size_t replace = 0);
	virtual void removeBlock(off_t start, size_t length);
	virtual bool readOnly() const;
	virtual bool isOpen() const;
	virtual void seek(long offset, Position p = Beginning);
	virtual off_t tell() const;
	virtual off_t length();
	virtual void truncate(long t);

	protected:
	static size_t populate_headers(char* buf, size_t size, size_t nitems, void* userdata);
	virtual void setCurlOptions();

	size_t _position;
	boost::optional<size_t> _length;
	String _uri;
	CURL* _curl;
	std::map<std::string, std::string> _headers;
};

