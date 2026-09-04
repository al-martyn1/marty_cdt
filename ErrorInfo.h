/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief ErrorInfo

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>

//
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/ErrorInfo.h"
// marty::chrome_devtools_protocol::
// marty::cdp::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// "error": {
//   "code": -32000,
//   "message": "No resource with given identifier found"
// }

struct ErrorInfo
{
    int            code;
    std::string    message;

}; // struct ErrorInfo


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( ErrorInfo
, code
, message
)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdp = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/ErrorInfo.h"
// marty::chrome_devtools_protocol::
// marty::cdp::
