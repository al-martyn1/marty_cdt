/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Connection

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "types.h"
//
#include "defs.h"
//
#include "utils.h"
//
#include "ErrorInfo.h"
//
#include "ResponseError.h"
//
#include "WebSocketMessage.h"
//
#include "MessageIdVariant.h"
//
// #include "AtomicBoolIdHandler.h"
// #include "AtomicBoolMethodHandler.h"

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
    // auto wsConnectRes = marty::cdt::utils::wsConnect( webSocket, defaultNewTabPageWsUrl // newPageWsUrl
    //                                                 , [&](const ix::WebSocketMessagePtr& pMsg)
    //                                                   {

//----------------------------------------------------------------------------
class Connection
{

    std::string           m_httpBaseUrl; // http://localhost:9810
    Timeouts              m_httpTimeouts;

    ix::WebSocket         m_webSocket;
    std::string           m_wsUrl;
    int                   m_wsTimeout = 3;
    std::atomic_uint      m_wsCommandId = 0;

    std::atomic_uint      m_wsStartCounter = 0;

    time_point_type       m_connectTimestamp; // = 0;

//--------------------------------------------------------------------------------------------------------------------
public:


    Connection() {}
    Connection(const std::string &wsUrl) : m_wsUrl(wsUrl) {}
    Connection(const std::string &wsUrl, int wsTimeout) : m_wsUrl(wsUrl), m_wsTimeout(wsTimeout) {}

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(Connection&&) = delete;
    Connection& operator=(Connection&&) = delete;

    ~Connection()
    {
        wsStop();
    }


//--------------------------------------------------------------------------------------------------------------------
public:

    void setHttpBaseUrl(const std::string &httpBaseUrl) // projectDirs.generateConnectionBaseUrlString(true /* http */);
    {
        m_httpBaseUrl = httpBaseUrl;
    }

    void setHttpTimeouts(Timeouts httpTimeouts)
    {
        m_httpTimeouts = httpTimeouts;
    }

    bool httpIsResponseOk(ix::HttpResponsePtr httpResponsePtr) const
    {
        return utils::ixHttpErrorCodeIsOk(httpResponsePtr->errorCode);
    }

    ix::HttpResponsePtr httpGetJsonList(std::vector<JsonListResponseEntry> &list) const
    {
        auto response = utils::httpGetJsonList(m_httpBaseUrl, m_httpTimeouts);
        if (!utils::ixHttpErrorCodeIsOk(response->errorCode))
            return response;

        if (!response->body.empty())
        {
            auto j = json::parse(response->body);
            from_json(j, list);
        }

        return response;
    }

    ix::HttpResponsePtr checkRunAndGetJsonList( std::vector<JsonListResponseEntry> &list
                                              , const std::string                  &chromeName  // "chrome"
                                              , const std::vector<std::string>     &spawnArgs
                                              , Timeouts                           probeTimeouts
                                              ) const
    {
        auto response = utils::runConnectAndGetJsonList(m_httpBaseUrl, chromeName, spawnArgs, probeTimeouts, m_httpTimeouts);
        if (!utils::ixHttpErrorCodeIsOk(response->errorCode))
            return response;
    
        if (!response->body.empty())
        {
            auto j = json::parse(response->body);
            from_json(j, list);
        }

        return response;
    }

    ix::HttpResponsePtr checkRunAndGetJsonList( std::vector<JsonListResponseEntry> &list
                                              , const std::string                  &chromeName  // "chrome"
                                              , const std::vector<std::string>     &spawnArgs
                                              ) const
    {
        return checkRunAndGetJsonList(list, chromeName, spawnArgs, m_httpTimeouts);
    }

    ix::HttpResponsePtr httpGetJsonVersion(JsonVersionResponse &versionResponse) const
    {
        auto response = utils::httpGetJsonVersion(m_httpBaseUrl, m_httpTimeouts);
        if (!utils::ixHttpErrorCodeIsOk(response->errorCode))
            return response;

        if (!response->body.empty())
        {
            auto j = nlohmann::json::parse(response->body);
            from_json(j, versionResponse);
        }

        return response;
    }

    ix::HttpResponsePtr httpNewPage(JsonNewPageResponse &newPageResponse, const std::string &newPageUrl) const
    {
        auto response = utils::httpPutJsonNewPage(m_httpBaseUrl, newPageUrl, m_httpTimeouts);
        if (!utils::ixHttpErrorCodeIsOk(response->errorCode))
            return response;

        if (!response->body.empty())
        {
            auto j = nlohmann::json::parse(response->body);
            from_json(j, newPageResponse);
        }

        return response;
    }

    ix::HttpResponsePtr httpNewPage(JsonNewPageResponse &newPageResponse) const
    {
        return httpNewPage(newPageResponse, std::string());
    }

    ix::HttpResponsePtr httpClosePage(const std::string &pageId) const
    {
        return utils::httpClosePage(m_httpBaseUrl, pageId, m_httpTimeouts);
    }

    ix::HttpResponsePtr httpActivatePage(const std::string &pageId) const
    {
        return utils::httpActivatePage(m_httpBaseUrl, pageId, m_httpTimeouts);
    }


//--------------------------------------------------------------------------------------------------------------------
protected:

    // Нам нужна очередь обработчиков по ID запроса
    //   её мы блокируем при добавлении
    //   при поиске и извлечении - обработчики одноразовые

    // Нам нужна мапа для обработчиков без ID
    //   её мы блокируем при добавлении обработчиков
    //   при удалении обработчиков
    //   при поиске обработчика

    // Нам нужен дефолтный обработчик запросов без ID
    // используем тот же блокиратор, что и для очереди обработчиков без ID

    // std::lock_guard<std::mutex> lock(m_mutex);

    // Нам нужна очередь сообщений и связанных с ними обработчиков, для вызова из dispatchMessage
    // Для этой очереди тоже нужен блокировщик
    //   для добавления
    //   проверки на непустоту и извлечения

    // struct WebSocketMessage
    // {
    //     WebSocketMessageType type;
    //     const std::string& str;
    //     size_t wireSize;
    //     WebSocketErrorInfo errorInfo;
    //     WebSocketOpenInfo openInfo;
    //     WebSocketCloseInfo closeInfo;
    //     bool binary;

    // Системные сообщения Open/Close/Error/Ping/Pong/Fragment, обработчик:
    // void onSystemEvent(ix::WebSocketMessageType type, const ix::WebSocketMessagePtr&)

    // Для сообщений типа Message надо проверять, binary оно или нет, и проверять str на непустоту
    // Если Message, не binary и str не пустая - парсим в json
    // void onMessageEvent(ix::WebSocketMessageType type, const ix::WebSocketMessagePtr&, json j)
    // нужно проверять j.is_null()
    // Ещё удобно вынести id и method в отдельные параметры
    // И использовать такой хэндлер для синхронных ответов на запросы
    // и для асинхронных сообщений о событиях
    // в первом случае будет валидным unsigned id
    // во втором случае будет валидным std::string method

    // ответ с ошибокой
    // {
    //   "id": 1,
    //   "error": {
    //     "code": -32000,
    //     "message": "No resource with given identifier found"
    //   }
    // }
    //  
    // ответ по делу
    // {
    //   "id": 3,
    //   "result": {}
    // }

    // Сообщение типа Message при наличии id может содержать
    // как "result" объект, так и "error" объект.
    // Если объект - "error", то это ошибка

    // Если id нет, это "инициативное" сообщение,
    // и оно содержит:
    //  - строку "method"
    //  - объект "params"
    // "method": "Network.responseReceivedExtraInfo",
    // "params": {}



//--------------------------------------------------------------------------------------------------------------------
public:


    using SystemEventHandlerType = std::function<void(Connection *pCon, const WebSocketMessage&, ix::WebSocketMessageType type)>;

    // Если method - пусто, то это ответ на запрос, и id валиден
    // Если не пусто - то это асинхронное сообщение
    using MessageHandlerType     = std::function<void(Connection *pCon, const WebSocketMessage&, MessageIdVariant, json)>;


//--------------------------------------------------------------------------------------------------------------------
protected:

    using mutex_type = std::mutex;
    //using mutex_type = std::recursive_mutex;


    struct IdHandlerEntry
    {
        unsigned              id;
        MessageHandlerType    handler;

    }; // struct IdHandlerEntry


    std::queue<WebSocketMessage>                                              m_msgQueue;
    mutable mutex_type                                                        m_msgQueueMutex;

    mutable std::deque<IdHandlerEntry>                                        m_idQueue;
    MessageHandlerType                                                        m_defaultIdHandler;
    mutable mutex_type                                                        m_idQueueMutex;

    std::unordered_map<ix::WebSocketMessageType, SystemEventHandlerType>      m_systemHandlers;
    mutable mutex_type                                                        m_systemHandlersMutex;


    std::stack<std::unordered_map<std::string, MessageHandlerType> >          m_methodHandlersStack;
    std::unordered_map<std::string, MessageHandlerType>                       m_methodHandlers;
    mutable std::unordered_map<std::string, std::vector<std::string> >        m_methodHandlerPatternsCache;
    mutable mutex_type                                                        m_methodHandlersMutex;


    MessageHandlerType findMethodEventHandler(const std::string &method) const
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);

        auto methodNamePatternsCacheIt = m_methodHandlerPatternsCache.find(method);

        if (methodNamePatternsCacheIt==m_methodHandlerPatternsCache.end())
        {
            std::vector<std::string> methodNamePatterns = utils::generatePatterns(method);
            methodNamePatternsCacheIt = m_methodHandlerPatternsCache.emplace(method, std::move(methodNamePatterns)).first;
        }

        for(const auto &p: methodNamePatternsCacheIt->second)
        {
            auto it = m_methodHandlers.find(p);
            if (it!=m_methodHandlers.end())
                return it->second;
        }

        return MessageHandlerType();
    }

    SystemEventHandlerType findSystemEventHandler(ix::WebSocketMessageType msgType) const
    {
        std::lock_guard<mutex_type> lock(m_systemHandlersMutex);
        auto it = m_systemHandlers.find(msgType);
        if (it==m_systemHandlers.end())
            return SystemEventHandlerType();
        return it->second;
    }


    void wsCallback(const ix::WebSocketMessagePtr& pMsg)
    {
        std::lock_guard<mutex_type> lock(m_msgQueueMutex);
        m_msgQueue.emplace(*pMsg);
    }

    bool getQueuedMessage(WebSocketMessage &msg)
    {
        std::lock_guard<mutex_type> lock(m_msgQueueMutex);
        if (m_msgQueue.empty())
            return false;

        msg = std::move(m_msgQueue.front());
        m_msgQueue.pop();

        return true;
    }

    void enqueIdHandler(unsigned id, MessageHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_idQueueMutex);
        m_idQueue.push_back({id, handler});
    }

    MessageHandlerType findIdHandler(unsigned id) const
    {
        std::lock_guard<mutex_type> lock(m_idQueueMutex);
        if (m_idQueue.empty())
            return m_defaultIdHandler; // MessageHandlerType();

        // Частый кейс - ожидаемый элемент в начале очереди
        if (m_idQueue.front().id==id)
        {
            auto tmp = m_idQueue.front();
            m_idQueue.pop_front();
            return tmp.handler;
        }

        // начинаем со второго элемента, первый уже проверили, и у нас точно не нулевое число элементов
        for(auto it=std::next(m_idQueue.begin()); it!=m_idQueue.end(); ++it)
        {
            if (it->id==id)
            {
                auto tmp = *it;
                m_idQueue.erase(it);
                return tmp.handler;
            }
        }

        return m_defaultIdHandler; // MessageHandlerType();
    }


    // MessageHandlerType getDefaultMessageHandler() const
    // {
    //     return MessageHandlerType([](const WebSocketMessage&, MessageIdVariant, json){});
    // }


    void processMessage(const WebSocketMessage &msg)
    {
        if ( msg.type==ix::WebSocketMessageType::Open
          || msg.type==ix::WebSocketMessageType::Close
          || msg.type==ix::WebSocketMessageType::Error
          || msg.type==ix::WebSocketMessageType::Ping
          || msg.type==ix::WebSocketMessageType::Pong
          || msg.type==ix::WebSocketMessageType::Fragment
           )
        {
            // Тут надо вызвать какой-то системный хэндлер
            auto sysHandler = findSystemEventHandler(msg.type);
            if (sysHandler)
                sysHandler(this, msg, msg.type);

        }
        else if (msg.type==ix::WebSocketMessageType::Message)
        {
            // Тут вызываем хэндлер для Message
            // Нужна доп обработка

            // MessageHandlerType findMethodEventHandler(const std::string &method) const
            // MessageHandlerType findIdHandler(unsigned id) const

            json j;

            if (!msg.binary)
            {
                if (msg.str.empty())
                {
                    throw std::runtime_error("marty::cdt::Connection::processMessage: Message doesn't contain data");
                }

                j = json::parse(msg.str);
            }

            if (j.contains("id"))
            {
                unsigned id = j["id"].get<unsigned>();

                auto idHandler = findIdHandler(id);

                // if (!idHandler)
                // {
                //     idHandler = m_defaultIdHandler;
                // }

                if (idHandler)
                {
                    if (j.contains("result"))
                    {
                        //auto jResult = j["result"];
                        idHandler(this, msg, MessageIdVariant(id), j["result"]);
                    }
                    else if (j.contains("error"))
                    {
                        auto jError = j["error"];

                        ResponseError re;
                        re.id = id;
                        from_json(jError, re.errorInfo);
                        idHandler(this, msg, MessageIdVariant(re), json{});
                    }
                    else
                    {
                        throw std::runtime_error( "marty::cdt::Connection::processMessage: Message doesn't contain known data:\n"
                                                + j.dump(2)
                                                );
                    }
                }
            }

            else if (j.contains("method"))
            {
                auto method = j["method"].get<std::string>();
                if (method.empty())
                    throw std::runtime_error("marty::cdt::Connection::processMessage: Message contains empty 'method'");

                auto methodHandler = findMethodEventHandler(method);
                if (methodHandler)
                {
                    if (j.contains("params"))
                    {
                        methodHandler(this, msg, MessageIdVariant(method), j["params"]);
                    }
                    else
                    {
                        methodHandler(this, msg, MessageIdVariant(method), json{});
                    }
                }
            }

            else
            {
                throw std::runtime_error( "marty::cdt::Connection::processMessage: unknown Message type, message data:\n"
                                        + j.dump(2)
                                        );
            }

        }

        else // unknown msg.type
        {
            // Кинуть исключение?
            throw std::runtime_error("marty::cdt::Connection::processMessage: unknown Message type");
        }

    }

    // ответ с ошибокой
    // {
    //   "id": 1,
    //   "error": {
    //     "code": -32000,
    //     "message": "No resource with given identifier found"
    //   }
    // }
    //  
    // ответ по делу
    // {
    //   "id": 3,
    //   "result": {}
    // }

    // Сообщение типа Message при наличии id может содержать
    // как "result" объект, так и "error" объект.
    // Если объект - "error", то это ошибка

    // Если id нет, это "инициативное" сообщение,
    // и оно содержит:
    //  - строку "method"
    //  - объект "params"
    // "method": "Network.responseReceivedExtraInfo",
    // "params": {}


    // struct WebSocketMessage
    // {
    //     ix::WebSocketMessageType    type     ;
    //     std::string                 str      ;
    //     size_t                      wireSize ;
    //     ix::WebSocketErrorInfo      errorInfo;
    //     ix::WebSocketOpenInfo       openInfo ;
    //     ix::WebSocketCloseInfo      closeInfo;
    //     bool                        binary   ;




    // struct MessageDispatchQueueEntry
    // {
    //     ix::WebSocketMessagePtr     msg;
    //  
    //     // depends on msg->type
    //     MessageHandlerType          messageHandlerType; // if (msg->type==ix::WebSocketMessageType::Message)
    //     SystemEventHandlerType      systemEventHandler; // else
    //  
    // }; // struct MessageDispatchQueueEntry
    //  
    // std::deque<MessageDispatchQueueEntry>    m_dispatchQueue;


    //--------------------------------------------------
    void throwResponseErrorMessage(const char *fnFrom, const ResponseError &err)
    {
        throw std::runtime_error(std::string(fnFrom) + ": " + std::to_string(err.errorInfo.code) + ": " + err.errorInfo.message);
    }


//--------------------------------------------------------------------------------------------------------------------
public:


    //--------------------------------------------------
    void wsSetUrl(const std::string &url)
    {
        m_wsUrl = url;
    }

    void wsSetTimeout(int to)
    {
        m_wsTimeout = to;
    }


    //--------------------------------------------------
    void wsSetDefaultIdHandler(MessageHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_idQueueMutex);
        m_defaultIdHandler = handler;
    }


    //--------------------------------------------------
    void wsSetSystemEventHandler(ix::WebSocketMessageType msgType, SystemEventHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_systemHandlersMutex);
        m_systemHandlers[msgType] = handler;
    }

    void wsSetSystemEventHandler(const std::vector<ix::WebSocketMessageType> &msgTypes, SystemEventHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_systemHandlersMutex);
        for(auto t: msgTypes)
            m_systemHandlers[t] = handler;
    }

    void wsSetSystemEventHandler(const std::initializer_list<ix::WebSocketMessageType> &msgTypes, SystemEventHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_systemHandlersMutex);
        for(auto t: msgTypes)
            m_systemHandlers[t] = handler;
    }


    //--------------------------------------------------
    void wsPushMethodEventHandlers()
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);
        m_methodHandlersStack.push(m_methodHandlers);
    }

    void wsPopMethodEventHandlers()
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);
        if (m_methodHandlersStack.empty())
            throw std::runtime_error("marty::cdt::Connection::wsPopMethodEventHandlers: stack empty, failed to pop MethodEventHandlers");

        m_methodHandlers = m_methodHandlersStack.top();
        m_methodHandlersStack.pop();
    }

    void wsSetMethodEventHandler(const std::string &method, MessageHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);
        m_methodHandlers[method] = handler;
    }

    void wsSetMethodEventHandler(const std::vector<std::string> &methods, MessageHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);
        for(auto m: methods)
            m_methodHandlers[m] = handler;
    }

    void wsSetMethodEventHandler(const std::initializer_list<std::string> &methods, MessageHandlerType handler)
    {
        std::lock_guard<mutex_type> lock(m_methodHandlersMutex);
        for(auto m: methods)
            m_methodHandlers[m] = handler;
    }




    //--------------------------------------------------
    ix::WebSocket& getWs()
    {
        return m_webSocket;
    }
 
    ix::WebSocketInitResult wsConnect()
    {
        return utils::wsConnect( m_webSocket, m_wsUrl
                               , [this](const ix::WebSocketMessagePtr& pm) { wsCallback(pm); }
                               , m_wsTimeout
                               );
    }
 

    //--------------------------------------------------
    // Since wsStart moment
    std::uint64_t getTimestamp() const
    {
        return (std::uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(utils::getSteadyClockNow() - m_connectTimestamp).count();
        //return std::chrono::milliseconds(utils::getSteadyClockNow() - m_connectTimestamp);
        //return utils::getSteadyClockNow() - m_connectTimestamp;
    }

    //--------------------------------------------------
    void wsStart()
    {
        
        // if (m_webSocket.getReadyState() == ix::ReadyState::Closed)
        //     m_webSocket.start();

        // if (m_wsStartCounter==0)
        // {
        //     m_webSocket.start();
        //     ++m_wsStartCounter;
        // }

        unsigned expected = 0;
        if (m_wsStartCounter.compare_exchange_strong(expected, 1))
        {
            m_webSocket.start();
            m_connectTimestamp = utils::getSteadyClockNow();
        }
    }
 
    void wsStop()
    {
        // if (m_webSocket.getReadyState() != ix::ReadyState::Closed)
        //     m_webSocket.stop();

        // if (m_wsStartCounter!=0)
        // {
        //     m_webSocket.stop();
        //     --m_wsStartCounter;
        // }

        unsigned expected = 1;
        if (m_wsStartCounter.compare_exchange_strong(expected, 0))
            m_webSocket.stop();
    }


    //--------------------------------------------------
    // return true, if some messages processed
    bool wsDispatchMessages()
    {
        std::size_t processedCount = 0;
        WebSocketMessage msg;
        while(getQueuedMessage(msg))
        {
            ++processedCount;
            processMessage(msg);
        }

        return processedCount!=0;
    }

    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    bool wsWaitAndDispatchMessages(unsigned timeoutMs, const std::atomic<bool> &waitFor)
    {
        auto timeout = std::chrono::milliseconds(timeoutMs);
        auto start   = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        auto now     = start;

        do
        {
            wsDispatchMessages();

            if (waitFor)
                return true;

            std::this_thread::yield();

            now = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        }
        while(std::chrono::duration_cast<std::chrono::milliseconds>(now - start) < timeout);

        return false;
    }

    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    // Позволяет меньше нагружать процессор, если мы ожидаем долгий процесс, и нам не сильно критично
    // очень быстро ловить все приходящие сообщения
    bool wsWaitAndDispatchMessages(unsigned timeoutMs, const std::atomic<bool> &waitFor, unsigned sleepPeriodMs)
    {
        // Чтобы не слишком долго засыпать и не сильно проспать заданный таймаут
        if (sleepPeriodMs*2 >= timeoutMs)
            sleepPeriodMs = timeoutMs / 2;

        if (sleepPeriodMs == 0)
            sleepPeriodMs = 1; // гарантировать хотя бы 1 мс паузы, раз нам нужен сон

        // Для автоматизации браузера через CDP я использую:
        // 50 мс – для ожидания загрузки страницы.
        // 20 мс – для ожидания появления элементов после загрузки.
        // 10 мс – для интерактивных действий (клик, ввод, ожидание ответа).
        // И всегда ограничиваю максимум 100 мс – это разумный предел.

        if (sleepPeriodMs > 100)
            sleepPeriodMs = 100;


        auto sleepPeriod = std::chrono::milliseconds(sleepPeriodMs);
        auto timeout     = std::chrono::milliseconds(timeoutMs);
        auto start       = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        auto now         = start;

        do
        {
            wsDispatchMessages();

            if (waitFor)
                return true;

            std::this_thread::sleep_for(sleepPeriod);

            now = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        }
        while(std::chrono::duration_cast<std::chrono::milliseconds>(now - start) < timeout);

        return false;
    }

    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    // Всегда сбрасывает флаг waitFor
    bool wsWaitAndDispatchMessagesAndReset(unsigned timeoutMs, std::atomic<bool> &waitFor)
    {
        bool res = wsWaitAndDispatchMessages(timeoutMs, waitFor);
        waitFor  = false;
        return res;
    }

    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    // Позволяет меньше нагружать процессор, если мы ожидаем долгий процесс, и нам не сильно критично
    // очень быстро ловить все приходящие сообщения
    // Всегда сбрасывает флаг waitFor
    bool wsWaitAndDispatchMessagesAndReset(unsigned timeoutMs, std::atomic<bool> &waitFor, unsigned sleepPeriodMs)
    {
        bool res = wsWaitAndDispatchMessages(timeoutMs, waitFor, sleepPeriodMs);
        waitFor  = false;
        return res;
    }


    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    template<typename EventFiredCheckHandler>
    bool wsWaitAndDispatchMessages(unsigned timeoutMs, EventFiredCheckHandler eventFiredCheckHandler)
    {
        auto timeout = std::chrono::milliseconds(timeoutMs);
        auto start   = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        auto now     = start;

        do
        {
            wsDispatchMessages();

            if (eventFiredCheckHandler())
                return true;

            std::this_thread::yield();

            now = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        }
        while(std::chrono::duration_cast<std::chrono::milliseconds>(now - start) < timeout);

        return false;
    }

    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    // Позволяет меньше нагружать процессор, если мы ожидаем долгий процесс, и нам не сильно критично
    // очень быстро ловить все приходящие сообщения
    template<typename EventFiredCheckHandler>
    bool wsWaitAndDispatchMessages(unsigned timeoutMs, EventFiredCheckHandler eventFiredCheckHandler, unsigned sleepPeriodMs)
    {
        // Чтобы не слишком долго засыпать и не сильно проспать заданный таймаут
        if (sleepPeriodMs*2 >= timeoutMs)
            sleepPeriodMs = timeoutMs / 2;

        if (sleepPeriodMs == 0)
            sleepPeriodMs = 1; // гарантировать хотя бы 1 мс паузы, раз нам нужен сон

        // Для автоматизации браузера через CDP я использую:
        // 50 мс – для ожидания загрузки страницы.
        // 20 мс – для ожидания появления элементов после загрузки.
        // 10 мс – для интерактивных действий (клик, ввод, ожидание ответа).
        // И всегда ограничиваю максимум 100 мс – это разумный предел.

        if (sleepPeriodMs > 100)
            sleepPeriodMs = 100;


        auto sleepPeriod = std::chrono::milliseconds(sleepPeriodMs);
        auto timeout     = std::chrono::milliseconds(timeoutMs);
        auto start       = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        auto now         = start;

        do
        {
            wsDispatchMessages();

            if (eventFiredCheckHandler())
                return true;

            std::this_thread::sleep_for(sleepPeriod);

            now = std::chrono::steady_clock::now(); // utils::getSteadyClockNow(); // std::chrono::steady_clock::now();
        }
        while(std::chrono::duration_cast<std::chrono::milliseconds>(now - start) < timeout);

        return false;
    }


    //--------------------------------------------------
    ix::WebSocketSendInfo wsSendEventSubscription(const std::string &eventName, bool enableSubscription=true)
    {
        return utils::wsSendEventSubscription(m_webSocket, m_wsCommandId++, eventName + (enableSubscription?".enable":".disable"));
    }
 
    ix::WebSocketSendInfo wsSendEventSubscription(const std::string &eventName, MessageHandlerType handler, bool enableSubscription=true)
    {
        auto newId = m_wsCommandId++;
 
        auto res = utils::wsSendEventSubscription(m_webSocket, newId, eventName + (enableSubscription?".enable":".disable"));
        if (res.success)
            enqueIdHandler(newId, handler);

        return res;
    }
 
    ix::WebSocketSendInfo wsEventSubscribe(const std::string &eventName, bool enableSubscription=true)
    {
        return wsSendEventSubscription(eventName, enableSubscription);
    }
 
    ix::WebSocketSendInfo wsEventSubscribe(const std::string &eventName, MessageHandlerType handler, bool enableSubscription=true)
    {
        return wsSendEventSubscription(eventName, handler, enableSubscription);
    }
 
    ix::WebSocketSendInfo wsEventUnsubscribe(const std::string &eventName)
    {
        return wsSendEventSubscription(eventName, false);
    }
 
    ix::WebSocketSendInfo wsEventUnsubscribe(const std::string &eventName, MessageHandlerType handler)
    {
        return wsSendEventSubscription(eventName, handler, false);
    }


    //--------------------------------------------------
    ix::WebSocketSendInfo wsSendCommand(const std::string &method, json params, MessageHandlerType handler)
    {
        auto newId = m_wsCommandId++;

        auto res = utils::wsSendCommand(m_webSocket, newId, method, params);
        if (res.success)
            enqueIdHandler(newId, handler);

        return res;
    }
  
    ix::WebSocketSendInfo wsSendCommand(const std::string &method, json params)
    {
        return utils::wsSendCommand(m_webSocket, m_wsCommandId++, method, params);
    }
  
    ix::WebSocketSendInfo wsSendCommand(const std::string &method, MessageHandlerType handler)
    {
        return wsSendCommand(method, json(), handler);
    }
  
    ix::WebSocketSendInfo wsSendCommand(const std::string &method)
    {
        return wsSendCommand(method, json());
    }


    void wsSendCommandThrowable(const std::string &calledFrom, const std::string &method, json params, MessageHandlerType handler)
    {
        auto res = wsSendCommand(method, params, handler);
        if (!res.success)
            throw std::runtime_error("from '" + calledFrom + "': failed to send command '" + method + "'");
    }

    void wsSendCommandThrowable(const std::string &calledFrom, const std::string &method, json params)
    {
        auto res = wsSendCommand(method, params);
        if (!res.success)
            throw std::runtime_error("from '" + calledFrom + "': failed to send command '" + method + "'");
    }

    void wsSendCommandThrowable(const std::string &calledFrom, const std::string &method, MessageHandlerType handler)
    {
        auto res = wsSendCommand(method, handler);
        if (!res.success)
            throw std::runtime_error("from '" + calledFrom + "': failed to send command '" + method + "'");
    }

    void wsSendCommandThrowable(const std::string &calledFrom, const std::string &method)
    {
        auto res = wsSendCommand(method);
        if (!res.success)
            throw std::runtime_error("from '" + calledFrom + "': failed to send command '" + method + "'");
    }


    // ix::WebSocketSendInfo wsSendCommand(const std::string &method, json params, MessageHandlerType handler)
    // {
    //     auto newId = m_wsCommandId++;
    //  
    //     auto res = utils::wsSendCommand(m_webSocket, newId, method, params);
    //     if (res.success)
    //         enqueIdHandler(newId, handler);
    //  
    //     return res;
    // }
    //  
    // ix::WebSocketSendInfo wsSendCommand(const std::string &method, json params)
    // {
    //     return utils::wsSendCommand(m_webSocket, m_wsCommandId++, method, params);
    // }
    //  
    // ix::WebSocketSendInfo wsSendCommand(const std::string &method, MessageHandlerType handler)
    // {
    //     return wsSendCommand(method, json(), handler);
    // }
    //  
    // ix::WebSocketSendInfo wsSendCommand(const std::string &method)
    // {
    //     return wsSendCommand(method, json());
    // }




    //--------------------------------------------------
    // Возвращает true, если дождались события, и false, если прошел заданный timeoutMs
    // throws exception on error
    bool cdtPageNavigate( PageNavigateResponse &response
                        , const std::string    &url
                        , unsigned             timeoutMs   = 10000
                        , bool                 waitForLoadCompletion = true
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
                                        std::visit( [&](auto&& arg)
                                                    {
                                                        using T = std::decay_t<decltype(arg)>;
  
                                                        if constexpr (std::is_same_v<T, unsigned>)
                                                        {
                                                            // Ok, nothing to do
                                                        }
                                                        else if constexpr (std::is_same_v<T, std::string>)
                                                        {
                                                            throw std::runtime_error(std::string(__func__) + ": awaiting ID, but got method: '" + std::get<std::string>(idVariant) + "'");
                                                        }
                                                        else if constexpr (std::is_same_v<T, marty::cdt::ResponseError>)
                                                        {
                                                            throwResponseErrorMessage(__func__, arg);
                                                            // throw std::runtime_error(std::string(__func__) + ": " + std::to_string(arg.errorInfo.code) + ": " + arg.errorInfo.message);
                                                        }
                                                    }
                                                  , idVariant
                                                  );


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



}; // class Connection



//--------------------------------------------------------------------------------------------------------------------

} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/Connection.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

//--------------------------------------------------------------------------------------------------------------------
