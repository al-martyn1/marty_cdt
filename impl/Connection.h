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
inline
bool Connection::cdtRuntimeEvaluate( json                             &jResult
                                   , const std::string                &expression
                                   , unsigned                         timeoutMs
                                   , RuntimeEvaluateReturnType         returnType         // returnByValue
                                   , const std::string                &contextId         // integer as string or empty string
                                   , const std::string                &objectGroup       // object group name string 
                                   , RuntimeEvaluateAwaitPromise       awaitPromise       // awaitPromise
                                   , RuntimeEvaluateUserGesture        userGesture        // userGesture
                                   , RuntimeEvaluateThrowOnSideEffect  throwOnSideEffect  // throwOnSideEffect
                                   , RuntimeEvaluateBreaksControl      breaksControl      // disableBreaks
                                   , RuntimeEvaluateReplMode           replMode           // replMode
                                   , RuntimeEvaluateCspMode            cspMode            // allowUnsafeEvalBlockedByCSP
                                   )
{
    json jParams = { { "expression", expression } };

    if (returnType!=RuntimeEvaluateReturnType::unspecified)
        jParams["returnByValue"] = (returnType==RuntimeEvaluateReturnType::returnByValue);

    if (!contextId.empty())
        jParams["contextId"] = std::stoi(contextId);

    if (!objectGroup.empty())
        jParams["objectGroup"] = objectGroup;

    if (awaitPromise!=RuntimeEvaluateAwaitPromise::unspecified)
        jParams["awaitPromise"] = (awaitPromise==RuntimeEvaluateAwaitPromise::awaitPromise);

    if (userGesture!=RuntimeEvaluateUserGesture::unspecified)
        jParams["userGesture"] = (userGesture==RuntimeEvaluateUserGesture::userGesture);

    if (throwOnSideEffect!=RuntimeEvaluateThrowOnSideEffect::unspecified)
        jParams["throwOnSideEffect"] = (throwOnSideEffect==RuntimeEvaluateThrowOnSideEffect::throwOnSideEffect);

    if (breaksControl!=RuntimeEvaluateBreaksControl::unspecified)
        jParams["disableBreaks"] = (breaksControl==RuntimeEvaluateBreaksControl::disableBreaks);

    if (replMode!=RuntimeEvaluateReplMode::unspecified)
        jParams["replMode"] = (replMode==RuntimeEvaluateReplMode::replMode);

    if (cspMode!=RuntimeEvaluateCspMode::unspecified)
        jParams["allowUnsafeEvalBlockedByCSP"] = (cspMode==RuntimeEvaluateCspMode::allowUnsafeEvalBlockedByCSP);

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
#if 1
template<typename CheckIterType>
inline
bool Connection::cdtRuntimeEvaluateGetValueImpl( json                              &jValue
                                               , const std::string                 &expression
                                               , CheckIterType                     checkBegin
                                               , CheckIterType                     checkEnd
                                               , unsigned                          timeoutMs 
                                               , RuntimeEvaluateReturnType         returnType          // returnByValue                    
                                               , const std::string                 &contextId          // integer as string or empty string
                                               , const std::string                 &objectGroup        // object group name string         
                                               , RuntimeEvaluateAwaitPromise       awaitPromise        // awaitPromise                     
                                               , RuntimeEvaluateUserGesture        userGesture         // userGesture                      
                                               , RuntimeEvaluateThrowOnSideEffect  throwOnSideEffect   // throwOnSideEffect                
                                               , RuntimeEvaluateBreaksControl      breaksControl       // disableBreaks                    
                                               , RuntimeEvaluateReplMode           replMode            // replMode                         
                                               , RuntimeEvaluateCspMode            cspMode             // allowUnsafeEvalBlockedByCSP      
                                               )
{
    json jResult;
    bool bRes = cdtRuntimeEvaluate( jResult
                                  , expression
                                  , timeoutMs
                                  , returnType         
                                  , contextId          
                                  , objectGroup        
                                  , awaitPromise       
                                  , userGesture        
                                  , throwOnSideEffect  
                                  , breaksControl      
                                  , replMode           
                                  , cspMode            
                                  );
    if (!bRes)
        return bRes;

    if (!jResult.contains("result"))
        throw std::runtime_error("no 'result' in reply: " + jResult.dump());

    auto jResultObj = jResult["result"];

    if (checkBegin!=checkEnd) // Есть значения для проверки типа
    {
        if (!jResultObj.contains("type"))
            throw std::runtime_error("no 'result'/'type' in reply: " + jResult.dump());
    
        std::string typeStr = jResultObj["type"].get<std::string>();

        bool bTypeFound = false;
        for(auto it=checkBegin; it!=checkEnd; ++it)
        {
            if (typeStr==*it)
            {
                bTypeFound = true;
                break;
            }
        }

        if (!bTypeFound)
            throw std::runtime_error("expected " + utils::mergeValsToString(checkBegin, checkEnd, ",", "or", "\'") + " result type, but got '" + typeStr + "'");
    }


        if (typeStr!="string")
            throw std::runtime_error("expected 'string' result type, but got '" + typeStr + "'");
    

        // std::vector<std::string> values = {"string", "number", "boolean", "object"};
        // std::string merged = marty::cdt::utils::mergeValsToString(values.begin(), values.end(), ",", "or", [](auto s) { return '\'' + s + '\''; });
        // cout << "Merged values: " << merged << "\n";



    if (!jResultObj.contains("value"))
        throw std::runtime_error("no 'result'/'value' in reply: " + jResult.dump());

    html = jResultObj["value"].get<std::string>();


    // {
    //   "result": {
    //     "type": "undefined"
    //   }
    // }
    //  
    // window.sessionStorage.getItem JSON:
    // {
    //   "result": {
    //     "type": "string",
    //     "value": "TYRNIYTFTYHJ"
    //   }
    // }
    
}

#endif
//--------------------------------------------------------------------------------------------------------------------
inline
bool Connection::cdtGetHtml( std::string        &html
                           , unsigned           timeoutMs
                           , const std::string  &contextId    // integer as string or empty string
                           , const std::string  &objectGroup  // object group name string         
                           )
{
    json jResult;
    bool bRes = cdtRuntimeEvaluate( jResult, "document.documentElement.outerHTML"
                                  , timeoutMs
                                  , RuntimeEvaluateReturnType::returnByValue
                                  , contextId
                                  , objectGroup
                                  );
    if (!bRes)
        return bRes;

    // Good response
    // {
    //   "result": {
    //     "type": "string",
    //     "value": "<html lang=\"ru\" >...</html>"
    //   }
    // }
    
    // Bad response
    // {
    //   "result": {
    //     "type": "undefined"
    //   }
    // }

    if (!jResult.contains("result"))
        throw std::runtime_error("no 'result' in reply: " + jResult.dump());

    auto jResultObj = jResult["result"];

    if (!jResultObj.contains("type"))
        throw std::runtime_error("no 'result'/'type' in reply: " + jResult.dump());

    std::string typeStr = jResultObj["type"].get<std::string>();
    if (typeStr!="string")
        throw std::runtime_error("expected 'string' result type, but got '" + typeStr + "'");

    if (!jResultObj.contains("value"))
        throw std::runtime_error("no 'result'/'value' in reply: " + jResult.dump());

    html = jResultObj["value"].get<std::string>();

    return bRes;
}
//--------------------------------------------------------------------------------------------------------------------



} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/Connection.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

//--------------------------------------------------------------------------------------------------------------------
