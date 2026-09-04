/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief MessageIdVariant

    Repository: https://github.com/al-martyn1/marty_cdt
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "ResponseError.h"

//
#include <string>
#include <vector>
#include <variant>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// #include "marty_cdt/MessageIdVariant.h"
// marty::chrome_devtools_protocol::
// marty::cdt::

namespace marty {
namespace chrome_devtools_protocol {

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
using MessageIdVariant = std::variant< unsigned        // id
                                     , std::string     // method
                                     , ResponseError   // Ошибка, если в сообщении с ID вместо result находится error. При ошибке id нельзя терять, но надо нагрузить и полем с ошибкой
                                     >;
//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
} // namespace chrome_devtools_protocol

namespace cdt = chrome_devtools_protocol;

} // namespace marty

// #include "marty_cdt/MessageIdVariant.h"
// marty::chrome_devtools_protocol::
// marty::cdt::
