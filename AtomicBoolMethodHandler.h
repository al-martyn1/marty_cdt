/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief AtomicBoolMethodHandler

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
#include <iostream>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/AtomicBoolMethodHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct AtomicBoolMethodHandler
{
    std::atomic<bool>   &atomicBool;
    bool                printInfo = false;

    void operator()(marty::cdt::Connection *pCon, const marty::cdt::WebSocketMessage& /* msg */, marty::cdt::MessageIdVariant idVariant, marty::cdt::json j)
    {
        //try
        //{
            // Просто убедиться, что там ID лежит, и всё корректно
            // Иначе вылетит исключение
            auto method = std::get<std::string>(idVariant); 
            MARTY_CDT_USED(method);

            atomicBool = true;
     
            MARTY_CDT_USED(j);
            if (printInfo)
            {
                std::cout << "timestamp: " << pCon->getTimestamp() << "\n";
                std::cout << "AtomicBoolMethodHandler, Method: " << method << "\n";
                std::cout << j.dump(2) << "\n\n";
            }

            // {
            //   "timestamp": 1968417.706551
            // }

        //}
        //catch(...)
        //{}
    }
    
    AtomicBoolMethodHandler() = delete;
    AtomicBoolMethodHandler(std::atomic<bool> &ab) : atomicBool(ab) {}
    AtomicBoolMethodHandler(std::atomic<bool> &ab, bool p) : atomicBool(ab), printInfo(p) {}

    AtomicBoolMethodHandler(const AtomicBoolMethodHandler &) = default;
    AtomicBoolMethodHandler& operator=(const AtomicBoolMethodHandler &) = default;

    AtomicBoolMethodHandler(AtomicBoolMethodHandler &&) = default;
    AtomicBoolMethodHandler& operator=(AtomicBoolMethodHandler &) = default;

}; // struct AtomicBoolMethodHandler

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/AtomicBoolMethodHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
