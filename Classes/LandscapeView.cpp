
#include "common.h"

#include "ContentModel.h"
#include "LandscapeView.h"

#include "cocos2d.h"

USING_NS_CC;

LandscapeView::LandscapeView(uint64_t width, uint64_t height)
    : _scene(nullptr)
    , _bgSprite(nullptr)
    , _focusedSpriteIndex(0)
    , _entryCount(0)
    , _scaleByFocus(nullptr)
    , _scaleByUnfocus(nullptr)
    , _requestedEntrySize(0, 0)
    , _boundingBox(nullptr)
    , _designResolutionSize(width, height)
#if WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE
    , _waNavSafeCount(0)
#endif
{
    METHOD_TRACE
}

LandscapeView::~LandscapeView()
{
    METHOD_TRACE

#if !WA_DETERMINE_CLEANUP_PROCEDURE_FOR_COCOS2D
    if (_scene)
    {
        _scene->release();
    }

    if (_scaleByFocus)
    {
        _scaleByFocus->release();
    }

    if (_scaleByUnfocus)
    {
        _scaleByUnfocus->release();
    }

    for (auto a : _moveToActions)
    {
        a->release();
    }

    for (auto s : _imageSprites)
    {
        if (s) s->release();
    }

    for (auto i : _platformImages)
    {
        delete i;
    }

    for (auto t : _textures)
    {
        delete t;
    }

    if (_headerLabel)
    {
        _headerLabel->release();
    }

    if (_footerLabel)
    {
        _footerLabel->release();
    }

    if (_boundingBox)
    {
        _boundingBox->release();
    }

    if (_bgSprite)
    {
        _bgSprite->release();
    }

    Director::getInstance()->end();

#endif /* WA_DETERMINE_CLEANUP_PROCEDURE_FOR_COCOS2D */
}

static const float animationDuration = 0.2f;
static const float imageScaleFactor  = 1.5f;
static const float imageGapFactor    = 1.325f;

static const std::string systemFontName = "Helvetica";
static const float textWrapFactor       =  0.6f;
static const float fontSize             = 24.0f;

void LandscapeView::init(ContentModel* m, ContentController* c)
{
    METHOD_TRACE

    ContentView::init(m, c);

    // Much of this is boilerplate from cocos2d
    const cocos2d::Size smallResolutionSize  = cocos2d::Size(480, 320);
    const cocos2d::Size mediumResolutionSize = cocos2d::Size(1280, 720);
    const cocos2d::Size largeResolutionSize  = cocos2d::Size(2048, 1536);

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("MlbSlideshow", cocos2d::Rect(0, 0, _designResolutionSize.width, _designResolutionSize.height));
#else
        glview = GLViewImpl::create("MlbSlideshow");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(_designResolutionSize.width, _designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/_designResolutionSize.height, largeResolutionSize.width/_designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/_designResolutionSize.height, mediumResolutionSize.width/_designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/_designResolutionSize.height, smallResolutionSize.width/_designResolutionSize.width));
    }

    _scaleByFocus = ScaleBy::create(animationDuration, imageScaleFactor);
    _scaleByFocus->retain();

    _scaleByUnfocus = _scaleByFocus->reverse();
    _scaleByUnfocus->retain();

    _scene = cocos2d::Scene::create();
    _scene->retain();
}

void LandscapeView::start()
{
    Director::getInstance()->runWithScene(_scene);
}

void LandscapeView::onModelManifestProcessed(int64_t entryCount, uint64_t requestedEntryWidth, uint64_t requestedEntryHeight)
{
    const Vec2 windowCenter(_designResolutionSize.width / 2.0f, _designResolutionSize.height / 2.0f);

    _requestedEntrySize = cocos2d::Size(requestedEntryWidth, requestedEntryHeight);

    _entryCount = entryCount;

    // Build up a list of cocos2d::MoveTo actions to apply for navigation
    for (int64_t k = 0; k < _entryCount; ++k)
    {
        auto rightAction  = MoveTo::create(animationDuration, Vec2(windowCenter.x + k * (imageGapFactor * _requestedEntrySize.width), windowCenter.y));
        rightAction->retain();
        _moveToActions.push_back(rightAction);

        if (k > 0)
        {
            auto leftAction  = MoveTo::create(animationDuration, Vec2(windowCenter.x - k * (imageGapFactor * _requestedEntrySize.width), windowCenter.y));
            leftAction->retain();
            _moveToActions.push_front(leftAction);
        }

        auto sprite = cocos2d::Sprite::create();
        sprite->retain();
        _imageSprites.push_back(sprite);
    }

    // This node will show the header blurb
    _headerLabel = Label::createWithSystemFont("", systemFontName, fontSize);
    _headerLabel->retain();

    if (_headerLabel)
    {
        cocos2d::Size headerDimensions = _headerLabel->getContentSize();
        _headerLabel->setPosition(Vec2(windowCenter.x, windowCenter.y + _requestedEntrySize.height));
        _scene->addChild(_headerLabel, 1);
        _headerLabel->setVisible(false);
    }
    else
    {
        TRACE("problem loading '%s'\n", systemFontName.c_str());
    }

    // This node will show the footer blurb
    _footerLabel = Label::createWithSystemFont("", systemFontName, fontSize);
    _footerLabel->retain();

    if (_footerLabel)
    {
        cocos2d::Size footerDimensions = _footerLabel->getContentSize();
        _footerLabel->setPosition(Vec2(windowCenter.x, windowCenter.y - _requestedEntrySize.height));
        _scene->addChild(_footerLabel, 1);
        _footerLabel->setVisible(false);
        _footerLabel->setWidth(_requestedEntrySize.width * textWrapFactor * imageScaleFactor);
        _footerLabel->setAlignment(TextHAlignment::CENTER);

    }
    else
    {
        TRACE("problem loading '%s'\n", systemFontName.c_str());
    }

    // This will serve as a bounding box for the focused element
    _boundingBox = cocos2d::DrawNode::create();
    _boundingBox->retain();

    float halfWidth  = (imageScaleFactor * _requestedEntrySize.width)  / 2.0 + 10.0;
    float halfHeight = (imageScaleFactor * _requestedEntrySize.height) / 2.0 + 10.0;

    auto x1y1 = Vec2(-halfWidth, -halfHeight);
    auto x2y2 = Vec2( halfWidth,  halfHeight);

    _boundingBox->setPosition(windowCenter);
    // _boundingBox->setLineWidth(4); // No effect, internally hard-coded to 2
    _boundingBox->drawRect(x1y1, x2y2, Color4F::WHITE);
    _scene->addChild(_boundingBox, 1);
    _boundingBox->setVisible(false);
}

void LandscapeView::onModelEntryChange(ContentEntry* entry)
{
    METHOD_TRACE

    cocos2d::Size imageDimensions(entry->getImageWidth(), entry->getImageHeight());
    const Vec2 windowCenter(_designResolutionSize.width / 2.0f, _designResolutionSize.height / 2.0f);
    auto order = entry->getOrder();

    if (entry->_dirtyFlags & ContentEntry::HEADER_CHANGED)
    {
        _headers.push_back(entry->getHeader());

        entry->_dirtyFlags &= ~ContentEntry::HEADER_CHANGED;
    }

    if (entry->_dirtyFlags & ContentEntry::FOOTER_CHANGED)
    {

        _footers.push_back(entry->getFooter());

        entry->_dirtyFlags &= ~ContentEntry::FOOTER_CHANGED;
    }

    if (entry->_dirtyFlags & ContentEntry::IMAGE_CHANGED)
    {
        cocos2d::Image* image    = new cocos2d::Image();
        _platformImages.push_back(image);
        const char* imageData    = entry->getImage();
        uint64_t imageDataLength = entry->getImageDataLength();

        if (image && imageData && imageDataLength &&
            image->initWithImageData(reinterpret_cast<const unsigned char*>(imageData), imageDataLength))
        {
            // TODO: alternatively there is a sprite (and may be image?) createWithTexture, etc

            TRACE("jpeg SUCCESSFULLY loaded into cocos2d::Image!!\n");
            cocos2d::Texture2D* texture = new cocos2d::Texture2D();
            _textures.push_back(texture);

            if (texture && texture->initWithImage(image))
            {
                TRACE("cocos2d::Image SUCCESSFULLY loaded into cocos2d::Texture2D!!\n");

                cocos2d::Sprite* sprite = nullptr;

                if (order == ContentModel::BG_ENTRY)
                {
                    sprite = cocos2d::Sprite::create();
                    sprite->retain();
                    _bgSprite = sprite;
                }
                else
                {
                    sprite = _imageSprites[order];
                }

                if (sprite->initWithTexture(texture))
                {
                    TRACE("cocos2d::Texture2D SUCCESSFULLY loaded into cocos2d::Sprite!!\n");

                    if (order != ContentModel::BG_ENTRY)
                    {
                        _scene->addChild(sprite, 1);

                        if (order < _imageSprites.size())
                        {
                            int64_t initFocus = order - _focusedSpriteIndex;

                            if (initFocus == 0)
                            {
                                focus(initFocus, true);
                            }

                            sprite->setPosition(windowCenter.x + initFocus * (imageGapFactor * imageDimensions.width), windowCenter.y);

#if WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE
                            _waNavSafeCount++;
#endif
                        }
                        else
                        {
                            TRACE(" @@ Unexpected order of image (non-bg) download"
                                  " arriving before manifest has been processed!\n");
                        }
                    }
                    else
                    {
                        _scene->addChild(sprite, 0);
                        sprite->setPosition(windowCenter);
                        auto bgWidth  = sprite->getContentSize().width;
                        auto bgHeight = sprite->getContentSize().height;
                        sprite->setScale(_designResolutionSize.width / bgWidth, _designResolutionSize.height / bgHeight);
                    }

                    entry->_dirtyFlags &= ~ContentEntry::IMAGE_CHANGED;
                }
                else
                {
                    TRACE("cocos2d::Texture2D NOT loaded into cocos2d::Sprite!!\n");
                }
            }
            else
            {
                TRACE("cocos2d::Image NOT loaded into cocos2d::Texture2D!!\n");
            }
        }
        else
        {
            TRACE("compressed jpeg data NOT loaded into cocos2d::Image!!\n");
        }
    }
}

void LandscapeView::focus(int64_t entry, bool focusState)
{
    METHOD_TRACE

    if (_headerLabel != nullptr)
    {
        _headerLabel->setString(_headers[entry]);
        _headerLabel->setVisible(focusState);
    }

    if (_footerLabel)
    {
        _footerLabel->setString(_footers[entry]);
        _footerLabel->setVisible(focusState);
    }

    if (_scaleByFocus && _scaleByUnfocus)
    {
        if (focusState)
        {
            _boundingBox->setVisible(false);
        }

        auto callbackToggleBound = CallFunc::create([focusState, this]()
        {
            if (focusState)
            {
                this->_boundingBox->setVisible(true);
            }
        });

        // create a sequence to up/downscale the sprite and then toggle the bounding box
        auto sequence = Sequence::create((focusState) ? _scaleByFocus : _scaleByUnfocus, callbackToggleBound, nullptr);

        _imageSprites[entry]->runAction(sequence);
        TRACE("%s: @@@ Executing action!\n", __PRETTY_FUNCTION__);
    }
    else
    {
        TRACE("%s: @@@ Choosing not to run the action: "
              "_imageSprites[%lld]: %p, _scaleByFocus: %p, _scaleByUnfocus: %p\n",
              __PRETTY_FUNCTION__, entry, _imageSprites[entry], _scaleByFocus, _scaleByUnfocus);
    }
}

void LandscapeView::animateSprites(ContentView::FocusDirection direction)
{
    METHOD_TRACE

    int64_t firstMoveToApply = _entryCount - _focusedSpriteIndex - 1;
    int64_t moveIndex = 0;
    int64_t spriteIndex = 0;

    // Apply all MoveTo actions to the image sprites
    for (auto m = _moveToActions.begin(); m != _moveToActions.end() && spriteIndex < _entryCount; ++m)
    {
        if (moveIndex >= firstMoveToApply)
        {
            TRACE("_imageSprites[%lld]->runAction(%lld);\n", spriteIndex, moveIndex);

            _imageSprites[spriteIndex]->runAction(*m);
            _imageSprites[spriteIndex]->setVisible(true);

            ++spriteIndex;
        }

        ++moveIndex;
    }
}

void LandscapeView::navigate(ContentView::FocusDirection direction)
{
    METHOD_TRACE

#if WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE
    if (_waNavSafeCount < _entryCount)
    {
        TRACE("WA_DISABLE_NAV_UNTIL_IMAGES_ARRIVE: Navigation disallowed for the moment...\n");
        return;
    }
#endif

    auto actionManager = Director::getInstance()->getActionManager();
    ssize_t runningActions = actionManager->getNumberOfRunningActions();

    // Debug cocos2d will assert if you try to re-run actions when they
    // are already running, so don't let the user advance until the prevous
    // ones are done...
    if (runningActions > 0)
    {
        TRACE("Woh, slow down: there are already %zd running actions\n", runningActions);
        return;
    }

    if ((direction == FocusDirection::Next) && (_focusedSpriteIndex + 1 >= _entryCount))
    {
        TRACE("sprite %lld is already focused\n", _focusedSpriteIndex);
        return;
    }

    if ((direction == FocusDirection::Prev) && (_focusedSpriteIndex - 1 < 0))
    {
        TRACE("sprite %lld is already focused\n", _focusedSpriteIndex);
        return;
    }

    focus(_focusedSpriteIndex, false);

    switch (direction)
    {
    case FocusDirection::Prev:
        _focusedSpriteIndex--;
        break;

    case FocusDirection::Next:
        _focusedSpriteIndex++;
        break;

    default:
        TRACE("Unexpected direction!\n");
        return;
    }

    animateSprites(direction);

    focus(_focusedSpriteIndex, true);
}
