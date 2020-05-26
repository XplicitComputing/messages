/* This is a generated file.  DO NOT EDIT! */


#pragma once

#include <CORE/Messages/message_types.hpp>

#include <CORE/Messages/meta.pb.h>
#include <CORE/Messages/spatial.pb.h>
#include <CORE/Messages/system.pb.h>
#include <CORE/Messages/vector.pb.h>

#include <cassert>
#include <cstdint>
#include <memory>


namespace xc {
namespace messages {

#define MESSAGE_READER_T(name, support)                                                             \
                                                                                                    \
    /* Client & Server -side messaging */                                                           \
                                                                                                    \
    template <typename Handler>                                                                     \
    class message_reader_ ## name                                                                   \
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


    // define message_reader_t 's

    MESSAGE_READER_T(Representation, xc::messages::message_tag_t<Messages::Representation>);
    MESSAGE_READER_T(Visual, xc::messages::message_tag_t<Messages::Visual>);
    MESSAGE_READER_T(GraphicsControl, xc::messages::message_tag_t<Messages::GraphicsControl>);
    MESSAGE_READER_T(SliceControl, xc::messages::message_tag_t<Messages::SliceControl>);
    MESSAGE_READER_T(TracersControl, xc::messages::message_tag_t<Messages::TracersControl>);
    MESSAGE_READER_T(Meta, xc::messages::message_tag_t<Messages::Meta>);
    MESSAGE_READER_T(Zone, xc::messages::message_tag_t<Messages::Zone>);
    MESSAGE_READER_T(Topology, xc::messages::message_tag_t<Messages::Topology>);
    MESSAGE_READER_T(GridControl, xc::messages::message_tag_t<Messages::GridControl>);
    MESSAGE_READER_T(MeshControl, xc::messages::message_tag_t<Messages::MeshControl>);
    MESSAGE_READER_T(TreeControl, xc::messages::message_tag_t<Messages::TreeControl>);
    MESSAGE_READER_T(NetworkControl, xc::messages::message_tag_t<Messages::NetworkControl>);
    MESSAGE_READER_T(Variables, xc::messages::message_tag_t<Messages::Variables>);
    MESSAGE_READER_T(Function, xc::messages::message_tag_t<Messages::Function>);
    MESSAGE_READER_T(Command, xc::messages::message_tag_t<Messages::Command>);
    MESSAGE_READER_T(Script, xc::messages::message_tag_t<Messages::Script>);
    MESSAGE_READER_T(Rule, xc::messages::message_tag_t<Messages::Rule>);
    MESSAGE_READER_T(Model, xc::messages::message_tag_t<Messages::Model>);
    MESSAGE_READER_T(System, xc::messages::message_tag_t<Messages::System>);
    MESSAGE_READER_T(Revision, xc::messages::message_tag_t<Messages::Revision>);
    MESSAGE_READER_T(Extrema, xc::messages::message_tag_t<Messages::Extrema>);
    MESSAGE_READER_T(Vector32, xc::messages::message_tag_t<Messages::Vector32>);
    MESSAGE_READER_T(Vector64, xc::messages::message_tag_t<Messages::Vector64>);
    MESSAGE_READER_T(Indices, xc::messages::message_tag_t<Messages::Indices>);
    MESSAGE_READER_T(Elements, xc::messages::message_tag_t<Messages::Elements>);

} }  // namespace xc::messages
