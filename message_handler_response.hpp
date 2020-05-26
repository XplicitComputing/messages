#pragma once

#include "message_types.hpp"
#include "noval.hpp"
#include "CORE/Common/serialization/message_packer.hpp"

#include <cstdint>
#include <type_traits>

#include <iostream>


namespace xc {
namespace messages {

    /* Server-side (response) message handler */

    class message_handler_response_t
    {
    public:

        explicit message_handler_response_t(std::uint16_t session, std::uint16_t version)
        : packer_(session, version)
        { }


        void
        on_complete(std::uint64_t timestamp)
        {
            //this->publish(timestamp);
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::GraphicsControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::ColorScheme>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Visual>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Slice>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Tracers>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Meta>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Zone>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Topology>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::GridControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::MeshControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::TreeControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::NetworkControl>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Variables>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Function>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Command>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Script>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Rule>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Model>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::System>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Setup>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Revision>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Extrema>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Vector32>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Vector64>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


        bool
        on_message(const xc::messages::message_tag<xc::messages::MsgType::Elements>& tag,
                   std::uint16_t protocol_version,
                   std::uint64_t timestamp,
                   const uint8_t* payload,
                   std::uint64_t size,
                   xc::serialization::message_packer_t::type* context)
        {
            // deserialize & do work..
            // Note:  we return server response through the context object

            using T = std::remove_const_t<std::remove_reference_t<typename decltype(tag)::type> >;
            using msg_response_t = noval_t;                                                                                         // potentially any of protobuf Message::X
            using data_response_t = noval_t;                                                                                        // (optional), and possibly one of Messages::{ Meta, Vector32, Vector64 }

            T t;
            t.ParseFromArray(payload, size);

            // do work.. - do something with deserialized `t'
            std::cout << std::endl << t.DebugString() << std::endl;


            if constexpr (!std::is_same<msg_response_t, noval_t>::value)
            {
                // do work.. - provide a response | result (if applicable), conveniently serializing so can return to client

                this->packer_ & msg_response_t{ };                                                                                  // init the msg-response and serialize it
                if constexpr (!std::is_same<data_response_t, noval_t>::value)
                {
                    this->packer_ & data_response_t{ };                                                                             // init the data-response and serialize it
                }
                auto p = this->packer_.operator()();                                                                                // seal!

                context->first = p.first;                                                                                           // buffer containing serialized response
                context->second = p.second;                                                                                         // byte-size of serialized response
            }
            return true;
        }


    private:

        xc::serialization::message_packer_t packer_;
    };

} }  // namespace xc::messages
