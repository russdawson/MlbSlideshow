
#ifndef _CONTENT_MODEL_H_
#define _CONTENT_MODEL_H_

class ContentView;
class ContentController;

#include <vector>

// A superclass to manipulate general data of the form:
// (1) text header
// (2) text footer
// (3) encoded image data
// Where each entry has a relative order
class ContentEntry
{
public:
    ContentEntry();
    virtual ~ContentEntry();

    virtual std::string getHeader()       = 0;
    virtual std::string getFooter()       = 0;
    virtual const char* getImage()        = 0;
    virtual uint64_t getImageDataLength() = 0;
    virtual uint64_t getImageWidth()      = 0;
    virtual uint64_t getImageHeight()     = 0;
    virtual int64_t  getOrder()           = 0;

    static const uint32_t HEADER_CHANGED;
    static const uint32_t FOOTER_CHANGED;
    static const uint32_t IMAGE_CHANGED;

    uint32_t _dirtyFlags;
};

class ContentModel
{
public:
    ContentModel();
    virtual ~ContentModel();

    void init(ContentView*, ContentController*);
    virtual void sync(std::string) = 0;

    virtual void setBgEntry(std::string) = 0;
    static const int32_t BG_ENTRY;

protected:
    ContentView*               _view;
    ContentController*         _controller;
    std::vector<ContentEntry*> _entries;
    ContentEntry*              _bgEntry;
};

#endif /* _CONTENT_MODEL_H_ */
