/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief AtomicBoolIdHandler

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "Connection.h"
#include "MessageIdVariant.h"
#include "WebSocketMessage.h"
//
#include "types.h"
#include "defs.h"
//
#include <nlohmann/json.hpp>

//
#include <atomic>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/AtomicBoolIdHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct AtomicBoolIdHandler
{
    std::atomic<bool>   &atomicBool;

    void operator()(marty::cdp::Connection */* pCon */, const marty::cdp::WebSocketMessage& /* msg */, marty::cdp::MessageIdVariant idVariant, marty::cdp::json j)
    {
        //try
        //{
            // Просто убедиться, что там ID лежит, и всё корректно
            // Иначе вылетит исключение
            auto id = std::get<unsigned>(idVariant); 
            MARTY_CDP_USED(id);

            atomicBool = true;
     
        //}
        //catch(...)
        //{}
    }

}; // struct AtomicBoolIdHandler



//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/AtomicBoolIdHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdp::
