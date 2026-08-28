/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Types for marty_cbp

    Repository: https://github.com/al-martyn1/marty_cbp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "umba/filename.h"
#include "umba/filesys.h"

//
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>
//
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cbp/cbp/types.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

namespace marty {
namespace chrome_devtools_protocol {

//----------------------------------------------------------------------------
using json = nlohmann::json;

//----------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
//enum class BaseConnectResult



//--------------------------------------------------------------------------------------------------------------------
struct Timeouts
{
    int connectTimeout    = 1; // 1 секунда на подключение
    int transferTimeout   = 1; // 1 секунда на получение данных

    Timeouts& setConnectTimeout(int t)
    {
        connectTimeout = t;
        return *this;
    }

    Timeouts& setTransferTimeout(int t)
    {
        transferTimeout = t;
        return *this;
    }

    Timeouts getCopy() const 
    {
        return *this;
    }

}; // struct Timeouts





//--------------------------------------------------------------------------------------------------------------------

} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cbp/types.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
#include "JsonListResponse.h"
#include "JsonVersionResponse.h"

//--------------------------------------------------------------------------------------------------------------------




