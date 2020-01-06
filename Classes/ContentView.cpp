
#include "common.h"

#include "ContentModel.h"
#include "ContentView.h"
#include "ContentController.h"

#include "cocos2d.h"

USING_NS_CC;

ContentView::ContentView()
    : _model(nullptr)
    , _controller(nullptr)
{
    METHOD_TRACE
}

ContentView::~ContentView()
{
    METHOD_TRACE
}

void ContentView::init(ContentModel* m, ContentController* c)
{
    METHOD_TRACE

    _model      = m;
    _controller = c;
}
