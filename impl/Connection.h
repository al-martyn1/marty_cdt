/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Connection

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "../Connection.h"

#include "../AtomicBoolIdHandler.h"
#include "../AtomicBoolMethodHandler.h"

//
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>
#include <variant>
#include <unordered_map>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/Connection.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_CDT_CONNECTION_CDT_COMMAND_CHECK_FOR_ID(idVariant)   \
            std::visit( [&](auto&& arg)                            \
                        {                                          \
                            using T = std::decay_t<decltype(arg)>; \
                                                                   \
                            if constexpr (std::is_same_v<T, unsigned>)                        {} \
                            else if constexpr (std::is_same_v<T, std::string>)                { throwGotMethodInsteadOfId(__func__, arg); } \
                            else if constexpr (std::is_same_v<T, marty::cdt::ResponseError>)  { throwResponseErrorMessage(__func__, arg); } \
                        }                                          \
                      , idVariant                                  \
                      )


//----------------------------------------------------------------------------
// Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
// throws exception on error
inline
bool Connection::cdtPageNavigate( PageNavigateResponse &response
                                , const std::string    &url
                                , unsigned             timeoutMs
                                , bool                 waitForLoadCompletion
                                )
{
    wsPushMethodEventHandlers();

    try
    {
        // Если не надо ждать, то сразу в сигнальном состоянии
        std::atomic<bool> domContentEventFiredFlag = waitForLoadCompletion ? false : true;
        std::atomic<bool> loadEventFiredFlag       = waitForLoadCompletion ? false : true;

        wsSetMethodEventHandler("Page.domContentEventFired", marty::cdt::AtomicBoolMethodHandler{domContentEventFiredFlag, false}); // false - в консоль ничего не выводим
        wsSetMethodEventHandler("Page.loadEventFired"      , marty::cdt::AtomicBoolMethodHandler{loadEventFiredFlag      , false});

        std::atomic<bool> pageNavigateFlag = false;
        wsSendCommandThrowable( __func__ // "cdtPageNavigate"
                              , "Page.navigate", { {"url", url} }
                              , [&](marty::cdt::Connection * /* pCon */, const marty::cdt::WebSocketMessage& /* msg */, marty::cdt::MessageIdVariant idVariant, marty::cdt::json j)
                                {
                                    MARTY_CDT_CONNECTION_CDT_COMMAND_CHECK_FOR_ID(idVariant);

                                    from_json(j, response);
                                    pageNavigateFlag = true;
                                }
                              );

        bool bRes = wsWaitAndDispatchMessages( timeoutMs
                                             , [&]() -> bool
                                               {
                                                   return bool(pageNavigateFlag) && bool(domContentEventFiredFlag) && bool(loadEventFiredFlag);
                                               }
                                             );
        wsPopMethodEventHandlers();

        return bRes;
    }
    catch(...)
    {
        wsPopMethodEventHandlers();
        throw;
    }

}

//--------------------------------------------------------------------------------------------------------------------
inline
bool Connection::cdtDomGetDocument( json       &jResult
                                  , int        depth
                                  , bool       pierce
                                  , unsigned   timeoutMs
                                  )
{
    std::atomic<bool> atomicBoolFlag = false;

    wsSendCommandThrowable( __func__
                          , "DOM.getDocument"
                          , {
                                {"depth" , depth },       // -1 означает "вся глубина"
                                {"pierce", pierce}        // проникать через shadow DOM
                            }
                          , [&](marty::cdt::Connection * /* pCon */ , const marty::cdt::WebSocketMessage&, marty::cdt::MessageIdVariant idVariant, marty::cdt::json j)
                            {
                                MARTY_CDT_CONNECTION_CDT_COMMAND_CHECK_FOR_ID(idVariant);

                                jResult = j;
                                atomicBoolFlag = true;
                            }
                          );

    return wsWaitAndDispatchMessages( timeoutMs, [&]() -> bool { return atomicBoolFlag; } );
}

//--------------------------------------------------------------------------------------------------------------------
inline
bool Connection::cdtDomGetDocument( DomDocument &domDocument
                                  , int         depth
                                  , bool        pierce
                                  , unsigned    timeoutMs
                                  )
{
    json j;
    bool res = cdtDomGetDocument(j, depth, pierce, timeoutMs);
    if (res)
    {
        domDocument.from_json(j);
    }

    return res;
}

//--------------------------------------------------------------------------------------------------------------------
bool Connection::cdtRuntimEvaluate( json                             &jResult
                                  , const std::string                &expression
                                  , unsigned                         timeoutMs
                                  , RuntimEvaluateReturnType         returnType         // returnByValue
                                  , const std::string                &contextId         // integer as string or empty string
                                  , const std::string                &objectGroup       // object group name string 
                                  , RuntimEvaluateAwaitPromise       awaitPromise       // awaitPromise
                                  , RuntimEvaluateUserGesture        userGesture        // userGesture
                                  , RuntimEvaluateThrowOnSideEffect  throwOnSideEffect  // throwOnSideEffect
                                  , RuntimEvaluateBreaksControl      breaksControl      // disableBreaks
                                  , RuntimEvaluateReplMode           replMode           // replMode
                                  , RuntimEvaluateCspMode            cspMode            // allowUnsafeEvalBlockedByCSP
                                  )
{
    json jParams = { { "expression", expression } };

    if (returnType!=RuntimEvaluateReturnType::unspecified)
        jParams["returnByValue"] = (returnType==RuntimEvaluateReturnType::returnByValue);

    if (!contextId.empty())
        jParams["contextId"] = std::stoi(contextId);

    if (!objectGroup.empty())
        jParams["objectGroup"] = objectGroup;

    if (awaitPromise!=RuntimEvaluateAwaitPromise::unspecified)
        jParams["awaitPromise"] = (awaitPromise==RuntimEvaluateAwaitPromise::awaitPromise);

    if (userGesture!=RuntimEvaluateUserGesture::unspecified)
        jParams["userGesture"] = (userGesture==RuntimEvaluateUserGesture::userGesture);

    if (throwOnSideEffect!=RuntimEvaluateThrowOnSideEffect::unspecified)
        jParams["throwOnSideEffect"] = (throwOnSideEffect==RuntimEvaluateThrowOnSideEffect::throwOnSideEffect);

    if (breaksControl!=RuntimEvaluateBreaksControl::unspecified)
        jParams["disableBreaks"] = (breaksControl==RuntimEvaluateBreaksControl::disableBreaks);

    if (replMode!=RuntimEvaluateReplMode::unspecified)
        jParams["replMode"] = (replMode==RuntimEvaluateReplMode::replMode);

    if (cspMode!=RuntimEvaluateCspMode::unspecified)
        jParams["allowUnsafeEvalBlockedByCSP"] = (cspMode==RuntimEvaluateCspMode::allowUnsafeEvalBlockedByCSP);

    std::atomic<bool> atomicBoolFlag = false;

    wsSendCommandThrowable( __func__
                          , "Runtime.evaluate"
                          , jParams
                          , [&](marty::cdt::Connection * /* pCon */ , const marty::cdt::WebSocketMessage&, marty::cdt::MessageIdVariant idVariant, marty::cdt::json j)
                            {
                                MARTY_CDT_CONNECTION_CDT_COMMAND_CHECK_FOR_ID(idVariant);

                                jResult = j;
                                atomicBoolFlag = true;
                            }
                          );

    return wsWaitAndDispatchMessages( timeoutMs, [&]() -> bool { return atomicBoolFlag; } );

}

//--------------------------------------------------------------------------------------------------------------------




} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/Connection.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

//--------------------------------------------------------------------------------------------------------------------
