
#ifndef _CONTENT_VIEW_H_
#define _CONTENT_VIEW_H_

class ContentModel;
class ContentController;

// A superclass to manipulate general view of ContentEntry elements
class ContentView
{
public:
    ContentView();
    virtual ~ContentView();

    enum FocusDirection
    {
        Prev,
        Next
    };

    // A place to do setup
    virtual void init(ContentModel*, ContentController*);

    // Change the view in some way as to highlight the next/prev ContentEntry
    virtual void navigate(FocusDirection) = 0;

    // Start the presentation process
    virtual void start() = 0;

    // A way for the model to message the view that manifest processing
    // is complete to the point where the view can start doing layout
    virtual void onModelManifestProcessed(int64_t, uint64_t, uint64_t) = 0;

    // A way for the model to message the view that an entry is ready
    virtual void onModelEntryChange(ContentEntry*) = 0;

protected:
    ContentModel*      _model;
    ContentController* _controller;
};

#endif /* _CONTENT_VIEW_H_ */
