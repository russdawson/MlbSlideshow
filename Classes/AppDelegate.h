
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

#include "ContentModel.h"
#include "ContentController.h"
#include "ContentView.h"

// Our cocos2s Application override
class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    void initGLContextAttrs();
    bool applicationDidFinishLaunching();
    void applicationDidEnterBackground();
    void applicationWillEnterForeground();

private:
    ContentModel*      _model;
    ContentView*       _view;
    ContentController* _controller;
};

#endif // _APP_DELEGATE_H_

