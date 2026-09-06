/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief Dom

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>
#include <stdexcept>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/JsonVersionResponse.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct DomNodeAttribute
{
    std::string       name ;
    std::string       value;

    void clear()
    {
        name .clear();
        value.clear();
    }

}; // struct DomNodeAttribute

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct DomNode;
void from_json(const json& j, DomNode &n);

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct DomNode
{
    std::string                      xmlVersion        ;
    std::string                      documentURL       ;
    std::string                      baseURL           ;

    int                              backendNodeId     ;
    std::string                      localName         ;
    int                              nodeId            ;
    std::string                      nodeName          ;
    int                              nodeType          ; // ???
    std::string                      nodeValue         ;
    int                              parentId          ;
    std::string                      publicId          ;
    std::string                      systemId          ;
    std::string                      frameId           ;
    std::string                      compatibilityMode ;
    bool                             isScrollable      ;
    bool                             isSVG             ;
    std::string                      pseudoType        ;

    std::vector<DomNodeAttribute>    attributes        ;

    int                              childNodeCount    ;
    std::vector<DomNode>             children          ;

    std::vector<DomNode>             pseudoElements    ;
    std::vector<DomNode>             shadowRoots       ;

    std::string                      shadowRootType    ;
    


    void clear()
    {
        xmlVersion        .clear();
        documentURL       .clear();
                          
        backendNodeId     = 0;
        localName         .clear();
        nodeId            = 0;
        nodeName          .clear();
        nodeType          = 0;
        nodeValue         .clear();
        parentId          = 0;
        publicId          .clear();
        systemId          .clear();
        frameId           .clear();
        compatibilityMode .clear();
        isScrollable      = false;
        isSVG             = false;
        pseudoType        .clear();
                          
        attributes        .clear();
                          
        childNodeCount    = 0;
        children          .clear();

        pseudoElements    .clear();
        shadowRoots       .clear();

        shadowRootType    .clear();
    }

    // void to_json(json& j) const
    // {
    //     j = json::object();
    //  
    //     // if (!browser.empty())
    //        j["Browser"] = browser;
    //  
    //     // if (!protocolVersion.empty())
    //        j["Protocol-Version"] = protocolVersion;
    //  
    //     // if (!userAgent.empty())
    //        j["User-Agent"] = userAgent;
    //  
    //     // if (!v8Version.empty())
    //        j["V8-Version"] = v8Version;
    //  
    //     // if (!webkitVersion.empty())
    //        j["WebKit-Version"] = webkitVersion;
    //  
    //     // if (!webSocketDebuggerUrl.empty())
    //        j["webSocketDebuggerUrl"] = webSocketDebuggerUrl;
    //  
    //     if (!debuggerVersion.empty())
    //        j["Debugger-Version"] = debuggerVersion;
    //  
    // }

    void from_json(const json& j)
    {
        if (!j.is_object())
            throw std::runtime_error("DomNode: object expected");

        clear();

        // // special iterator member functions for objects
        // for (json::iterator it = o.begin(); it != o.end(); ++it) {
        //   std::cout << it.key() << " : " << it.value() << "\n";
        // }
        //  
        // // the same code as range for
        // for (auto& el : o.items()) {
        //   std::cout << el.key() << " : " << el.value() << "\n";
        // }



        for (const auto& [key, value] : j.items())
        {
            //std::cout << key << " : " << value << "\n";

            if (key=="xmlVersion")
            {
                xmlVersion = value.get<std::string>();
            }

            else if (key=="documentURL")
            {
                documentURL = value.get<std::string>();
            }

            else if (key=="baseURL")
            {
                baseURL = value.get<std::string>();
            }

            else if (key=="backendNodeId")
            {
                backendNodeId = value.get<int>();
            }

            else if (key=="localName")
            {
                localName = value.get<std::string>();
            }

            else if (key=="nodeId")
            {
                nodeId = value.get<int>();
            }

            else if (key=="nodeName")
            {
                nodeName = value.get<std::string>();
            }

            else if (key=="nodeType")
            {
                nodeType = value.get<int>();
            }

            else if (key=="nodeValue")
            {
                nodeValue = value.get<std::string>();
            }

            else if (key=="parentId")
            {
                parentId = value.get<int>();
            }

            else if (key=="publicId")
            {
                publicId = value.get<std::string>();
            }

            else if (key=="systemId")
            {
                systemId = value.get<std::string>();
            }

            else if (key=="frameId")
            {
                frameId = value.get<std::string>();
            }

            else if (key=="compatibilityMode")
            {
                compatibilityMode = value.get<std::string>();
            }

            else if (key=="pseudoType")
            {
                pseudoType = value.get<std::string>();
            }

            else if (key=="isScrollable")
            {
                isScrollable = value.get<bool>();
            }

            else if (key=="isSVG")
            {
                isSVG = value.get<bool>();
            }

            else if (key=="attributes")
            {
                // iterate the array
                std::size_t cnt = 0;
                DomNodeAttribute attr;
                for (json::const_iterator it=value.begin(); it!=value.end(); ++it, ++cnt)
                {
                    // std::cout << *it << '\n';
                    if ((cnt&1)==0)
                    {
                        // attr name
                        attr.name = it->get<std::string>();
                    }
                    else
                    {
                        // attr value
                        attr.value = it->get<std::string>();
                        attributes.push_back(attr);
                        attr.clear();
                    }
                }
            }

            else if (key=="childNodeCount")
            {
                childNodeCount = value.get<int>();
            }

            else if (key=="children")
            {
                
                for (json::const_iterator it=value.begin(); it!=value.end(); ++it)
                {
                    DomNode node;
                    node.from_json(*it);
                    children.push_back(node);
                }
            }

            else if (key=="pseudoElements")
            {
                for (json::const_iterator it=value.begin(); it!=value.end(); ++it)
                {
                    DomNode node;
                    node.from_json(*it);
                    pseudoElements.push_back(node);
                }
            }

            else if (key=="shadowRootType")
            {
                shadowRootType = value.get<std::string>();
            }

            else if (key=="shadowRoots")
            {
                for (json::const_iterator it=value.begin(); it!=value.end(); ++it)
                {
                    DomNode node;
                    node.from_json(*it);
                    shadowRoots.push_back(node);
                }
            }

            else
            {
                // кидаем исключение на любой неизвестный элемент - и будем добавлять обработчики по мере обнаружения таких элементов
                throw std::runtime_error("DomNode::from_json: Value not handled: " + key);
            }

        }

    }


}; // struct DomNode

// inline
// void to_json(json& j, const JsonVersionResponse &r)
// {
//     r.to_json(j);
// }
//  
inline
void from_json(const json& j, DomNode &n)
{
    n.from_json(j);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct DomDocument
{
    DomNode    rootNode;

    void from_json(const json& j)
    {
        if (!j.is_object())
            throw std::runtime_error("DomDocument: object expected");

        if (j.contains("root"))
            rootNode.from_json(j["root"]);
    }

}; // struct DomDocument

inline
void from_json(const json& j, DomDocument &d)
{
    d.from_json(j);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/JsonVersionResponse.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
