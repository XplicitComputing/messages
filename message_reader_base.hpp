#pragma once

#include "CORE/Common/io/network_utils.hpp"
#include "CORE/Common/io/payload.hpp"
#include "CORE/Common/serialization/detail/message_header.hpp"
#include "CORE/Common/serialization/detail/packet_header.hpp"
#include "CORE/Common/type_traits/underlying.hpp"
#include "CORE/Messages/function_enumerants.hpp"
#include "CORE/Messages/message_types.hpp"
#include "CORE/Messages/meta.pb.h"
#include "CORE/Messages/spatial.pb.h"
#include "CORE/Messages/system.pb.h"
#include "CORE/Messages/vector.pb.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>




// use `XCDEBUG' - because we *may* want to keep asserts in the code and not define `NDEBUG'
#ifdef XCDEBUG
# define XCLOG(msg) \
    std::cerr << std::endl << "[[" << msg << "]]" << std::endl;
  /**/
#else
# define XCLOG(msg)
#endif



namespace xc {
namespace messages {

    /* Packet ver-1.0 function argument index helper */
    /* Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN } */

namespace detail {

    template <std::uint16_t N, std::uint16_t Version>
    struct parameter_index;

    template <std::uint16_t N>                                                                          // N is indexed from 1
    struct parameter_index<N, 1>
    {
        // Ver-1.0 protocol

        static constexpr
        std::uint16_t value = N + 1;                                                                    // make it 0-indexed


        static constexpr
        std::uint16_t funcname = 0;

        static constexpr
        std::uint16_t msgtype = 1;

        static constexpr
        std::uint16_t return_slot = 2;


        static constexpr
        std::uint16_t zero_offset = 2;                                                                  // the head of all message payloads are preceded by meta-data of  { FunctionName, MsgType }


        static_assert(N >= 1, "bad argument value");
    };

}  // namespace xc::messages::detail


    template <std::uint16_t N, std::uint16_t Version>
    static constexpr
    std::uint16_t arg = xc::messages::detail::parameter_index<N, Version>::value;


    template <std::uint16_t Version>
    static constexpr
    std::int16_t funcname = xc::messages::detail::parameter_index<1, Version>::funcname;


    template <std::uint16_t Version>
    static constexpr
    std::int16_t msgtype = xc::messages::detail::parameter_index<1, Version>::msgtype;


    template <std::uint16_t Version>
    static constexpr
    std::uint16_t return_slot = xc::messages::detail::parameter_index<1, Version>::return_slot;         // client-side utlitiy


    template <std::uint16_t Version>
    static constexpr
    std::uint16_t zero_offset = xc::messages::detail::parameter_index<1, Version>::zero_offset;


    // ----


    template <std::uint16_t Version>
    struct msgtype_test
    {
        template <typename T>
        bool operator()(const T&) const noexcept
        {
            // pass-through

            return true;
        }
    };


    template < >
    struct msgtype_test<1>                                                                              // Ver-1.0 specialization
    {
        using hash_t = xc::messages::MsgType;

        template <typename T>
        bool operator()(const T&) const noexcept
        {
            // pass-through

            return true;
        }


        bool operator()(const hash_t& e) const noexcept
        {
            // Validate ver-1.0 protocol constraint
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }

            auto ee = static_cast<std::underlying_type_t<hash_t> > (e);
            return ee == xc::underlying(hash_t::Callable_) || ee == xc::underlying(hash_t::Data_) || ee == xc::underlying(hash_t::Log_);
        }
    };


    // ----

    template <typename S, typename HashType>
    class message_reader_t;

    // ----


    template <typename HashType>
    struct message_reader
    {
        using hash_t = HashType;


        static
        xc::messages::FunctionName
        get_funcname_hash(std::uint16_t version, std::uint8_t* ptr, std::size_t size)
        {
            // Query function-name hash within the packet
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }

            switch (version)
            {
                case 1 :
                {
                    static constexpr std::uint16_t Version = 1;

                    return xc::messages::message_reader<hash_t>::template get_arg<xc::messages::FunctionName, xc::messages::funcname<Version>, Version>(ptr, size);
                }
            }

            return xc::messages::FunctionName::noval_t_;
        }


        template <std::uint16_t N>
        static
        hash_t
        msgtype(uint8_t* ptr,
                std::size_t size,
                std::uint8_t* next = nullptr)
        {
            // Query message-type of the Nth message-header (0-indexed) in a packet
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }
            //
            // pkthdr := [pktlen][version][sxn.id][seq.nbr][nbr.msgs][send.dt]
            // msghdr := [msg.type][msg.len][ctr.type][ctr.ele][ctr.size]

            const auto& pkthdr     = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            //std::uint16_t ver    = pkthdr.version();
            //std::uint64_t seqnbr = pkthdr.sequence_number();
            std::uint16_t msgcnt   = pkthdr.message_count();

            std::uint8_t* begin = ptr;
            std::uint8_t* end = ptr + size;
            begin += sizeof(xc::serialization::detail::packet_header_t);                                // jump over pkthdr

            assert(N < msgcnt && "index out of range");

            const auto* msghdr  = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            if (N && next && (next < end))
            {
                begin = next;
                msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            }
            else
            {
                for (auto i = 0; i != N; ++i)
                {
                    begin += (msghdr->block_size() + 1);
                    msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
                }
            }
            assert(end != begin && "corrupted packet");

            auto* payload = begin + sizeof(xc::serialization::detail::message_header_t);

            //return static_cast<hash_t> (msghdr->message_type());

            // return underlying data-type..
            xc::messages::message_tag_t<hash_t> arg_wrapper;
            arg_wrapper.ParseFromArray(payload, msghdr->payload_size());
            return arg_wrapper.operator()();
        }


        template <std::uint16_t N>
        static
        std::uint64_t
        ctrsize(const uint8_t* ptr,
                std::size_t size,
                const std::uint8_t* next = nullptr)
        {
            // Query message container-size in the event of a composite type, of the Nth message-header
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }
            //
            // pkthdr := [pktlen][version][sxn.id][seq.nbr][nbr.msgs][send.dt]
            // msghdr := [msg.type][msg.len][ctr.type][ctr.ele][ctr.size]

            const auto& pkthdr     = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            //std::uint16_t ver    = pkthdr.version();
            //std::uint64_t seqnbr = pkthdr.sequence_number();
            std::uint16_t msgcnt   = pkthdr.message_count();

            const std::uint8_t* begin = ptr;
            const std::uint8_t* end = ptr + size;
            begin += sizeof(xc::serialization::detail::packet_header_t);                                // jump over pkthdr

            assert(N < msgcnt && "index out of range");

            const auto* msghdr  = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            if (N && next && (next < end))
            {
                begin = next;
                msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            }
            else
            {
                for (auto i = 0; i != N; ++i)
                {
                    begin += (msghdr->block_size() + 1);
                    msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
                }
            }
            assert(end != begin && "corrupted packet");

            return msghdr->container_size();
        }


        template< typename RT
                , std::uint16_t N
                , std::uint16_t Version
                >
        static
        RT
        get_arg(std::uint8_t* ptr,
                std::size_t size,
                std::uint8_t* next = nullptr,
                std::enable_if_t<Version == 1>* = 0)
        {
            // Get deserialized Nth argument of Pkt (ver-1.0 protocol, zero-indexed)
            // next == nullptr ? time-complexity O(N) : O(1)
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }
            //
            // pkthdr := [pktlen][version][sxn.id][seq.nbr][nbr.msgs][send.dt]
            // msghdr := [msg.type][msg.len][ctr.type][ctr.ele][ctr.size]

            const auto& pkthdr   = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            //std::uint16_t ver    = pkthdr.version();
            //std::uint64_t seqnbr = pkthdr.sequence_number();
            std::uint16_t msgcnt = pkthdr.message_count();

            std::uint8_t* begin = ptr;
            std::uint8_t* end = ptr + size;
            begin += sizeof(xc::serialization::detail::packet_header_t);                                // jump over pkthdr

            assert(N < msgcnt && "index out of range");

            const auto* msghdr  = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            if (N && next && (next < end))
            {
                // optimization - we have a `next', avoid the for-loop!

                begin = next;
                msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            }
            else
            {
                for (auto i = 0; i != N; ++i)
                {
                    begin += (msghdr->block_size() + 1);
                    msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
                }
            }
            assert(end != begin && "corrupted packet");
            if (xc::messages::template type_msg<RT>::type::value != msghdr->message_type())
            {
                const std::string got = xc::messages::msgtype_name(msghdr->message_type());
                const std::string expected = xc::messages::message_tag_t<RT>::tag::name;
                std::cerr << std::endl << "Error, mismatched types: expected: ‘" << expected << "‘, got: ‘" << got << "‘" << std::endl;
            }
            assert(xc::messages::template type_msg<RT>::type::value == msghdr->message_type() && "mismatched types");

            auto* payload = begin + sizeof(xc::serialization::detail::message_header_t);

            auto mt = msghdr->message_type();
            if (!N && (mt != xc::underlying(hash_t::FunctionName_)))                                    // a protocol ver-1.0 requirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }
            if (N == 1 && (mt != xc::underlying(hash_t::MsgType_)))                                     // a protocol ver-1.0 requirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }

            // point to next arg..
            next = begin;
            next += (msghdr->block_size() + 1);
            if (next >= end)
            {
                next = nullptr;
            }

            // return underlying data-type..
            xc::messages::message_tag_t<RT> arg_wrapper;
            arg_wrapper.ParseFromArray(payload, msghdr->payload_size());

            decltype(arg_wrapper.operator()()) e = arg_wrapper.operator()();
            if (N == 1 && !xc::messages::msgtype_test<Version>().operator()(e))                         // test for protocol reqirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }
            return e;
        }


        template< typename RT
                , std::uint16_t Version
                >
        static
        RT
        get_arg(std::uint16_t N,
                std::uint8_t* ptr,
                std::size_t size,
                std::uint8_t* next = nullptr,
                std::enable_if_t<Version == 1>* = 0)
        {
            // Get deserialized Nth argument of Pkt (ver-1.0 protocol, zero-indexed)
            // next == nullptr ? time-complexity O(N) : O(1)
            //
            // Ver-1.0 protocol:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }
            //
            // pkthdr := [pktlen][version][sxn.id][seq.nbr][nbr.msgs][send.dt]
            // msghdr := [msg.type][msg.len][ctr.type][ctr.ele][ctr.size]

            const auto& pkthdr   = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            //std::uint16_t ver    = pkthdr.version();
            //std::uint64_t seqnbr = pkthdr.sequence_number();
            std::uint16_t msgcnt = pkthdr.message_count();

            std::uint8_t* begin = ptr;
            std::uint8_t* end = ptr + size;
            begin += sizeof(xc::serialization::detail::packet_header_t);                                 // jump over pkthdr

            assert(N < msgcnt && "index out of range");

            const auto* msghdr  = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            if (N && next && (next < end))
            {
                // optimization - we have a `next', avoid the for-loop!

                begin = next;
                msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
            }
            else
            {
                for (auto i = 0; i != N; ++i)
                {
                    begin += (msghdr->block_size() + 1);
                    msghdr = reinterpret_cast<const xc::serialization::detail::message_header_t *> (begin);
                }
            }
            assert(end != begin && "corrupted packet");
            if (xc::messages::template type_msg<RT>::type::value != msghdr->message_type())
            {
                const std::string got = xc::messages::msgtype_name(msghdr->message_type());
                const std::string expected = xc::messages::message_tag_t<RT>::tag::name;
                std::cerr << std::endl << "Error, mismatched types: expected: ‘" << expected << "‘, got: ‘" << got << "‘" << std::endl;
            }
            assert(xc::messages::template type_msg<RT>::type::value == msghdr->message_type() && "mismatched types");

            auto* payload = begin + sizeof(xc::serialization::detail::message_header_t);

            auto mt = msghdr->message_type();
            if (!N && (mt != xc::underlying(hash_t::FunctionName_)))                                    // a protocol ver-1.0 requirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }
            if (N == 1 && (mt != xc::underlying(hash_t::MsgType_)))                                     // a protocol ver-1.0 requirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }

            // point to next arg..
            next = begin;
            next += (msghdr->block_size() + 1);
            if (next >= end)
            {
                next = nullptr;
            }

            // return underlying data-type..
            xc::messages::message_tag_t<RT> arg_wrapper;
            arg_wrapper.ParseFromArray(payload, msghdr->payload_size());

            decltype(arg_wrapper.operator()()) e = arg_wrapper.operator()();
            if (N == 1 && !xc::messages::msgtype_test<Version>().operator()(e))                         // test for protocol reqirement
            {
                assert(false && "message is not protocol ver-1.0");
                next = nullptr;
                return RT{ };
            }
            return e;
        }
    };



    // ----



    template< typename S
            , typename HashType
            , template <typename, typename> class Derived
            >
    class message_reader_base_t : public message_reader<HashType>
    {
    public:

        using hash_t = HashType;
        using derived_t = Derived<S, HashType>;


        message_reader_base_t(S* subject)
        : subject_(subject)
        {
            assert(subject != nullptr && "attempted to bind nullptr to message_reader_base_t subject");
        }


        template <typename F>
        bool
        operator ()(xc::io::payload_t<hash_t>& e, F&& ftor)
        {
            return this->operator()(e.timestamp(), e.sa_address(), e.data(), e.size(), e.operator()(), std::forward<F>(ftor));
        }


        template <typename F>
        bool
        operator ()(std::uint64_t timestamp,
                    xc::io::sockaddr_t sa,
                    std::uint8_t* ptr,                                                                   // ptr  := the packet
                    std::size_t size,                                                                    // size := length of packet (incl header)
                    typename xc::io::payload_t<hash_t>::result_type& context,
                    F&& ftor)
        {
            // consume message packet..

            assert(ptr != nullptr);

            const auto& pkthdr = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            if (!pkthdr.has_payload())
            {
                context.reset();

                return false;                                                                            // empty (eot|heartbeat) - ignore
            }

            // if we have a payload, we aught to have at least one msg-hdr!
            assert(size >= (sizeof(xc::serialization::detail::packet_header_t) + sizeof(xc::serialization::detail::message_header_t)));

            std::uint16_t sxnid = pkthdr.session_id();
            const std::string ipaddr(xc::io::binary_to_dotted(sa));

            return this->process_packet_(timestamp, ipaddr, sxnid, ptr, size, context, std::forward<F>(ftor));
        }


    protected:

        template <typename F>
        bool
        process_packet_(std::uint64_t /*timestamp*/,
                        const std::string& /*ipaddr*/,
                        std::uint16_t /*sxnid*/,
                        std::uint8_t* ptr,
                        std::size_t size,
                        typename xc::io::payload_t<hash_t>::result_type& context,
                        F&& ftor)
        {
            const auto& pkthdr   = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            std::uint16_t ver    = pkthdr.version();
            //std::uint64_t seqnbr = pkthdr.sequence_number();
            //std::uint16_t msgcnt = pkthdr.message_count();

            const std::uint8_t* begin = ptr;
            //const std::uint8_t* end = ptr + size;
            begin += sizeof(xc::serialization::detail::packet_header_t);                                // jump over pkthdr

            switch (ver)
            {
                case 1:     // Protocol v1.0:  Pkt { hash(fn-name), arg1, .., argN }
                {
                    static constexpr std::uint16_t Version = 1;

                    return static_cast<derived_t *> (this)->template process_packet<Version>(ptr, size, context, std::forward<F>(ftor));
                }


                default:
                {
                    std::cerr << std::endl << "Packet Failure: unsupported protocol!" << std::endl;
                    return false;
                }
            }
            return false;
        }



        S* subject_;        
    };

} }  // namespace xc::messages
