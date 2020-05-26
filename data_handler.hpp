#pragma once

#include "message_types.hpp"

#include <cstdint>
#include <type_traits>

#include <iostream>


namespace xc {
namespace messages {

    /* Client-side data handler */

    class data_handler_t
    {
    public:

        constexpr data_handler_t() = default;


        void
        on_complete(std::uint64_t timestamp)
        {
            //this->publish(timestamp);
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
    };

} }  // namespace xc::messages
