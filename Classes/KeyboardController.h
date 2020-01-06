
#ifndef _KEYBOARD_CONTROLLER_H_
#define _KEYBOARD_CONTROLLER_H_

#include "ContentController.h"

class KeyboardController : public ContentController
{
public:
    KeyboardController();
    ~KeyboardController();

    void init(ContentModel*, ContentView*);
};

#endif /* _KEYBOARD_CONTROLLER_H_ */
