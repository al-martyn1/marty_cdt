/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Utils for marty_cdt

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "types.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"
// #include "umba/sleep.h"
//
#include "marty_url/percent_encoding.h"
//
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>

//
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/utils.h"
// marty::chrome_devtools_protocol::utils::
// marty::cdt::utils::

namespace marty {
namespace chrome_devtools_protocol {
namespace utils {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
time_point_type getSteadyClockNow()
{
    return std::chrono::steady_clock::now();
    // std::chrono::duration_cast<std::chrono::milliseconds>();
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
template<typename IteratorType, typename StringifierType>
std::string mergeValsToString(IteratorType b, IteratorType e, std::string sepStr, std::string lastSepStr, StringifierType stringifier)
{
    auto size = std::distance(b, e);

    using SizeType = std::decay_t<decltype(size)>;

    if (size==0)
        return std::string();

    if (size==1)
        return stringifier(*b);

    if (sepStr.empty())
        sepStr = ", ";
    else if (sepStr.back()!=' ')
        sepStr.append(1, ' ');

    if (lastSepStr.empty())
        lastSepStr = sepStr;
    else
    {
        if (lastSepStr.front()!=' ')
            lastSepStr = ' ' + lastSepStr;
        if (lastSepStr.back()!=' ')
            lastSepStr.append(1, ' ');
    }

    std::string resStr;

    SizeType nLast = size-1;
    SizeType i = 0;

    for(; b!=e; ++b, ++i)
    {
        if (!i)
        {
            resStr = stringifier(*b);
        }
        else
        {
            if (i>=nLast)
                resStr.append(lastSepStr);
            else
                resStr.append(sepStr);

            resStr.append(stringifier(*b));
        }
    }

    return resStr;
}

//--------------------------------------------------------------------------------------------------------------------
template<typename IteratorType>
std::string mergeValsToString(IteratorType b, IteratorType e, std::string sepStr=",", std::string lastSepStr="or", const std::string quotStr="\'")
{
    return mergeValsToString(b, e, sepStr, lastSepStr, [&](auto s) { return quotStr + s + quotStr; });
}
//--------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
char digit2hex(unsigned d, bool bUpper=true)
{
    if (d<10)
        return char('0' + d);
    return char((bUpper?'A':'a') + d-10);
}

inline
std::string toHexString(std::uint8_t b, bool bUpper=true)
{
    std::string resStr;
    resStr.append(1,digit2hex(unsigned(b>>4)&0x0Fu, bUpper));
    resStr.append(1,digit2hex(unsigned(b   )&0x0Fu, bUpper));
    return resStr;
}

inline
std::string toHexString(std::uint16_t w, bool bUpper=true)
{
    return toHexString(std::uint8_t(w>>8), bUpper) + toHexString(std::uint8_t(w), bUpper);
}

inline
std::string toHexString(std::uint32_t dw, bool bUpper=true)
{
    return toHexString(std::uint16_t(dw>>16), bUpper) + toHexString(std::uint16_t(dw), bUpper);
}

inline
std::string toHexString(std::uint64_t qw, bool bUpper=true)
{
    return toHexString(std::uint32_t(qw>>32), bUpper) + toHexString(std::uint32_t(qw), bUpper);
}

inline
std::string toHexString(const std::vector<std::uint8_t> &v, bool bUpper=true)
{
    std::string resStr;

    for(auto b: v)
    {
        if (!resStr.empty())
            resStr.append(1, ' ');
        resStr.append(toHexString(b, bUpper));
    }

    return resStr;
}

//----------------------------------------------------------------------------
inline
std::string toHexDump(std::size_t w, const std::uint8_t *pData, std::size_t dataSz, const std::string &breakLine="\n", bool bUpper=true, char fillCh=' ')
{
    std::string resStr; resStr.reserve(dataSz*3u);

    std::size_t i=0;
    for(; i!=dataSz; ++i)
    {
        if ((i%w)==0 && i!=0)
        {
            // out << " // 0x" << utils::toHexString(std::uint16_t(i-16)) << " / " << i-16;
        }
        resStr.append(((i%w)==0 ? breakLine : std::string(1, fillCh)));
        // out << ((i%16)==0 ? "\n" : "");
        // //out << (i==0 ? "{ " : ", ");
        // out << "0x" << utils::toHexString(metadata[i]); // cli::utils::numswr((unsigned)metadata[i], 3);
        resStr.append(toHexString(pData[i], bUpper));

    }

    resStr.append(((i%w)==0 ? breakLine : std::string(0, fillCh)));
    //out << ((i%16)==0 ? "\n" : "");

    return resStr;
}

//----------------------------------------------------------------------------
inline
std::string toHexDump(std::size_t w, const std::vector<std::uint8_t> &data, const std::string &breakLine="\n", bool bUpper=true, char fillCh=' ')
{
    if (data.empty())
        return std::string();

    return toHexDump(w, &data[0], data.size(), breakLine, bUpper, fillCh);
}

//----------------------------------------------------------------------------
inline
std::string toHexDump(std::size_t w, const std::string &data, const std::string &breakLine="\n", bool bUpper=true, char fillCh=' ')
{
    if (data.empty())
        return std::string();

    return toHexDump(w, (const std::uint8_t*)data.data(), data.size(), breakLine, bUpper, fillCh);
}

//----------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
std::vector<std::string> generatePatterns(const std::string& input, char delimiter = '.')
{
    if (input.empty())
    {
        return {"*"};
    }

    // Разбиваем строку на части
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = input.find(delimiter);
    while (end != std::string::npos)
    {
        parts.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(delimiter, start);
    }
    parts.push_back(input.substr(start));

    std::vector<std::string> result;
    result.reserve(parts.size() + 1);

    // Полная строка (оригинал)
    result.push_back(input);

    // Добавляем шаблоны с заменой суффикса на '*'
    // Идём от большего числа частей к меньшему
    for (size_t i = parts.size(); i > 1; --i)
    {
        std::string prefix;
        for (size_t j = 0; j < i - 1; ++j)
        {
            if (j > 0) prefix += delimiter;
            prefix += parts[j];
        }
        prefix += delimiter;
        prefix += '*';
        result.push_back(prefix);
    }

    // Завершающий шаблон — просто "*"
    result.push_back("*");

    return result;
}

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
    // ws://localhost:9222
    // http://localhost:9222/json/list
    return std::string(http ? "http" : "ws") + "://localhost:" + std::to_string(port);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
std::vector<std::string> generateArgsForSpawnChromeExactDirs( const std::string &userDataDir
                                                            , const std::string &diskCacheDir
                                                            , int port
                                                            , bool restoreLastSession=false // в настройках не должно быть настроено восстановление, так как мы можем принудительно восстановить, но принудительно отключить восстановление не получится
                                                            )
{
    std::vector<std::string> argsVec;

    std::string schemeSepLocalhostPort = "://localhost:" + std::to_string(port);

    argsVec.push_back("--new-window"); // "--new-tab"
    argsVec.push_back("--remote-debugging-port=" + std::to_string(port));
    argsVec.push_back("--remote-allow-origins=http" + schemeSepLocalhostPort + ",ws" + schemeSepLocalhostPort + ",wss" + schemeSepLocalhostPort);
    argsVec.push_back("--user-data-dir="  + userDataDir);
    argsVec.push_back("--disk-cache-dir=" + diskCacheDir);
    if (restoreLastSession)
        argsVec.push_back("--restore-last-session");

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
// enum class WebSocketMessageType
// {
//     Message  = 0,
//     Open     = 1,
//     Close    = 2,
//     Error    = 3,
//     Ping     = 4,
//     Pong     = 5,
//     Fragment = 6
// };

inline
std::string ixWebSocketMessageTypeToString(ix::WebSocketMessageType t)
{
    switch(t)
    {
        case ix::WebSocketMessageType::Message  : return "Message";
        case ix::WebSocketMessageType::Open     : return "Open";
        case ix::WebSocketMessageType::Close    : return "Close";
        case ix::WebSocketMessageType::Error    : return "Error";
        case ix::WebSocketMessageType::Ping     : return "Ping";
        case ix::WebSocketMessageType::Pong     : return "Pong";
        case ix::WebSocketMessageType::Fragment : return "Fragment";
        default: return "<UNKNOWN>";
    }
}
//--------------------------------------------------------------------------------------------------------------------




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
ix::HttpRequestArgsPtr makeRequestArgs(const std::string& url, Timeouts timeouts=Timeouts{-1,-1})
{
    ix::HttpClient httpClient;
    return ixHttpRequestArgsSetTimeouts(httpClient.createRequest(url), timeouts);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(ix::HttpRequestArgsPtr pArgs, Timeouts timeouts=Timeouts{-1,-1})
{
    ix::HttpClient httpClient;
    ixHttpRequestArgsSetTimeouts(pArgs, timeouts);
    return httpClient.get(pArgs->url, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGet(const std::string& url, Timeouts timeouts=Timeouts{-1,-1})
{
    auto pArgs = makeRequestArgs(url, timeouts);
    ix::HttpClient httpClient;
    return httpClient.get(pArgs->url, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPut(ix::HttpRequestArgsPtr pArgs, const std::string body=std::string(), Timeouts timeouts=Timeouts{-1,-1})
{
    ix::HttpClient httpClient;
    ixHttpRequestArgsSetTimeouts(pArgs, timeouts);
    return httpClient.put(pArgs->url, body, pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPut(ix::HttpRequestArgsPtr pArgs, Timeouts timeouts)
{
    return httpPut(pArgs, std::string(), timeouts);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonList(const std::string &httpBaseUrl, Timeouts timeouts=Timeouts{-1,-1})
{
    auto jsonListUrl = httpBaseUrl + "/json/list";
    return httpGet(jsonListUrl, timeouts);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr runConnectAndGetJsonList( const std::string              &httpBaseUrl // + "/json/list";
                                            , const std::string              &chromeName  // "chrome"
                                            , const std::vector<std::string> &spawnArgs
                                            , Timeouts                       probeTimeouts
                                            , Timeouts                       connectTimeouts
                                            , std::size_t                    nConnectTryMax=10
                                            , unsigned                       connectSleepMsTimeout=10
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

    if (!nConnectTryMax)
        nConnectTryMax = 10;

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
            response = httpGetJsonList(httpBaseUrl, connectTimeouts); //umba::cdt::utils::httpGet(jsonListUrl, connectTimeouts);
            if (ixHttpErrorCodeIsOk(response->errorCode))
                return response;

            ++nConnectTry;

            std::this_thread::sleep_for(std::chrono::milliseconds(connectSleepMsTimeout));
            // umba::sleepMs(connectSleepMsTimeout);

            connectSleepMsTimeout *= 2;

        } while(nConnectTry!=nConnectTryMax);
    
    }

    return response;
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpGetJsonVersion(const std::string &httpBaseUrl, Timeouts timeouts=Timeouts{-1,-1})
{
    auto jsonListUrl = httpBaseUrl + "/json/version";
    return httpGet(jsonListUrl, timeouts);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPutJsonNewPage(const std::string &httpBaseUrl, const std::string &newPageUrl, Timeouts timeouts=Timeouts{-1,-1})
{
    auto jsonNewPageUrl = httpBaseUrl + "/json/new";
    if (!newPageUrl.empty())
        jsonNewPageUrl += "?" + marty::url::urlEncodeComponent(newPageUrl);

    auto pArgs = makeRequestArgs(jsonNewPageUrl, timeouts);
    return httpPut(pArgs);
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpPutJsonNewPage(const std::string &httpBaseUrl, Timeouts timeouts=Timeouts{-1,-1})
{
    return httpPutJsonNewPage(httpBaseUrl, std::string(), timeouts);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
inline
ix::HttpResponsePtr httpClosePage(const std::string &httpBaseUrl, const std::string &pageId, Timeouts timeouts)
{
    auto jsonClosePageUrl = httpBaseUrl + "/json/close/";
    jsonClosePageUrl += marty::url::urlEncodeComponent(pageId);
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
    jsonClosePageUrl += marty::url::urlEncodeComponent(pageId);
    auto pArgs = makeRequestArgs(jsonClosePageUrl, timeouts);

    ix::HttpClient httpClient;
    return httpClient.request( pArgs->url
                             , ix::HttpClient::kGet
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
// ix::WebSocketInitResult
//   bool success;
//   int http_status;
//   std::string errorStr;
//   WebSocketHttpHeaders headers;
//   std::string uri;
//   std::string protocol;
inline
ix::WebSocketInitResult wsConnect(ix::WebSocket& webSocket, const std::string& url, int timeoutSec = 3)
{
    webSocket.setUrl(url);
    return webSocket.connect(timeoutSec);
}

inline
ix::WebSocketInitResult wsConnect(ix::WebSocket& webSocket, const std::string& url, std::function<void(const ix::WebSocketMessagePtr&)> msgCallback, int timeoutSec = 3)
{
    webSocket.setUrl(url);
    webSocket.setOnMessageCallback(msgCallback);
    return webSocket.connect(timeoutSec);
}

// void setOnMessageCallback(const OnMessageCallback& callback);
// static void setTrafficTrackerCallback(const OnTrafficTrackerCallback& callback);
// using OnMessageCallback = std::function<void(const WebSocketMessagePtr&)>;
// using OnTrafficTrackerCallback = std::function<void(size_t size, bool incoming)>;

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// namespace ix
// {
//     struct WebSocketSendInfo
//     {
//         bool success;
//         bool compressionError;
//         size_t payloadSize;
//         size_t wireSize;

        // WebSocketSendInfo send(const std::string& data,
        //                        bool binary = false,
        //                        const OnProgressCallback& onProgressCallback = nullptr);
        // WebSocketSendInfo sendBinary(const std::string& data,
        //                              const OnProgressCallback& onProgressCallback = nullptr);
        // WebSocketSendInfo sendBinary(const IXWebSocketSendData& data,
        //                              const OnProgressCallback& onProgressCallback = nullptr);
        // // does not check for valid UTF-8 characters. Caller must check that.
        // WebSocketSendInfo sendUtf8Text(const std::string& text,
        //                                const OnProgressCallback& onProgressCallback = nullptr);
        // // does not check for valid UTF-8 characters. Caller must check that.
        // WebSocketSendInfo sendUtf8Text(const IXWebSocketSendData& text,
        //                                const OnProgressCallback& onProgressCallback = nullptr);
        // WebSocketSendInfo sendText(const std::string& text,
        //                            const OnProgressCallback& onProgressCallback = nullptr);

// https://chromedevtools.github.io/devtools-protocol/
//--------------------------------------------------------------------------------------------------------------------
// Domains
//  
// E Accessibility Domain         - https://chromedevtools.github.io/devtools-protocol/tot/Accessibility/
// E Animation Domain             - https://chromedevtools.github.io/devtools-protocol/tot/Animation/
// E Audits Domain                - https://chromedevtools.github.io/devtools-protocol/tot/Audits/
// E Autofill Domain              - https://chromedevtools.github.io/devtools-protocol/tot/Autofill/
// E BackgroundService Domain     - https://chromedevtools.github.io/devtools-protocol/tot/BackgroundService/
// E BluetoothEmulation Domain    - https://chromedevtools.github.io/devtools-protocol/tot/BluetoothEmulation/
//   Browser Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Browser/
// E CacheStorage Domain          - https://chromedevtools.github.io/devtools-protocol/tot/CacheStorage/
// E Cast Domain                  - https://chromedevtools.github.io/devtools-protocol/tot/Cast/
// D Console Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Console/
// E CrashReportContext Domain    - https://chromedevtools.github.io/devtools-protocol/tot/CrashReportContext/
// E CSS Domain                   - https://chromedevtools.github.io/devtools-protocol/tot/CSS/
//   Debugger Domain              - https://chromedevtools.github.io/devtools-protocol/tot/Debugger/
// E DeviceAccess Domain          - https://chromedevtools.github.io/devtools-protocol/tot/DeviceAccess/
// E DeviceOrientation Domain     - https://chromedevtools.github.io/devtools-protocol/tot/DeviceOrientation/
//   DOM Domain                   - https://chromedevtools.github.io/devtools-protocol/tot/DOM/
//   DOMDebugger Domain           - https://chromedevtools.github.io/devtools-protocol/tot/DOMDebugger/
// E DOMSnapshot Domain           - https://chromedevtools.github.io/devtools-protocol/tot/DOMSnapshot/
// E DOMStorage Domain            - https://chromedevtools.github.io/devtools-protocol/tot/DOMStorage/
// E DOMStorage Domain            - https://chromedevtools.github.io/devtools-protocol/tot/DOMStorage/
//   Emulation Domain             - https://chromedevtools.github.io/devtools-protocol/tot/Emulation/
// E EventBreakpoints Domain      - https://chromedevtools.github.io/devtools-protocol/tot/EventBreakpoints/
// E Extensions Domain            - https://chromedevtools.github.io/devtools-protocol/tot/Extensions/
// E FedCm Domain                 - https://chromedevtools.github.io/devtools-protocol/tot/FedCm/
//   Fetch Domain                 - https://chromedevtools.github.io/devtools-protocol/tot/Fetch/
// E FileSystem Domain            - https://chromedevtools.github.io/devtools-protocol/tot/FileSystem/
// E HeadlessExperimental Domain  - https://chromedevtools.github.io/devtools-protocol/tot/HeadlessExperimental/
// E HeapProfiler Domain          - https://chromedevtools.github.io/devtools-protocol/tot/HeapProfiler/
// E IndexedDB Domain             - https://chromedevtools.github.io/devtools-protocol/tot/IndexedDB/
//   Input Domain                 - https://chromedevtools.github.io/devtools-protocol/tot/Input/
// E Inspector Domain             - https://chromedevtools.github.io/devtools-protocol/tot/Inspector/
//   IO Domain                    - https://chromedevtools.github.io/devtools-protocol/tot/IO/
// E LayerTree Domain             - https://chromedevtools.github.io/devtools-protocol/tot/LayerTree/
//   Log Domain                   - https://chromedevtools.github.io/devtools-protocol/tot/Log/
// E Media Domain                 - https://chromedevtools.github.io/devtools-protocol/tot/Media/
// E Memory Domain                - https://chromedevtools.github.io/devtools-protocol/tot/Memory/
//   Network Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Network/
// E Overlay Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Overlay/
//   Page Domain                  - https://chromedevtools.github.io/devtools-protocol/tot/Page/
//   Performance Domain           - https://chromedevtools.github.io/devtools-protocol/tot/Performance/
// E PerformanceTimeline Domain   - https://chromedevtools.github.io/devtools-protocol/tot/PerformanceTimeline/
// E Preload Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Preload/
//   Profiler Domain              - https://chromedevtools.github.io/devtools-protocol/tot/Profiler/
// E PWA Domain                   - https://chromedevtools.github.io/devtools-protocol/tot/PWA/
//   Runtime Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Runtime/
// D Schema Domain                - https://chromedevtools.github.io/devtools-protocol/tot/Schema/
//   Security Domain              - https://chromedevtools.github.io/devtools-protocol/tot/Security/
// E ServiceWorker Domain         - https://chromedevtools.github.io/devtools-protocol/tot/ServiceWorker/
// E SmartCardEmulation Domain    - https://chromedevtools.github.io/devtools-protocol/tot/SmartCardEmulation/
// E Storage Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Storage/
// E SystemInfo Domain            - https://chromedevtools.github.io/devtools-protocol/tot/SystemInfo/
//   Target Domain                - https://chromedevtools.github.io/devtools-protocol/tot/Target/
// E Tethering Domain             - https://chromedevtools.github.io/devtools-protocol/tot/Tethering/
//   Tracing Domain               - https://chromedevtools.github.io/devtools-protocol/tot/Tracing/
// E WebAudio Domain              - https://chromedevtools.github.io/devtools-protocol/tot/WebAudio/
// E WebAuthn Domain              - https://chromedevtools.github.io/devtools-protocol/tot/WebAuthn/
// E WebMCP Domain                - https://chromedevtools.github.io/devtools-protocol/tot/WebMCP/

// Домен    Основные события
// Page     Page.domContentEventFired — DOM готов
//          Page.loadEventFired — страница загружена
//          Page.frameNavigated — началась навигация
// Network  Network.requestWillBeSent — запрос отправлен
//          Network.responseReceived — ответ получен
// Runtime  Runtime.consoleAPICalled — вызов console.log
//          Runtime.exceptionThrown — необработанное исключение
// DOM      DOM.documentUpdated — DOM обновлён
//          DOM.attributeModified — атрибут изменён

// Полный JSON-файл со схемой всех доменов, команд и событий через HTTP-эндпоинт: curl http://localhost:9810/json/protocol

//--------------------------------------------------------------------------------------------------------------------
inline
ix::WebSocketSendInfo wsSendEventSubscription(ix::WebSocket& webSocket, unsigned cmdId, const std::string &subscriptionName)
{
    json j = { {"id", cmdId}
             , {"method", subscriptionName}
             };
    return webSocket.sendText(j.dump());
}

//--------------------------------------------------------------------------------------------------------------------
inline
ix::WebSocketSendInfo wsSendCommand(ix::WebSocket& webSocket, unsigned cmdId, const std::string &method, json params)
{
    json j = { {"id", cmdId}
             , {"method", method}
             //  {"params", {{"url", "https://example.com"}}}
             };

    if (!params.is_null())
        j["params"] = params;

    return webSocket.sendText(j.dump());
}

//--------------------------------------------------------------------------------------------------------------------
// Browser.getWindowForTarget
// Browser.setWindowBounds
//  
// {
//   "id": 1,
//   "method": "Browser.getWindowForTarget"
// }
//  
// {
//   "id": 1,
//   "result": {
//     "windowId": 12345,
//     "bounds": { "left": 0, "top": 0, "width": 1280, "height": 720, "windowState": "normal" }
//   }
// }
//  
// {
//   "id": 2,
//   "method": "Browser.setWindowBounds",
//   "params": {
//     "windowId": 12345,
//     "bounds": {
//       "width": 1024,
//       "height": 768,
//       "windowState": "normal" // или "maximized", "minimized", "fullscreen"
//     }
//   }
// }





} // namespace utils
} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/utils.h"
// marty::chrome_devtools_protocol::utils::
// marty::cdt::utils::

//--------------------------------------------------------------------------------------------------------------------

