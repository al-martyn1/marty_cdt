/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief PageNavigateResponse

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/JsonVersionResponse.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct PageNavigateResponse
{
    std::string    frameId;
    std::string    loaderId;
    bool           isDownload;

    void clear()
    {
        frameId.clear();
        loaderId.clear();
        isDownload = false;
    }

    // {
    //   "frameId": "B7FD8C637FE293E9375D3C54F9EA23AF",
    //   "isDownload": false,
    //   "loaderId": "2E177D49307BB34D5EE46B1937247AC7"
    // }


    // void to_json(json& j) const
    // {
    //     j = json::object();
    //  
    //     // if (!browser.empty())
    //        j["Browser"] = browser;
    //  
    //     // if (!protocolVersion.empty())
    //        j["Protocol-Version"] = protocolVersion;
    //  
    //     // if (!userAgent.empty())
    //        j["User-Agent"] = userAgent;
    //  
    //     // if (!v8Version.empty())
    //        j["V8-Version"] = v8Version;
    //  
    //     // if (!webkitVersion.empty())
    //        j["WebKit-Version"] = webkitVersion;
    //  
    //     // if (!webSocketDebuggerUrl.empty())
    //        j["webSocketDebuggerUrl"] = webSocketDebuggerUrl;
    //  
    //     if (!debuggerVersion.empty())
    //        j["Debugger-Version"] = debuggerVersion;
    //  
    // }

    void from_json(const json& j)
    {
        if (!j.is_object())
            throw std::runtime_error("PageNavigateResponse: object expected");
    
        clear();

        if (j.contains("frameId"))
            frameId = j["frameId"].get<std::string>();

        if (j.contains("loaderId"))
            loaderId = j["loaderId"].get<std::string>();

        if (j.contains("isDownload"))
            isDownload = j["isDownload"].get<bool>();

    }


}; // struct PageNavigateResponse

// inline
// void to_json(json& j, const PageNavigateResponse &r)
// {
//     r.to_json(j);
// }
 
inline
void from_json(const json& j, PageNavigateResponse &r)
{
    r.from_json(j);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/JsonVersionResponse.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
