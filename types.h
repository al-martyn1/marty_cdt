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
enum class RuntimeEvaluateReturnType
{
    unspecified      = -1,
    objectId         = 0,
    returnByValue

}; // enum class RuntimeEvaluateReturnType

inline
std::string to_string(RuntimeEvaluateReturnType v)
{
    return v==RuntimeEvaluateReturnType::unspecified ? "unspecified" : (v==RuntimeEvaluateReturnType::objectId ? "objectId" : "returnByValue");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateAwaitPromise
{
    unspecified      = -1,
    dontAwaitPromise = 0,
    awaitPromise

}; // enum class RuntimeEvaluateAwaitPromise

inline
std::string to_string(RuntimeEvaluateAwaitPromise v)
{
    return v==RuntimeEvaluateAwaitPromise::unspecified ? "unspecified" : (v==RuntimeEvaluateAwaitPromise::dontAwaitPromise ? "dontAwaitPromise" : "awaitPromise");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateUserGesture
{
    unspecified      = -1,
    noUserGesture    = 0,
    userGesture

}; // enum class RuntimeEvaluateUserGesture

inline
std::string to_string(RuntimeEvaluateUserGesture v)
{
    return v==RuntimeEvaluateUserGesture::unspecified ? "unspecified" : (v==RuntimeEvaluateUserGesture::noUserGesture ? "noUserGesture" : "userGesture");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateThrowOnSideEffect
{
    unspecified         = -1,
    noThrowOnSideEffect = 0,
    throwOnSideEffect

}; // enum class RuntimeEvaluateThrowOnSideEffect

inline
std::string to_string(RuntimeEvaluateThrowOnSideEffect v)
{
    return v==RuntimeEvaluateThrowOnSideEffect::unspecified ? "unspecified" : (v==RuntimeEvaluateThrowOnSideEffect::noThrowOnSideEffect ? "noThrowOnSideEffect" : "throwOnSideEffect");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateBreaksControl
{
    unspecified      = -1,
    enableBreaks     = 0,
    noDisableBreaks  = enableBreaks,
    disableBreaks

}; // enum class RuntimeEvaluateBreaksControl

inline
std::string to_string(RuntimeEvaluateBreaksControl v)
{
    return v==RuntimeEvaluateBreaksControl::unspecified ? "unspecified" : (v==RuntimeEvaluateBreaksControl::enableBreaks ? "noDisableBreaks" : "disableBreaks");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateReplMode
{
    unspecified      = -1,
    noReplMode      = 0,
    replMode,

}; // enum class RuntimeEvaluateReplMode

inline
std::string to_string(RuntimeEvaluateReplMode v)
{
    return v==RuntimeEvaluateReplMode::unspecified ? "unspecified" : (v==RuntimeEvaluateReplMode::noReplMode ? "noReplMode" : "replMode");
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class RuntimeEvaluateCspMode // Content Security Policy
{
    unspecified      = -1,
    useCsp           = 0,
    dontAllowUnsafeEvalBlockedByCSP = useCsp,
    allowUnsafeEvalBlockedByCSP,

}; // enum class RuntimeEvaluateCspMode

inline
std::string to_string(RuntimeEvaluateCspMode v)
{
    return v==RuntimeEvaluateCspMode::unspecified ? "unspecified" : (v==RuntimeEvaluateCspMode::allowUnsafeEvalBlockedByCSP ? "allowUnsafeEvalBlockedByCSP" : "dontAllowUnsafeEvalBlockedByCSP");
}
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




