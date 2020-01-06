
#include "common.h"

#include "ContentModel.h"
#include "ContentView.h"
#include "ContentController.h"

ContentController::ContentController()
    : _model(nullptr)
    , _view(nullptr)
{
    METHOD_TRACE
}

ContentController::~ContentController()
{
    METHOD_TRACE
}

void ContentController::init(ContentModel* m, ContentView* v)
{
    METHOD_TRACE

    _model = m;
    _view  = v;
}

void ContentController::requestContent(std::string manifestUrl, std::string bgEntryUrl)
{
    METHOD_TRACE

    if (_model)
    {
        _model->setBgEntry(bgEntryUrl);
        _model->sync(manifestUrl);
    }
}

void ContentController::onNavigateLeft()
{
    METHOD_TRACE

    if (_view)
    {
        _view->navigate(ContentView::FocusDirection::Prev);
    }
}

void ContentController::onNavigateRight()
{
    METHOD_TRACE

    if (_view)
    {
        _view->navigate(ContentView::FocusDirection::Next);
    }
}
