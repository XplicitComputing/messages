#pragma once

#include "protocol_device.hpp"


namespace xc {
namespace messages {

    template< typename Handler
            , template <typename, template <typename> class...> class Reader
            , template <typename> class... MsgReaders
            >
    struct data_device_t : xc::messages::protocol_device_t<Handler, Reader, MsgReaders...>
    {
        template <typename... T>
        data_device_t(T&&... args)
        : protocol_device_t<Handler, Reader, MsgReaders...>(std::forward<T>(args)...)
        { }
    };

} }  // namespace xc::messages
