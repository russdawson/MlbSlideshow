
#ifndef _LANDSCAPE_VIEW_H_
#define _LANDSCAPE_VIEW_H_

#include "cocos2d.h"

class ContentModel;
class ContentController;

#include "ContentView.h"

class LandscapeView : public ContentView
{
public:
    LandscapeView(uint64_t, uint64_t);
    ~LandscapeView();

    // A place to do setup, etc
    void init(ContentModel*, ContentController*);

    void start();
    void onModelManifestProcessed(int64_t, uint64_t, uint64_t);
    void onModelEntryChange(ContentEntry*);
    void navigate(ContentView::FocusDirection);

private:
    void animateSprites(ContentView::FocusDirection);
    void focus(int64_t, bool);

    cocos2d::Scene*                  _scene;
    std::vector<cocos2d::Sprite*>    _imageSprites;
    cocos2d::Sprite*                 _bgSprite;

    cocos2d::Label*                  _headerLabel;
    cocos2d::Label*                  _footerLabel;
    std::vector<std::string>         _headers;
    std::vector<std::string>         _footers;

    std::vector<cocos2d::Image*>     _platformImages;
    std::vector<cocos2d::Texture2D*> _textures;
    int64_t                          _focusedSpriteIndex;
    int64_t                          _entryCount;
    std::list<cocos2d::MoveTo*>      _moveToActions;
    cocos2d::ScaleBy*                _scaleByFocus;
    cocos2d::ScaleBy*                _scaleByUnfocus;
    cocos2d::Size                    _requestedEntrySize;
    cocos2d::DrawNode*               _boundingBox;
    cocos2d::Size                    _designResolutionSize;

#if WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE
    int64_t _waNavSafeCount;
#endif
};

#endif /* _LANDSCAPE_VIEW_H_ */
