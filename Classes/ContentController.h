
#ifndef _CONTENT_CONTROLLER_H_
#define _CONTENT_CONTROLLER_H_

class ContentModel;
class ContentView;

// Superclass for controllers, I would envision subclasses
// like IR remote, keyboard, etc.
class ContentController
{
public:
    ContentController();
    virtual ~ContentController();

    // Tie to a model and view, and allow the subclass
    // a place to set up
    virtual void init(ContentModel*, ContentView*);

    // A place for initiating content sync with
    // (1) a content manifest and
    // (2) a background image
    void requestContent(std::string, std::string);

    // A place for messaging the view to move left
    void onNavigateLeft();

    // A place for messaging the view to move right
    void onNavigateRight();

protected:
    ContentModel* _model;
    ContentView*  _view;
};

#endif /* _CONTENT_CONTROLLER_H_ */
