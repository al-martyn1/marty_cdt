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
#include <iostream>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/AtomicBoolIdHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct AtomicBoolIdHandler
{
    std::atomic<bool>   &atomicBool;
    bool                printInfo = false;

    void operator()(marty::cdt::Connection *pCon, const marty::cdt::WebSocketMessage& /* msg */, marty::cdt::MessageIdVariant idVariant, marty::cdt::json j)
    {
        //try
        //{
            // Просто убедиться, что там ID лежит, и всё корректно
            // Иначе вылетит исключение
            auto id = std::get<unsigned>(idVariant); 
            MARTY_CDT_USED(id);

            atomicBool = true;

            MARTY_CDT_USED(j);
            if (printInfo)
            {
                std::cout << "timestamp: " << pCon->getTimestamp() << "\n";
                std::cout << "AtomicBoolIdHandler, ID: " << id << "\n";
                std::cout << j.dump(2) << "\n\n";
            }
     
        //}
        //catch(...)
        //{}
    }
    
    AtomicBoolIdHandler() = delete;
    AtomicBoolIdHandler(std::atomic<bool> &ab) : atomicBool(ab) {}
    AtomicBoolIdHandler(std::atomic<bool> &ab, bool p) : atomicBool(ab), printInfo(p) {}

    AtomicBoolIdHandler(const AtomicBoolIdHandler &) = default;
    AtomicBoolIdHandler& operator=(const AtomicBoolIdHandler &) = default;

    AtomicBoolIdHandler(AtomicBoolIdHandler &&) = default;
    AtomicBoolIdHandler& operator=(AtomicBoolIdHandler &) = default;

}; // struct AtomicBoolIdHandler

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/AtomicBoolIdHandler.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
