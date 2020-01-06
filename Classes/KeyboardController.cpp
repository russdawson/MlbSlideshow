
#include "common.h"

#include "ContentModel.h"
#include "ContentView.h"
#include "KeyboardController.h"

#include "cocos2d.h"

KeyboardController::KeyboardController()
{
    METHOD_TRACE
}

KeyboardController::~KeyboardController()
{
    METHOD_TRACE
}

// Set up keyboard listeners to respond to left and right key
// Note: key-down seemed to be a better experience for navigation,
// rather than key-up.
void KeyboardController::init(ContentModel* m, ContentView* v)
{
    METHOD_TRACE

    ContentController::init(m, v);

    cocos2d::EventDispatcher* eventDispatcher = nullptr;

    eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
    {
        METHOD_TRACE

        TRACE("Key %d was pressed\n", (int)keyCode);

        switch (keyCode)
        {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            this->onNavigateLeft();
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            this->onNavigateRight();
            break;

        default:
            break;
        }
    };
    
    listener->onKeyReleased = [](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
    {
        METHOD_TRACE

        TRACE("Key %d was released\n", (int)keyCode);
    };

    eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
}
