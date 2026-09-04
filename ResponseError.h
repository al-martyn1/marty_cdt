/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief ResponseError

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "ErrorInfo.h"
//
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/ResponseError.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
struct ResponseError
{
    unsigned    id;
    ErrorInfo   errorInfo;

}; // struct ResponseError


// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( ResponseError
// , code
// , message
// )

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/ResponseError.h"
// marty::chrome_devtools_protocol::
// marty::cdp::
