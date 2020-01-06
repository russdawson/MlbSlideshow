
#ifndef _MLB_MODEL_H_
#define _MLB_MODEL_H_

#include "network/HttpClient.h"

using namespace cocos2d::network;

class ContentView;
class ContentController;

#include "ContentModel.h"

class MlbEntry : public ContentEntry
{
public:
    MlbEntry(std::string teams, std::string gameDescription, std::string imageUrl,
             int64_t gameNumber, uint64_t expectedImageWidth, uint64_t expectedImageHeight)
        : _encodedImageData(nullptr)
        , _httpResponse(nullptr)
    {
        METHOD_TRACE

        _teams               = teams;
        _gameDescription     = gameDescription;
        _imageUrl            = imageUrl;
        _expectedImageWidth  = expectedImageWidth;
        _expectedImageHeight = expectedImageHeight;
        _gameNumber          = gameNumber;

        // since image data is not present yet,
        // we don't consider it dirty.

        if (!_teams.empty())
        {
            _dirtyFlags |= HEADER_CHANGED;
        }

        if (!_gameDescription.empty())
        {
            _dirtyFlags |= FOOTER_CHANGED;
        }
    }

    ~MlbEntry()
    {
        METHOD_TRACE

        if (_httpResponse)
        {
            _httpResponse->release();
        }
    }

    std::string getHeader() { return _teams; }
    std::string getFooter() { return _gameDescription; }
    const char* getImage()  { return _encodedImageData; }

    uint64_t getImageDataLength() { return _encodedImageDataLength; }
    uint64_t getImageWidth()      { return _expectedImageWidth; }
    uint64_t getImageHeight()     { return _expectedImageHeight; }
    int64_t  getOrder()           { return _gameNumber; }

    void setHttpResponse(HttpResponse* response)
    {
        METHOD_TRACE

        _httpResponse = response;
        _httpResponse->retain();
    }

    void setEncodedImage(const char* data, uint64_t length)
    {
        METHOD_TRACE

        _encodedImageData       = data;
        _encodedImageDataLength = length;

        _dirtyFlags |= IMAGE_CHANGED;
    }

private:
    std::string   _teams;
    std::string   _gameDescription;

    const char*   _encodedImageData;
    uint64_t      _encodedImageDataLength;
    uint64_t      _expectedImageWidth;
    uint64_t      _expectedImageHeight;
    int64_t       _gameNumber;

    std::string   _imageUrl;
    HttpResponse* _httpResponse;
};

class MlbModel : public ContentModel
{
public:
    MlbModel(uint64_t, uint64_t);
    ~MlbModel();

    void sync(std::string);
    void setBgEntry(std::string);

private:
    void fetchAsset(std::string, std::string);
    void onSyncCompleted(HttpClient *, HttpResponse *);
    void onFetchAssetCompleted(HttpClient *, HttpResponse *);
    uint64_t _requestedAssetWidth;
    uint64_t _requestedAssetHeight;
};

#endif /* _MLB_MODEL_H_ */
