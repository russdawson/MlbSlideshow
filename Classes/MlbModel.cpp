
#include "common.h"

#include "MlbModel.h"
#include "ContentView.h"
#include "ContentController.h"

#include "json/document-wrapper.h"
#include "cocos2d.h"

MlbModel::MlbModel(uint64_t w, uint64_t h)
    : _requestedAssetWidth(w)
    , _requestedAssetHeight(h)
{
    METHOD_TRACE
}

MlbModel::~MlbModel()
{
    METHOD_TRACE
}

// Manifest is returned here, we can process it and see what assets
// we need to download, and get header and footer strings
void MlbModel::onSyncCompleted(HttpClient *sender, HttpResponse *response)
{
    METHOD_TRACE

    if (!response)
    {
        TRACE("response is nullptr!\n");
        return;
    }

    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag())) 
    {
        TRACE("%s completed\n", response->getHttpRequest()->getTag());
    }
    
    long statusCode = response->getResponseCode();
    char statusString[64] = {};
    TRACE(statusString, "HTTP Status Code: %ld, tag = %s\n", statusCode, response->getHttpRequest()->getTag());
    
    if (!response->isSucceed()) 
    {
        TRACE("response failed: %s\n", response->getErrorBuffer());
        return;
    }

    std::vector<char> *response_header = response->getResponseHeader();
    std::string header_raw(response_header->begin(), response_header->end());

    if (header_raw.empty())
    {
        TRACE(" @@ header is empty!!\n");
        return;
    }

    std::stringstream header_stream(header_raw);
    std::string header_line;
    bool is_json = false;

    while (std::getline(header_stream, header_line))
    {
        std::size_t found = header_line.find("Content-Type");

        if (found != std::string::npos)
        {
            found = header_line.find("application/json");

            if (found != std::string::npos)
            {
                is_json = true;
                break;
            }

            TRACE("Unexpected %s\n", header_line.c_str());
            return;
        }
    }

    if (is_json)
    {
        TRACE("json found!!\n");

        std::vector<char> *response_data = response->getResponseData();
        std::string json_raw(response_data->begin(), response_data->end());

        rapidjson::Document manifest;

        if (!json_raw.empty())
        {
            // Parse file with rapid json
            manifest.Parse<0>(json_raw.c_str());
            // Print error
            if (manifest.HasParseError()) {
                size_t offset = manifest.GetErrorOffset();
                if (offset > 0)
                    offset--;
                std::string errorSnippet = json_raw.substr(offset, 10);
                TRACE("json parse error %d at <%s>\n", manifest.GetParseError(), errorSnippet.c_str());
            }
            else
            {
                TRACE("json parsing succeeded!\n");

                // TODO this should perhaps be parsed more carefully, checking that
                // these are not null values, etc.

                rapidjson::Value& games_subroot = manifest["dates"][0]["games"];
                rapidjson::SizeType total_games = games_subroot.Size();

                for (rapidjson::SizeType k = 0; k < total_games; ++k)
                {

                    rapidjson::Value& teams_subroot = games_subroot[k]["teams"];

                    std::string awayTeamName = teams_subroot["away"]["team"]["name"].GetString();
                    std::string homeTeamName = teams_subroot["home"]["team"]["name"].GetString();

                    rapidjson::Value& neutral_subroot = games_subroot[k]["content"]["editorial"]["recap"]["mlb"];

                    std::string headline = neutral_subroot["headline"].GetString();
                    std::string requestedDimension = std::to_string(_requestedAssetWidth) + "x" + std::to_string(_requestedAssetHeight);
                    std::string imageUrl = neutral_subroot["photo"]["cuts"][requestedDimension.c_str()]["src"].GetString();
                    TRACE("[ game %d:\n", k);
                    TRACE("  top: %s\n", headline.c_str());
                    TRACE("  bottom: %s @ %s\n", awayTeamName.c_str(), homeTeamName.c_str());
                    TRACE("  dimensions: %s\n", requestedDimension.c_str());
                    TRACE("  image url: %s\n", imageUrl.c_str());
                    TRACE("]\n");

                    auto e = new MlbEntry(awayTeamName + " @ " + homeTeamName, headline, imageUrl, k, _requestedAssetWidth, _requestedAssetHeight);
                    _entries.push_back(e);

                    // image data is not here yet but the ContentView knows enough to start doing layout
                    _view->onModelEntryChange(e);

                    fetchAsset(imageUrl, std::to_string(k));
                }

                _view->onModelManifestProcessed(_entries.size(), _requestedAssetWidth, _requestedAssetHeight);
            }
        }
        else
        {
            TRACE("got an HttpResponse, but the data portion was empty!\n");
        }
    }
}


void MlbModel::onFetchAssetCompleted(HttpClient *sender, HttpResponse *response)
{
    METHOD_TRACE

    if (!response)
    {
        TRACE("response is nullptr!\n");
        return;
    }

    long statusCode = response->getResponseCode();
    char statusString[64] = {};
    TRACE(statusString, "HTTP Status Code: %ld, tag = %s\n", statusCode, response->getHttpRequest()->getTag());
    
    if (!response->isSucceed()) 
    {
        TRACE("response failed: %s\n", response->getErrorBuffer());
        return;
    }

    std::vector<char> *response_header = response->getResponseHeader();
    std::string header_raw(response_header->begin(), response_header->end());

    if (header_raw.empty())
    {
        TRACE(" @@ header is empty!!\n");
        return;
    }

    std::stringstream header_stream(header_raw);
    std::string header_line;
    bool is_jpeg = false;

    while (std::getline(header_stream, header_line))
    {
        std::size_t found = header_line.find("Content-Type");

        if (found != std::string::npos)
        {
            found = header_line.find("image/jpeg");

            if (found != std::string::npos)
            {
                is_jpeg = true;
                break;
            }

            TRACE("Unexpected %s\n", header_line.c_str());

            return;
        }
    }

    if (is_jpeg)
    {
        TRACE("jpeg found!!\n");

        std::vector<char> *response_data = response->getResponseData();

        if (!response_data->empty())
        {
            const char *jpeg_raw = response_data->data();

            unsigned char temp[3] = {0};
            memcpy(temp, jpeg_raw, 3);
            TRACE("jpeg sig: %02hhx %02hhx %02hhx\n", temp[0], temp[1], temp[2]);

            const char* gameNumber = response->getHttpRequest()->getTag();

            if (0 != strlen(gameNumber))
            {
                TRACE("game image %s download completed\n", gameNumber);
            }

            int64_t n = std::stoi(gameNumber);
            MlbEntry* e = nullptr;

            if (n != ContentModel::BG_ENTRY)
            {
                e = dynamic_cast<MlbEntry*>(_entries[n]);
            }
            else
            {
                e = dynamic_cast<MlbEntry*>(_bgEntry);
            }

            if (e)
            {
                e->setHttpResponse(response);
                e->setEncodedImage(jpeg_raw, response_data->size());

                if (_view)
                {
                    _view->onModelEntryChange(e);
                }
            }
            else
            {
                TRACE("%s: expected an MlbEntry!\n", __PRETTY_FUNCTION__);
            }
        }
    }
}

void MlbModel::sync(std::string manifestUrl)
{
    // TODO collapse this shared code with fetchAsset into
    // a helper method, and pass the callback directly

    METHOD_TRACE

    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(manifestUrl);
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(MlbModel::onSyncCompleted, this));
    request->setTag("content manifest");
    HttpClient::getInstance()->sendImmediate(request);

    request->release();
}

void MlbModel::setBgEntry(std::string bgEntryUrl)
{
    METHOD_TRACE

    _bgEntry = new MlbEntry("", "", bgEntryUrl, ContentModel::BG_ENTRY, 1920, 1080);

    fetchAsset(bgEntryUrl, std::to_string(ContentModel::BG_ENTRY));
}

void MlbModel::fetchAsset(std::string assetUrl, std::string tag)
{
    METHOD_TRACE

    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(assetUrl);
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(MlbModel::onFetchAssetCompleted, this));
    request->setTag(tag);
    HttpClient::getInstance()->send(request);

    request->release();
}
