/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief JsonListResponseEntry

    Repository: https://github.com/al-martyn1/marty_cbp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cbp/JsonListResponseEntry.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct JsonListResponseEntry
{
    std::string    id;
    std::string    type;                 // page, iframe, worker, service_worker, browser_ui, shared_worker, background_page, browser, webview, tab
                                         // maybe or deprecated: deprecation, javascript, network, recommendation, rendering, security
                                         // other: other - Используется как catch-all для целей, тип которых не попадает под другие категории.
                                         // catch-all?

    std::string    parentId;             // optional
    std::string    title;
    std::string    url;
    std::string    devtoolsFrontendUrl;
    std::string    webSocketDebuggerUrl;

    std::string    description;          // optional
    std::string    faviconUrl;           // optional
    std::string    thumbnailUrl;         // optional
    std::string    openerId;             // optional
    std::string    browserContextId;     // optional

}; // struct JsonListResponseEntry

using JsonNewPageResponse = JsonListResponseEntry;

// "description": "",
// "devtoolsFrontendUrl": "https://chrome-devtools-frontend.appspot.com/serve_rev/@2ccadf9e6d760667310dc510c978ded23d99be67/inspector.html?ws=localhost:9810/devtools/page/1F01554C04885EB8CB4094DA4D693879",
// "id": "1F01554C04885EB8CB4094DA4D693879",
// "title": "",
// "type": "page",
// "url": "chrome://settings/onStartup",
// "webSocketDebuggerUrl": "ws://localhost:9810/devtools/page/1F01554C04885EB8CB4094DA4D693879"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( JsonListResponseEntry
, id
, type
, parentId
, title
, url
, devtoolsFrontendUrl
, webSocketDebuggerUrl

, description
, faviconUrl
, thumbnailUrl
, openerId
, browserContextId
)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// struct JsonListResponse
// {
//     std::vector<JsonListResponseEntry>    list;
//  
// }; // struct JsonListResponse
//  
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( JsonListResponse
// , list
// )

//--------------------------------------------------------------------------------------------------------------------


} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cbp/JsonListResponseEntry.h"
// marty::chrome_devtools_protocol::
// marty::cdp::
