/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief WebSocketMessage - копия ix::WebSocketMessage, только поля все по значению

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
// #include "marty_cdt/WebSocketMessage.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct WebSocketMessage
{
    ix::WebSocketMessageType    type     ;
    std::string                 str      ;
    size_t                      wireSize ;
    ix::WebSocketErrorInfo      errorInfo;
    ix::WebSocketOpenInfo       openInfo ;
    ix::WebSocketCloseInfo      closeInfo;
    bool                        binary   ;


    WebSocketMessage() = default; // нужен для getQueuedMessage

    WebSocketMessage(const ix::WebSocketMessage &msg)
    : type     (msg.type     )
    , str      (msg.str      )
    , wireSize (msg.wireSize )
    , errorInfo(msg.errorInfo)
    , openInfo (msg.openInfo )
    , closeInfo(msg.closeInfo)
    , binary   (msg.binary   )
    {}

    WebSocketMessage(const WebSocketMessage&) = default;
    WebSocketMessage& operator=(const WebSocketMessage&) = default;

    WebSocketMessage(WebSocketMessage&&) noexcept = default;
    WebSocketMessage& operator=(WebSocketMessage&&) noexcept = default;

}; // struct WebSocketMessage

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/WebSocketMessage.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
