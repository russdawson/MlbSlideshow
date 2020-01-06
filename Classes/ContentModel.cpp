
#include "common.h"
#include "ContentModel.h"
#include "ContentView.h"
#include "ContentController.h"

const uint32_t ContentEntry::HEADER_CHANGED = 0x00000001;
const uint32_t ContentEntry::FOOTER_CHANGED = 0x00000002;
const uint32_t ContentEntry::IMAGE_CHANGED  = 0x00000004;

const int32_t ContentModel::BG_ENTRY = -1;

ContentEntry::ContentEntry()
    : _dirtyFlags(0)
{
    METHOD_TRACE
}

ContentEntry::~ContentEntry()
{
    METHOD_TRACE
}

ContentModel::ContentModel()
    : _view(nullptr)
    , _controller(nullptr)
    , _entries()
    , _bgEntry(nullptr)
{
    METHOD_TRACE
}

ContentModel::~ContentModel()
{
    METHOD_TRACE

    for (auto e : _entries)
    {
        delete e;
    }

    if (_bgEntry)
    {
        delete _bgEntry;
    }
}

void ContentModel::init(ContentView* v, ContentController* c)
{
    METHOD_TRACE

    _view       = v;
    _controller = c;
}
