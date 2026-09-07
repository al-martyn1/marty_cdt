/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Types for marty_cdt

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
// #include "umba/filename.h"
// #include "umba/filesys.h"

//
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>
//
#include <nlohmann/json.hpp>

//
#include <chrono>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/types.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//----------------------------------------------------------------------------
using json = nlohmann::json;

//----------------------------------------------------------------------------
using time_point_type = std::chrono::steady_clock::time_point;

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



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateReturnType
{
    unspecified      = -1,
    objectId         = 0,
    returnByValue

}; // enum class RuntimEvaluateReturnType



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateAwaitPromise
{
    unspecified      = -1,
    dontAwaitPromise = 0,
    awaitPromise

}; // enum class RuntimEvaluateAwaitPromise



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateUserGesture
{
    unspecified      = -1,
    noUserGesture    = 0,
    userGesture

}; // enum class RuntimEvaluateUserGesture



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateThrowOnSideEffect
{
    unspecified      = -1,
    noThrowOnSideEffect = 0,
    throwOnSideEffect

}; // enum class RuntimEvaluateThrowOnSideEffect



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateBreaksControl
{
    unspecified      = -1,
    enableBreaks     = 0,
    disableBreaks

}; // enum class RuntimEvaluateBreaksControl



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateReplMode
{
    unspecified      = -1,
    noReplMode      = 0,
    replMode,

}; // enum class RuntimEvaluateReplMode



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimEvaluateCspMode // Content Security Policy
{
    unspecified      = -1,
    useCsp           = 0,
    allowUnsafeEvalBlockedByCSP,

}; // enum class RuntimEvaluateCspMode

//--------------------------------------------------------------------------------------------------------------------





//--------------------------------------------------------------------------------------------------------------------

} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/types.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------

// #include "AtomicBoolIdHandler.h"
// #include "AtomicBoolMethodHandler.h"
#include "Dom.h"
#include "ErrorInfo.h"
#include "JsonListResponse.h"
#include "JsonVersionResponse.h"
#include "MessageIdVariant.h"
#include "PageNavigateResponse.h"
#include "ResponseError.h"
#include "WebSocketMessage.h"




//--------------------------------------------------------------------------------------------------------------------




