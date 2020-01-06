
#include "AppDelegate.h"
#include "common.h"

#include "MlbModel.h"
#include "LandscapeView.h"
#include "KeyboardController.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
    METHOD_TRACE
}

AppDelegate::~AppDelegate()
{
    METHOD_TRACE

    delete _controller;
    delete _view;
    delete _model;
}

// override/boilerplate from cocos2d
void AppDelegate::initGLContextAttrs()
{
    METHOD_TRACE

    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// override from cocos2d, a place for us to set up our objects
bool AppDelegate::applicationDidFinishLaunching()
{
    METHOD_TRACE

    // We're requesting MLB assets in the 320x180 section
    _model      = new MlbModel(320, 180);

    // Asking for a 1280x720 window
    _view       = new LandscapeView(1280, 720);

    // We want to use the keyboard for navigation
    _controller = new KeyboardController();

    // Tie the mvc triangle together
    _model->init(_view, _controller);
    _view->init(_model, _controller);
    _controller->init(_model, _view);

    // This most closely corresponds to a content manifest, so that's what we call it
    std::string manifestUrl = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";

    // We also request a special "extra" asset that's not in the manifest (the background image)
    std::string bgImageUrl  = "http://mlb.mlb.com/mlb/images/devices/ballpark/1920x1080/1.jpg";

    // Controller messages the model to initiate content sync
    _controller->requestContent(manifestUrl, bgImageUrl);

    // This starts the UI
    _view->start();

    return true;
}

// override from cocos2d
void AppDelegate::applicationDidEnterBackground()
{
    METHOD_TRACE

    Director::getInstance()->stopAnimation();
}

// override from cocos2d
void AppDelegate::applicationWillEnterForeground()
{
    METHOD_TRACE

    Director::getInstance()->startAnimation();
}
