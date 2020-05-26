#pragma once

#include "message_types.hpp"

#include <cstdint>
#include <type_traits>

#include <iostream>


namespace xc {
namespace messages {

    /* Client-side message handler */

    class message_handler_t
    {
    public:

        constexpr message_handler_t() = default;


        void
        on_complete(std::uint64_t timestamp)
        {
            //this->publish(timestamp);
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::GraphicsControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::ColorScheme>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Visual>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Slice>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Tracers>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Meta>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Zone>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Topology>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::GridControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::MeshControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::TreeControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::NetworkControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Variables>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Function>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Command>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Script>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Rule>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Model>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::System>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Setup>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Revision>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Extrema>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Vector32>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Vector64>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }


        template <typename Context>
        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Elements>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const std::uint8_t* payload,
                   std::uint64_t size,
                   const Context* context)
        {
            // deserialize & do work..

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;

            T t;
            t.ParseFromArray(payload, size);

            // do work..
            std::cout << std::endl << t.DebugString() << std::endl;

            return true;
        }
    };

} }  // namespace xc::messages
