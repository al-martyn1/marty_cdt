/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Utils for marty_cbp

    Repository: https://github.com/al-martyn1/marty_cbp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "types.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"
#include "umba/sleep.h"
//
#include "marty_url/percent_encoding.h"
//
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cbp/utils.h"
// marty::chrome_devtools_protocol::utils::
// marty::cdp::utils::

namespace marty {
namespace chrome_devtools_protocol {
namespace utils {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
std::string generateChromeUserDataFolderForProjectImpl( const std::string &folderRole // profile, cache etc
                                                      , std::string projectPath
                                                      , std::string browserName=std::string()
                                                      , const std::string &projectSubfolder=std::string() // if empty use temp folder
                                                      )
{
    projectPath = umba::filename::makeCanonical(projectPath);

    if (browserName.empty())
        browserName = "chrome";

    if (!projectSubfolder.empty())
    {
        projectPath = umba::filename::appendPath(projectPath, projectSubfolder);
        return umba::filename::makeCanonical(umba::filename::appendPath(projectPath,  /* "." + browserName + "-" + */  folderRole));
    }

    auto flattenPath = umba::filename::flattenPath(projectPath);

    projectPath = umba::filesys::getTempFolderPath();
    projectPath = umba::filename::appendPath(projectPath, "." + browserName + "-" + folderRole);
    return umba::filename::appendPath(projectPath, flattenPath);
}

//--------------------------------------------------------------------------------------------------------------------
inline
std::string generateChromeUserProfileFolderForProject( const std::string &projectPath
                                                     , std::string browserName=std::string()
                                                     , const std::string &projectSubfolder=std::string() // if empty use temp folder
                                                     )
{
    return generateChromeUserDataFolderForProjectImpl("profile", projectPath, browserName, projectSubfolder);
}

//--------------------------------------------------------------------------------------------------------------------
inline
std::string generateChromeUserCacheFolderForProject( const std::string &projectPath
                                                   , std::string browserName=std::string()
                                                   , const std::string &projectSubfolder=std::string() // if empty use temp folder
                                                   )
{
    return generateChromeUserDataFolderForProjectImpl("cache", projectPath, browserName, projectSubfolder);
}

//--------------------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------------------------------------
/*
    Порт можно потом проверить
    http://localhost:SOME_PORT/json/version

    {
      "Browser": "Chrome/118.0.5993.88",
      "Protocol-Version": "1.3",
      "User-Agent": "...",
      "V8-Version": "...",
      "WebKit-Version": "...",
      "webSocketDebuggerUrl": "...",
      "userDataDir": "/home/user/.config/google-chrome/MyProfile"
    }

    и сравнить с тем, какой предполагается для данного порта. Если отличается - значит, произошла коллизия по порту.

    Тогда рапортуем и выходим
*/

inline
int generatePortNumberForProjectConnection(std::string projectPath, int startPort=9000, int range=1000)
{
    umba::filename::stripLastPathSep(projectPath);

    std::size_t h = std::hash<std::string>{}(projectPath);

    return startPort + int(h%std::size_t(range));
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
std::string generateConnectionBaseUrlString(int port, bool http=false)
{
    using namespace std;

    // ws://localhost:9222
    // http://localhost:9222/json/list
    return std::string(http ? "http" : "ws") + "://localhost:" + to_string(port);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
std::vector<std::string> generateArgsForSpawnChromeExactDirs( const std::string &userDataDir
                                                            , const std::string &diskCacheDir
                                                            , int port
                                                            )
{
    using namespace std;

    std::vector<std::string> argsVec;

    argsVec.push_back("--new-window"); // "--new-tab"
    argsVec.push_back("--remote-debugging-port=" + to_string(port));
    argsVec.push_back("--user-data-dir="  + userDataDir);
    argsVec.push_back("--disk-cache-dir=" + diskCacheDir);
    //argsVec.push_back("" + );

    return argsVec;
}

//--------------------------------------------------------------------------------------------------------------------
inline
std::vector<std::string> generateArgsForSpawnChrome( const std::string &projectPath
                                                   , std::string browserName=std::string()
                                                   , const std::string &projectSubfolder=std::string()
                                                   , int port=-1, int startPort=9000, int range=1000
                                                   )
{
    if (port==-1)
    {
        port = generatePortNumberForProjectConnection(projectPath, startPort, range);
    }

    if (browserName.empty())
        browserName = "chrome";

    return generateArgsForSpawnChromeExactDirs( generateChromeUserProfileFolderForProject(projectPath, browserName, projectSubfolder )
                                              , generateChromeUserCacheFolderForProject(projectPath, browserName, projectSubfolder )
                                              , port
                                              );
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// enum class HttpErrorCode : int
// {
//     Ok                    = 0,
//     CannotConnect         = 1,
//     Timeout               = 2,
//     Gzip                  = 3,
//     UrlMalformed          = 4,
//     CannotCreateSocket    = 5,
//     SendError             = 6,
//     ReadError             = 7,
//     CannotReadStatusLine  = 8,
//     MissingStatus         = 9,
//     HeaderParsingError    = 10,
//     MissingLocation       = 11,
//     TooManyRedirects      = 12,
//     ChunkReadError        = 13,
//     CannotReadBody        = 14,
//     Cancelled             = 15,
//     Invalid               = 100
// };


inline
std::string ixHttpErrorCodeToString(ix::HttpErrorCode c)
{
    switch(c)
    {
        case ix::HttpErrorCode::Ok                  : return "Ok";
        case ix::HttpErrorCode::CannotConnect       : return "CannotConnect";
        case ix::HttpErrorCode::Timeout             : return "Timeout";
        case ix::HttpErrorCode::Gzip                : return "Gzip";
        case ix::HttpErrorCode::UrlMalformed        : return "UrlMalformed";
        case ix::HttpErrorCode::CannotCreateSocket  : return "CannotCreateSocket";
        case ix::HttpErrorCode::SendError           : return "SendError";
        case ix::HttpErrorCode::ReadError           : return "ReadError";
        case ix::HttpErrorCode::CannotReadStatusLine: return "CannotReadStatusLine";
        case ix::HttpErrorCode::MissingStatus       : return "MissingStatus";
        case ix::HttpErrorCode::HeaderParsingError  : return "HeaderParsingError";
        case ix::HttpErrorCode::MissingLocation     : return "MissingLocation";
        case ix::HttpErrorCode::TooManyRedirects    : return "TooManyRedirects";
        case ix::HttpErrorCode::ChunkReadError      : return "ChunkReadError";
        case ix::HttpErrorCode::CannotReadBody      : return "CannotReadBody";
        case ix::HttpErrorCode::Cancelled           : return "Cancelled";
        case ix::HttpErrorCode::Invalid             : return "Invalid";
        default: return "<UNKNOWN>";
    }
}

//--------------------------------------------------------------------------------------------------------------------
inline
bool ixHttpErrorCodeIsOk(ix::HttpErrorCode c)
{
    return c==ix::HttpErrorCode::Ok;
}

//--------------------------------------------------------------------------------------------------------------------
inline
bool ixHttpErrorCodeCantConnect(ix::HttpErrorCode c)
{
    return c==ix::HttpErrorCode::CannotConnect // отлуп соединения - никто не слушает
        || c==ix::HttpErrorCode::Timeout       // ожидание превышено - сокет видимо слушают, но сервак тупит
         ;
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpRequestArgsPtr ixHttpRequestArgsSetTimeouts(ix::HttpRequestArgsPtr pArgs, Timeouts timeouts)
{
    if (timeouts.connectTimeout>=0)
        pArgs->connectTimeout  = timeouts.connectTimeout;

    if (timeouts.transferTimeout>=0)
        pArgs->transferTimeout = timeouts.transferTimeout;

    return pArgs;
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// struct HttpRequestArgs
// {
//     std::string url;
//     std::string verb;
//     WebSocketHttpHeaders extraHeaders;
//     std::string body;
//     std::string multipartBoundary;
//     int connectTimeout = 60;
//     int transferTimeout = 1800;
//     bool followRedirects = true;
//     int maxRedirects = 5;
//     bool verbose = false;
//     bool compress = true;
//     bool compressRequest = false;
//     Logger logger;
//     OnProgressCallback onProgressCallback;
//     OnChunkCallback onChunkCallback;
//     std::atomic<bool> cancel;
// };

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpRequestArgsPtr makeRequestArgs(const std::string& url, Timeouts timeouts)
{
    ix::HttpClient httpClient;
    return ixHttpRequestArgsSetTimeouts(httpClient.createRequest(url), timeouts);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpRequestArgsPtr makeRequestArgs(const std::string& url)
{
    return makeRequestArgs(url, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(ix::HttpRequestArgsPtr pArgs, Timeouts timeouts)
{
    ix::HttpClient httpClient;
    ixHttpRequestArgsSetTimeouts(pArgs, timeouts);
    return httpClient.get(pArgs->url, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(ix::HttpRequestArgsPtr pArgs)
{
    return httpGet(pArgs, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(const std::string& url, Timeouts timeouts)
{
    auto pArgs = makeRequestArgs(url, timeouts);
    ix::HttpClient httpClient;
    return httpClient.get(pArgs->url, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(const std::string& url)
{
    return httpGet(url, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPut(ix::HttpRequestArgsPtr pArgs, const std::string body, Timeouts timeouts)
{
    ix::HttpClient httpClient;
    ixHttpRequestArgsSetTimeouts(pArgs, timeouts);
    return httpClient.put(pArgs->url, body, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPut(ix::HttpRequestArgsPtr pArgs, const std::string body=std::string())
{
    return httpPut(pArgs, body, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonList(const std::string &httpBaseUrl, Timeouts timeouts)
{
    auto jsonListUrl = httpBaseUrl + "/json/list";
    return httpGet(jsonListUrl, timeouts);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonList(const std::string &httpBaseUrl)
{
    return httpGetJsonList(httpBaseUrl, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr runConnectAndGetJsonList( const std::string              &httpBaseUrl // + "/json/list";
                                            , const std::string              &chromeName  // "chrome"
                                            , const std::vector<std::string> &spawnArgs
                                            , Timeouts                        probeTimeouts
                                            , Timeouts                        connectTimeouts
                                            )
{
    //auto jsonListUrl = httpBaseUrl + "/json/list";
    auto response = httpGetJsonList(httpBaseUrl, probeTimeouts);

    if (ixHttpErrorCodeIsOk(response->errorCode))
        return response;

    if (!ixHttpErrorCodeIsOk(response->errorCode) && !ixHttpErrorCodeCantConnect(response->errorCode))
        return response; // Something goes wrong while connecting to browser

    std::vector<std::string> foundExes;
    umba::shellapi::findExecutable(chromeName, foundExes);

    if (foundExes.empty())
        return response;

    for(auto chromeExeFullName : foundExes)
    {
        auto spawnRes = umba::shellapi::spawnProcess( chromeExeFullName
                                                    , spawnArgs
                                                    , umba::shellapi::SpawnProcessFlags::default_ // | umba::shellapi::SpawnProcessFlags::argvFromCmd
                                                    );
        if (spawnRes==std::uintptr_t(-1))
            continue;

        std::size_t nConnectTry = 0;

        do
        {
            //cout << "Connect try: " << nConnectTry << "\n";
            response = httpGetJsonList(httpBaseUrl, connectTimeouts); //umba::cdp::utils::httpGet(jsonListUrl, connectTimeouts);
            if (ixHttpErrorCodeIsOk(response->errorCode))
                return response;

            ++nConnectTry;

        } while(nConnectTry<=10);
    
    }

    return response;
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonVersion(const std::string &httpBaseUrl, Timeouts timeouts)
{
    auto jsonListUrl = httpBaseUrl + "/json/version";
    return httpGet(jsonListUrl, timeouts);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonVersion(const std::string &httpBaseUrl)
{
    return httpGetJsonVersion(httpBaseUrl, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPutJsonNewPage(const std::string &httpBaseUrl, const std::string &newPageUrl, Timeouts timeouts)
{
    auto jsonNewPageUrl = httpBaseUrl + "/json/new";
    jsonNewPageUrl += "?" + marty::url::urlEncodeComponent(newPageUrl);
    //ix::HttpRequestArgsPtr 
    auto pArgs = makeRequestArgs(jsonNewPageUrl, timeouts);
    return httpPut(pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPutJsonNewPage(const std::string &httpBaseUrl, const std::string &newPageUrl)
{
    return httpPutJsonNewPage(httpBaseUrl, newPageUrl, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpClosePage(const std::string &httpBaseUrl, const std::string &pageId, Timeouts timeouts)
{
    auto jsonClosePageUrl = httpBaseUrl + "/json/close/";
    jsonClosePageUrl += pageId; // marty::url::urlEncodeComponent(newPageUrl);
    auto pArgs = makeRequestArgs(jsonClosePageUrl, timeouts);

    ix::HttpClient httpClient;
    return httpClient.request( pArgs->url
                             , ix::HttpClient::kDelete
                             , ""
                             , pArgs
                             );
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpClosePage(const std::string &httpBaseUrl, const std::string &pageId)
{
    return httpClosePage(httpBaseUrl, pageId, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpActivatePage(const std::string &httpBaseUrl, const std::string &pageId, Timeouts timeouts)
{
    auto jsonClosePageUrl = httpBaseUrl + "/json/activate/";
    jsonClosePageUrl += pageId; // marty::url::urlEncodeComponent(newPageUrl);
    auto pArgs = makeRequestArgs(jsonClosePageUrl, timeouts);

    ix::HttpClient httpClient;
    return httpClient.request( pArgs->url
                             , ix::HttpClient::kDelete
                             , ""
                             , pArgs
                             );
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpActivatePage(const std::string &httpBaseUrl, const std::string &pageId)
{
    return httpActivatePage(httpBaseUrl, pageId, Timeouts{-1,-1});
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------

} // namespace utils
} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cbp/utils.h"
// marty::chrome_devtools_protocol::utils::
// marty::cdp::utils::

//--------------------------------------------------------------------------------------------------------------------

