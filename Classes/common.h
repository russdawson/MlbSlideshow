
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "pthread.h"

#define TRACE(message, ...)           \
{                                     \
    printf((message), ##__VA_ARGS__); \
    fflush(stdout);                   \
}

#define METHOD_TRACE TRACE("%s on thread %p\n", __PRETTY_FUNCTION__, pthread_self());

// Layout is not working if images are in process of being
// downloaded, so for now disable navigation during that time...
#define WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE 1

// Exiting the app in the middle of content load or animation
// will sometimes crash. TODO: find out the proper way to clean
// up, remove sprites from the scene, etc. before destructing
// (removeFromParent or other methods)
#define WA_DETERMINE_CLEANUP_PROCEDURE_FOR_COCOS2D 1

#endif /* _COMMON_H_ */
