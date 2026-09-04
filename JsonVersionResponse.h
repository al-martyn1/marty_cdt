/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief JsonVersionResponse

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
struct JsonVersionResponse
{
    std::string    browser;
    std::string    protocolVersion;
    std::string    userAgent;
    std::string    v8Version;
    std::string    webkitVersion;
    std::string    debuggerVersion;

    std::string    webSocketDebuggerUrl;

    void clear()
    {
        browser.clear();
        protocolVersion.clear();
        userAgent.clear();
        v8Version.clear();
        webkitVersion.clear();
        debuggerVersion.clear();

        webSocketDebuggerUrl.clear();
    }


// {
//    "Browser": "Chrome/151.0.7922.110",
//    "Protocol-Version": "1.3",
//    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/151.0.0.0 Safari/537.36",
//    "V8-Version": "15.1.206.16",
//    "WebKit-Version": "537.36 (@2ccadf9e6d760667310dc510c978ded23d99be67)",
//    "webSocketDebuggerUrl": "ws://localhost:9810/devtools/browser/22ec886c-fbe4-4987-aeb7-45c6505d514e"
// }

    void to_json(json& j) const
    {
        j = json::object();

        // if (!browser.empty())
           j["Browser"] = browser;

        // if (!protocolVersion.empty())
           j["Protocol-Version"] = protocolVersion;

        // if (!userAgent.empty())
           j["User-Agent"] = userAgent;

        // if (!v8Version.empty())
           j["V8-Version"] = v8Version;

        // if (!webkitVersion.empty())
           j["WebKit-Version"] = webkitVersion;

        // if (!webSocketDebuggerUrl.empty())
           j["webSocketDebuggerUrl"] = webSocketDebuggerUrl;

        if (!debuggerVersion.empty())
           j["Debugger-Version"] = debuggerVersion;

    }

    void from_json(const json& j)
    {
        if (!j.is_object())
            throw std::runtime_error("JsonVersionResponse: object expected");
    
        clear();

        if (j.contains("Browser"))
            browser = j["Browser"].get<std::string>();

        if (j.contains("Protocol-Version"))
            protocolVersion = j["Protocol-Version"].get<std::string>();

        if (j.contains("User-Agent"))
            userAgent = j["User-Agent"].get<std::string>();

        if (j.contains("V8-Version"))
            v8Version = j["V8-Version"].get<std::string>();

        if (j.contains("WebKit-Version"))
            webkitVersion = j["WebKit-Version"].get<std::string>();

        if (j.contains("webSocketDebuggerUrl"))
            webSocketDebuggerUrl = j["webSocketDebuggerUrl"].get<std::string>();

        if (j.contains("Debugger-Version"))
            debuggerVersion = j["Debugger-Version"].get<std::string>();

    }


}; // struct JsonVersionResponse

inline
void to_json(json& j, const JsonVersionResponse &r)
{
    r.to_json(j);
}
 
inline
void from_json(const json& j, JsonVersionResponse &r)
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
