#pragma once

#include <CORE/Messages/message_types.hpp>

#include <CORE/Messages/meta.pb.h>
#include <CORE/Messages/vector.pb.h>

#include <cassert>
#include <cstdint>
#include <memory>


namespace xc {
namespace messages {

#define DATA_READER_T(name, support)                                                                \
                                                                                                    \
    /* Client-side data messaging */                                                                \
                                                                                                    \
    template <typename Handler>                                                                     \
    class data_reader_ ## name                                                                      \
    {                                                                                               \
    public:                                                                                         \
                                                                                                    \
        using support_t = support;                                                                  \
                                                                                                    \
                                                                                                    \
        message_reader_ ## name()                                                                   \
        {                                                                                           \
            /* Constructor */                                                                       \
        }                                                                                           \
                                                                                                    \
                                                                                                    \
        void                                                                                        \
        operator ()(std::shared_ptr<Handler> handler)                                               \
        {                                                                                           \
            this->handler_ = handler;                                                               \
        }                                                                                           \
                                                                                                    \
                                                                                                    \
        template <typename Context = void>                                                          \
        bool                                                                                        \
        operator()(std::uint16_t version,                                                           \
                   std::uint64_t timestamp,                                                         \
                   const std::uint8_t* payload,                                                     \
                   std::uint64_t size,                                                              \
                   Context* context = nullptr)                                                      \
        {                                                                                           \
            /* forward payload to Handler::on_message */                                            \
                                                                                                    \
            assert(payload);                                                                        \
                                                                                                    \
            /* call the overload */                                                                 \
            return this->handler_->on_message(xc::messages::message_tag<support_t::id>(),           \
                                              version,                                              \
                                              timestamp,                                            \
                                              payload,                                              \
                                              size,                                                 \
                                              context);                                             \
        }                                                                                           \
                                                                                                    \
                                                                                                    \
    private:                                                                                        \
                                                                                                    \
        std::shared_ptr<Handler> handler_;                                                          \
    };

/**/


    // define data_reader_t 's

    DATA_READER_T(Meta, xc::messages::message_tag_t<Messages::Meta>);
    DATA_READER_T(Vector32, xc::messages::message_tag_t<Messages::Vector32>);
    DATA_READER_T(Vector64, xc::messages::message_tag_t<Messages::Vector64>);

} }  // namespace xc::messages
