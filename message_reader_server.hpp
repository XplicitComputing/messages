#pragma once

#include "CORE/Common/computer.hpp"
#include "CORE/Common/io/payload.hpp"
#include "CORE/Common/serialization/detail/packet_header.hpp"
#include "CORE/Common/utilities/severity.hpp"
#include "CORE/Common/utilities/forwarder.hpp"
#include "CORE/Common/utilities/time.hpp"
#include "CORE/Messages/function_enumerants.hpp"
#include "CORE/Messages/message_reader_base.hpp"
#include "CORE/Messages/message_types.hpp"
#include "CORE/Messages/meta.pb.h"
#include "CORE/Messages/spatial.pb.h"
#include "CORE/Messages/system.pb.h"
#include "CORE/Messages/vector.pb.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>



namespace xc {


    /* Forward Declarations */

    class Server;


namespace messages {

    template <typename HashType>
    class message_reader_t<xc::Server, HashType> : public xc::messages::message_reader_base_t<xc::Server, HashType, message_reader_t>
    {
    private:

        xc::Computer::endpoint_pair_t cxn_;


    public:

        using hash_t = HashType;
        using subject_t = xc::Server;
        using base_t = xc::messages::message_reader_base_t<subject_t, hash_t, message_reader_t>;


        message_reader_t(subject_t* s, xc::Computer::endpoint_pair_t cxn)
        : base_t(s)
        , cxn_(cxn)
        { }
 

        template <std::uint16_t Version, typename F>
        bool
        process_packet(std::uint8_t* ptr,
                       std::size_t size,
                       typename xc::io::payload_t<hash_t>::result_type& context,
                       F&& ftor)
        {
            // **************************************************************************************************************************************************************
            // -- CALLS ON SERVER FROM CLIENT                               client::spod(..) { server->foo(..); }
            // **************************************************************************************************************************************************************

            // Protocol v1.0:  Pkt { hash(fn-name), arg1, .., argN }
            // Are potential calls Client makes on the Server:  client::foo(..) { server->func(..); }


            auto fn_args_encoded = [](const std::uint8_t* ptr) -> std::uint16_t
            {
                // return count of function-arguments encoded in the Pkt

                const auto pkthdr    = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
                std::uint16_t msgcnt = pkthdr.message_count();

                return msgcnt - xc::messages::zero_offset<Version>;                                                 // version 1.0 protocol:  Pkt { func-name, Callable_|Data_|Log_, param1, .., paramN };  subtract two to account for `func-name' && 'Callable_'|'Data_|Log_'
            };


            auto broadcast_outcome = [ftor = xc::utilities::make_forwarder(ftor)](auto hash, bool result) -> bool   // xc::messages::Log Server::broadcast(const xc::messages::Log& message);
            {
                // will broadcast a log-message to client making request if -DBROADCAST_RESULT is NOT #defined, else..
                // will be broadcast to **all** connected clients otherwise (Ie. if -DBROADCAST_RESULT  is  #defined)


                std::uint64_t timestamp = xc::utilities::time::nanos_since_epoch();

                std::ostringstream oss;
                oss << xc::utilities::time::epoch_to_string(timestamp) << " : [" << xc::messages::funcname_hash_to_string(hash) << "] " << (result ? "client request completed with success" : "client request failure!");
                xc::messages::Log message((result ? xc::utilities::severity::info : xc::utilities::severity::error), oss.str());


#ifndef BROADCAST_RESULT
                // broadcast log-message to client that put out the request only

                (*ftor)(xc::messages::FunctionName::broadcast, xc::messages::MsgType::Log_, message);
                return true;                                                                                        // *true* :  because we go through ftor for packaging
#else
                // broadcast log-message to **all** connected clients

                (void)this->subject_->broadcast(message);
                return false;                                                                                       // *false* :  because we by-pass ftor packaging semantics
#endif
            };



            // NOTE: unfortunately, the compile-time equivalent has too heavy a CT burden!

            // all messages from client to server are "server-callable" packets!
            xc::messages::FunctionName funcname_hash = xc::messages::message_reader<hash_t>::template get_arg<xc::messages::FunctionName, xc::messages::funcname<Version>, Version>(ptr, size);
            auto mt = xc::messages::message_reader<hash_t>::template msgtype<xc::messages::msgtype<Version> >(ptr, size);
            assert((mt == xc::messages::MsgType::Callable_ || mt == xc::messages::MsgType::Log_) && "message is not protocol ver-1.0");        // all messages from client to server should be "server-callable" packets! ..or a (broadcast) log-message from going from server onward to client!
            switch (funcname_hash)
            {
                // FIXME:  case-statements can be auto-generated!

                case xc::messages::FunctionName::addAlgorithm :                                                     // bool Server::addAlgorithm(std::int64_t sys, std::string algorithm);
                {
                    XCLOG("message_reader_server.addAlgorithm")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto algorithm = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->addAlgorithm(sys, algorithm);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::addCondition :                                                     // Messages::Command Server::addCondition(std::int64_t sys, std::string algorithm);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.addCondition")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto algorithm = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Command result = this->subject_->addCondition(sys, algorithm);
                    context.reset(result, hash_t::Command);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::addModel :                                                         // bool Server::addModel(std::int64_t sys, std::string model);
                {
                    XCLOG("message_reader_server.addModel")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto model = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->addModel(sys, model);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::addRegionToInstruction :                                           // bool Server::addRegionToInstruction(std::int64_t sys, int instr, int id, int dim, bool overwrite);
                {
                    XCLOG("message_reader_server.addRegionToInstruction")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto instr = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<3, Version>, Version>(ptr, size, next);
                    auto dim = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<4, Version>, Version>(ptr, size, next);
                    auto overwrite = xc::messages::message_reader<hash_t>::template get_arg<bool, arg<5, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->addRegionToInstruction(sys, instr, id, dim, overwrite);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::addSlice :                                                         // bool Server::addSlice(std::int64_t sys, Messages::SliceControl control);
                {
                    XCLOG("message_reader_server.addSlice")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto control = xc::messages::message_reader<hash_t>::template get_arg<Messages::SliceControl, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->addSlice(sys, control);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::assignSymbol :                                                     // bool Server::assignSymbol(std::int64_t sys, std::string filepath);
                {
                    XCLOG("message_reader_server.assignSymbol")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto filepath = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->assignSymbol(sys, filepath);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::broadcast :                                                        // xc::messages::Log Server::broadcast(const xc::messages::Log& message);
                {
                    // will be broadcast to client making request if -DRETURN_RECEIPT is #defined, else..
                    // will be broadcast to **all** connected clients otherwise

                    //XCLOG("message_reader_server.broadcast")


                    std::uint8_t* next = nullptr;
                    auto message = xc::messages::message_reader<hash_t>::template get_arg<xc::messages::Log, arg<1, Version>, Version>(ptr, size, next);


#ifdef RETURN_RECEIPT
                    // broadcast return-receipt to client that put out the request only

                    context.reset(message, hash_t::Log_);
                    ftor(funcname_hash, xc::messages::MsgType::Log_, context);
                    return true;                                                                                    // *true* :  because we go through ftor for packaging
#else
                    // broadcast return-receipt to **all** clients

                    (void)this->subject_->broadcast(message);
                    return false;                                                                                   // *false* :  because we by-pass ftor packaging semantics
#endif
                }


                case xc::messages::FunctionName::canSolve :                                                         // bool Server::canSolve(std::int64_t sys);
                {
                    XCLOG("message_reader_server.canSolve")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->canSolve(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::clearSubsystems :                                                  // bool Server::clearSubsystems(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.clearSubsystems")

                    //const auto& pkthdr   = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
                    //std::uint16_t msgcnt = pkthdr.message_count();

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->clearSubsystems(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::convertToMesh :                                                    // bool Server::convertToMesh(std::int64_t sys);
                {
                    XCLOG("message_reader_server.convertToMesh")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->convertToMesh(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::deleteAllIterations :                                              // bool Server::deleteAllIterations(std::int64_t sys);
                {
                    XCLOG("message_reader_server.deleteAllIterations")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->deleteAllIterations(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::deleteGroup :                                                      // bool Server::deleteGroup(std::int64_t sys, int id);
                {
                    XCLOG("message_reader_server.deleteGroup")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->deleteGroup(sys, id);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::deleteIteration :                                                  // bool Server::deleteIteration(std::int64_t sys);
                {
                    XCLOG("message_reader_server.deleteIteration")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->deleteIteration(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::deleteSystem :                                                     // std::shared_ptr<System> Server::deleteSystem(std::int64_t sys, bool recursive = false, std::string usr = "");
                {
                    XCLOG("message_reader_server.deleteSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    bool recursive(false);
                    std::string usr;
                    switch (fn_args_encoded(ptr))
                    {
                        case 2 :
                        {
                            recursive = xc::messages::message_reader<hash_t>::template get_arg<bool, arg<2, Version>, Version>(ptr, size, next);
                            break;
                        }

                        case 3 :
                        {
                            recursive = xc::messages::message_reader<hash_t>::template get_arg<bool, arg<2, Version>, Version>(ptr, size, next);
                            usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                            break;
                        }
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->deleteSystem(sys, recursive, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::deleteVolume :                                                     // bool Server::deleteVolume(std::int64_t sys, int id);
                {
                    XCLOG("message_reader_server.deleteVolume")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->deleteVolume(sys, id);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::disableSystem :                                                    // bool Server::disableSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.disableSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->disableSystem(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::disableWrite :                                                     // bool Server::disableWrite(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.disableWrite")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->disableWrite(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::enableSystem :                                                     // bool Server::enableSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.enableSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->enableSystem(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::enableWrite :                                                      // bool Server::enableWrite(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.enableWrite")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->enableWrite(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::enqueueJob :                                                       // bool Server::enqueueJob(std::int64_t sys);
                {
                    XCLOG("message_reader_server.enqueueJob")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->enqueueJob(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::eraseJob :                                                         // bool Server::eraseJob(std::int64_t sys);
                {
                    XCLOG("message_reader_server.eraseJob")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->eraseJob(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::eraseRegionFromInstruction :                                       // bool Server::eraseRegionFromInstruction(std::int64_t sys, int instr, int id, int dim);
                {
                    XCLOG("message_reader_server.eraseRegionFromInstruction")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto instr = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<3, Version>, Version>(ptr, size, next);
                    auto dim = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<4, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->eraseRegionFromInstruction(sys, instr, id, dim);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::estimateMeshTime :                                                 // std::time_t Server::estimateMeshTime(std::int64_t sys, std::set<int>& surfs, Messages::MeshControl& mcontrol);
                {
                    // ** client-side blocks for result
 
                    XCLOG("message_reader_server.estimateMeshTime")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::set<std::int32_t> surfs;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        surfs.emplace(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto mcontrol = xc::messages::message_reader<hash_t>::template get_arg<Messages::MeshControl, Version>(arg<3, Version> + offset, ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    // std::time_t ~ std::int64_t
                    std::time_t result = this->subject_->estimateMeshTime(sys, surfs, mcontrol);
                    context.reset(result, hash_t::int64_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::exportFile :                                                       // bool Server::exportFile(std::int64_t sys, std::string fname);
                {
                    XCLOG("message_reader_server.exportFile")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto fname = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->exportFile(sys, fname);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::fuseFaces :                                                        // bool Server::fuseFaces(std::int64_t sys);
                {
                    XCLOG("message_reader_server.fuseFaces")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->fuseFaces(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::getAlgorithmInfo :                                                 // Messages::Function Server::getAlgorithmInfo(std::string algorithmType);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getAlgorithmInfo")

                    std::uint8_t* next = nullptr;
                    auto algorithmType = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<1, Version>, Version>(ptr, size, next);

                    Messages::Function result = this->subject_->getAlgorithmInfo(algorithmType);
                    context.reset(result, hash_t::Function);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getAlgorithmNames :                                                // std::vector<std::string> Server::getAlgorithmNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getAlgorithmNames")

                    //std::uint8_t* next = nullptr;

                    std::vector<std::string> result = this->subject_->getAlgorithmNames();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getAttribute :                                                     // Messages::Vector32 getAttribute(std::int64_t sys, std::string pk);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getAttribute")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto pk = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Vector32 result = this->subject_->getAttribute(sys, pk);
                    context.reset(result, hash_t::Vector32);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getAvailableModels :                                               // std::set<std::string> Server::getAvailableModels(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getAvailableModels")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::set<std::string> result = this->subject_->getAvailableModels(sys);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getAverageResidual :                                               // double Server::getAverageResidual(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getAverageResidual")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    double result = this->subject_->getAverageResidual(sys);
                    context.reset(result, hash_t::double_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getChildNames :                                                    // std::vector<std::string> getChildNames(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getChildNames")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::vector<std::string> result = this->subject_->getChildNames(sys);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getCLcode :                                                        // std::string Server::getCLcode(std::int64_t instructionID);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getCLcode")

                    std::uint8_t* next = nullptr;
                    auto instructionID = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string result = this->subject_->getCLcode(instructionID);
                    context.reset(result, hash_t::string_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getConditionsInfo :                                                // Commands Server::getConditionsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getConditionsInfo")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Commands result = this->subject_->getConditionsInfo(sys);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getDefaultDevice :                                                 // std::string Server::getDefaultDevice();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getDefaultDevice")

                    //std::uint8_t* next = nullptr;

                    std::string result = this->subject_->getDefaultDevice();
                    context.reset(result, hash_t::string_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getDeviceNames :                                                   // std::vector<std::string> Server::getDeviceNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getDeviceNames")

                    //std::uint8_t* next = nullptr;

                    std::vector<std::string> result = this->subject_->getDeviceNames();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getFacesByAngle :                                                  // std::vector<Messages::Zone> Server::getFacesByAngle(std::int64_t sys, std::vector<Messages::Zone> faceIndices, double angle, bool absoluteAngle);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getFacesByAngle")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::vector<Messages::Zone> faceIndices;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<Messages::Zone, Version>(arg<2, Version> + e, ptr, size, next);
                        faceIndices.emplace_back(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto angle = xc::messages::message_reader<hash_t>::template get_arg<double, Version>(arg<3, Version> + offset, ptr, size, next);
                    auto absoluteAngle = xc::messages::message_reader<hash_t>::template get_arg<bool, Version>(arg<4, Version> + offset, ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::vector<Messages::Zone> result = this->subject_->getFacesByAngle(sys, faceIndices, angle, absoluteAngle);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getGeometryNames :                                                 // std::vector<std::string> Server::getGeometryNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getGeometryNames")

                    //std::uint8_t* next = nullptr;

                    std::vector<std::string> result = this->subject_->getGeometryNames();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getInstructionData :                                               // Record Server::getInstructionData(std::int64_t i);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getInstructionData")

                    std::uint8_t* next = nullptr;
                    auto i = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    Record result = this->subject_->getInstructionData(i);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getInstructionRegion :                                             // Messages::Zone Server::getInstructionRegion(std::int64_t i, std::int32_t d);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getInstructioRegion")

                    std::uint8_t* next = nullptr;
                    auto i = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto d = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(i, this->cxn_);

                    Messages::Zone result = this->subject_->getInstructionRegion(i, d);
                    context.reset(result, hash_t::Zone);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getJobsList :                                                      // std::list<Messages::Script> Server::getJobsList();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getJobsList")

                    //std::uint8_t* next = nullptr;

                    std::list<Messages::Script> result = this->subject_->getJobsList();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getModelInfo :                                                     // Messages::Model Server::getModelInfo(std::int64_t sys, std::string modelType);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getModelInfo")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto modelType = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Model result = this->subject_->getModelInfo(sys, modelType);
                    context.reset(result, hash_t::Model);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getModelNames :                                                    // std::vector<std::string> Server::getModelNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getModelNames")

                    //std::uint8_t* next = nullptr;

                    std::vector<std::string> result = this->subject_->getModelNames();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getModelsInfo :                                                    // Models Server::getModelsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getModelsInfo")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Models result = this->subject_->getModelsInfo(sys);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }



                case xc::messages::FunctionName::getParent :                                                        // std::int64_t Server::getParent(std::int64_t uid) const;
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getParent")

                    std::uint8_t* next = nullptr;
                    auto uid = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(uid, this->cxn_);

                    std::int64_t result = this->subject_->getParent(uid);
                    context.reset(result, hash_t::int64_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getPhysicsInfo :                                                   // Messages::Model Server::getPhysicsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getPhysicsInfo")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Model result = this->subject_->getPhysicsInfo(sys);
                    context.reset(result, hash_t::Model);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getPhysicsProperties :                                             // std::set<std::string> Server::getPhysicsProperties(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getPhysicsProperties")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::set<std::string> result = this->subject_->getPhysicsProperties(sys);
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getRootUID :                                                       // std::int64_t Server::getRootUID();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getRootUID")

                    //std::uint8_t* next = nullptr;

                    std::int64_t result = this->subject_->getRootUID();
                    context.reset(result, hash_t::int64_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getServerConfig :                                                  // Messages::Variables Server::getServerConfig();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getServerConfig")

                    //std::uint8_t* next = nullptr;

                    Messages::Variables result = this->subject_->getServerConfig();
                    context.reset(result, hash_t::Variables);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getServerInfo :                                                    // std::string Server::getServerInfo();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getServerInfo")

                    //std::uint8_t* next = nullptr;

                    std::string result = this->subject_->getServerInfo();
                    context.reset(result, hash_t::string_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getServerName :                                                    // std::string Server::getServerName() const;
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getServerName")

                    //std::uint8_t* next = nullptr;

                    std::string result = this->subject_->getServerName();
                    context.reset(result, hash_t::string_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getSolverInfo :                                                    // Messages::Script Server::getSolverInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getSolverInfo")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Script result = this->subject_->getSolverInfo(sys);
                    context.reset(result, hash_t::Script);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getSystemNames :                                                   // std::vector<std::string> Server::getSystemNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getSystemNames")

                    //std::uint8_t* next = nullptr;

                    std::vector<std::string> result = this->subject_->getSystemNames();
                    context.reset(result, hash_t::noval_t_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::getVisual :                                                        // Messages::Visual Server::getVisual(std::int64_t sys, int dimType, int id)
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.getVisual")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto dimType = xc::messages::message_reader<hash_t>::template get_arg<int, arg<2, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<int, arg<3, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    Messages::Visual result = this->subject_->getVisual(sys, dimType, id);
                    context.reset(result, hash_t::Visual);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::importFile :                                                       // bool Server::importFile(std::int64_t sys, std::string fname, bool makeChild = false, std::string usr = "");
                {
                    XCLOG("message_reader_server.importFile")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto fname = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    bool makeChild(false);
                    std::string usr;
                    bool result;
                    switch (fn_args_encoded(ptr))
                    {
                        case 3 :
                        {
                            makeChild = xc::messages::message_reader<hash_t>::template get_arg<bool, arg<3, Version>, Version>(ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                            result = this->subject_->importFile(sys, fname, makeChild);
                            return broadcast_outcome(funcname_hash, result);
                        }

                        case 4 :
                        {
                            makeChild = xc::messages::message_reader<hash_t>::template get_arg<bool, arg<3, Version>, Version>(ptr, size, next);
                            usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<4, Version>, Version>(ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                            result = this->subject_->importFile(sys, fname, makeChild, usr);
                            return broadcast_outcome(funcname_hash, result);
                        }
                    }
                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::isReady :                                                          // bool Server::isReady();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_server.isReady")

                    //std::uint8_t* next = nullptr;

                    bool result = this->subject_->isReady();
                    context.reset(result, hash_t::bool_);
                    ftor(funcname_hash, xc::messages::MsgType::Data_, result);

                    return true;
                }


                case xc::messages::FunctionName::load :                                                             // bool Server::load(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_server.load")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto path = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->load(sys, path);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::loadCLsource :                                                     // bool Server::loadCLsource(std::int64_t sys, std::string sourceCode);
                {
                    XCLOG("message_reader_server.loadSource")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto sourceCode = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->loadCLsource(sys, sourceCode);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::loadConfig :                                                       // bool Server::loadConfig(Messages::Variables vars);
                                                                                                                    // bool Client::loadServerConfig(Messages::Variables vars);
                {
                    XCLOG("message_reader_server.loadConfig")

                    std::uint8_t* next = nullptr;
                    auto vars = xc::messages::message_reader<hash_t>::template get_arg<Messages::Variables, arg<1, Version>, Version>(ptr, size, next);

                    bool result = this->subject_->loadConfig(vars);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::lockSystem :                                                       // bool Server::lockSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.lockSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->lockSystem(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::makeBackgroundWithSDF :                                            // std::shared_ptr<System> Server::makeBackgroundWithSDF(std::int64_t parent, std::set<int> surfaces, Messages::GridControl& gcontrol, std::string usr = "");
                {
                    XCLOG("message_reader_server.makeBackgroundWithSDF")

                    std::uint8_t* next = nullptr;
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::set<std::int32_t> surfaces;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        surfaces.emplace(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto gcontrol = xc::messages::message_reader<hash_t>::template get_arg<Messages::GridControl, Version>(arg<3, Version> + offset, ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 4)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<4, Version> + offset, ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->makeBackgroundWithSDF(parent, surfaces, gcontrol, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::makeGrid :                                                         // std::shared_ptr<System> Server::makeGrid(std::int64_t parent, Messages::GridControl& gcontrol, std::string usr = "");
                {
                    XCLOG("message_reader_server.makeGrid")

                    std::uint8_t* next = nullptr;
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto gcontrol = xc::messages::message_reader<hash_t>::template get_arg<Messages::GridControl, arg<2, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 3)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->makeGrid(parent, gcontrol, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::makeGroupFromSDF :                                                 // bool Server::makeGroupFromSDF(std::int64_t sys, double min, double max, int id);
                {
                    XCLOG("message_reader_server.makeGroupFromSDF")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<1, Version>, Version>(ptr, size, next);
                    auto min = xc::messages::message_reader<hash_t>::template get_arg<double, arg<2, Version>, Version>(ptr, size, next);
                    auto max = xc::messages::message_reader<hash_t>::template get_arg<double, arg<3, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<4, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->makeGroupFromSDF(sys, min, max, id);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::makeMesh :                                                         // std::shared_ptr<System> Server::makeMesh(std::int64_t parent, std::set<int> surfaces, Messages::MeshControl& mcontrol, std::string sysname = "", std::string usr = "");
                {
                    XCLOG("message_reader_server.makeMesh")

                    std::uint8_t* next = nullptr;
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::set<std::int32_t> surfaces;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        surfaces.emplace(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto mcontrol = xc::messages::message_reader<hash_t>::template get_arg<Messages::MeshControl, Version>(arg<3, Version> + offset, ptr, size, next);

                    std::string sysname;
                    std::string usr;
                    std::shared_ptr<System> result;
                    switch (fn_args_encoded(ptr))
                    {
                        case 4 :
                        {
                            sysname = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<4, Version> + offset, ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                            result = this->subject_->makeMesh(parent, surfaces, mcontrol, sysname);
                            return broadcast_outcome(funcname_hash, !!result);                                      // serialize shptr as bool
                        }

                        case 5 :
                        {
                            sysname = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<4, Version> + offset, ptr, size, next);
                            usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<5, Version> + offset, ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                            result = this->subject_->makeMesh(parent, surfaces, mcontrol, sysname, usr);
                            return broadcast_outcome(funcname_hash, !!result);                                      // serialize shptr as bool
                        }
                    }
                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::makeOctree :                                                       // std::shared_ptr<System> Server::makeOctree(std::int64_t parent, std::set<int> surfIds, Messages::TreeControl& ocontrol, std::string sysname = "", std::string usr = "");
                {
                    XCLOG("message_reader_server.makeOctree")

                    std::uint8_t* next = nullptr;
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::set<std::int32_t> surfIds;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        surfIds.emplace(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto ocontrol = xc::messages::message_reader<hash_t>::template get_arg<Messages::TreeControl, Version>(arg<3, Version> + offset, ptr, size, next);

                    std::string sysname;
                    std::string usr;
                    std::shared_ptr<System> result;
                    switch (fn_args_encoded(ptr))
                    {
                        case 4 :
                        {
                            sysname = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<4, Version> + offset, ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                            result = this->subject_->makeOctree(parent, surfIds, ocontrol, sysname);
                            return broadcast_outcome(funcname_hash, !!result);                                      // serialize shptr as bool
                        }

                        case 5 :
                        {
                            sysname = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<4, Version> + offset, ptr, size, next);
                            usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<5, Version> + offset, ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                            result = this->subject_->makeOctree(parent, surfIds, ocontrol, sysname, usr);
                            return broadcast_outcome(funcname_hash, !!result);                                      // serialize shptr as bool
                        }
                    }
                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::makeSurfaceFromFaces :                                             // bool Server::makeSurfaceFromFaces(std::int64_t sys, std::vector<Messages::Zone> faces, int regionId);
                {
                    XCLOG("message_reader_server.makeSurfacesFromFaces")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::vector<Messages::Zone> faces;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<Messages::Zone, Version>(arg<2, Version> + e, ptr, size, next);
                        faces.emplace_back(ee);
                    }
                    auto offset = len ? len - 1 : 0;

                    auto regionId = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<3, Version> + offset, ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->makeSurfaceFromFaces(sys, faces, regionId);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::makeSurfaceFromVolume :                                            // bool Server::makeSurfaceFromVolume(std::int64_t sys, int vol, int surfId);
                {
                    XCLOG("message_reader_server.makeSurfaceFromVolume")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto vol = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);
                    auto surfId = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<3, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->makeSurfaceFromVolume(sys, vol, surfId);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::makeVolumeFromSDF :                                                // bool Server::makeVolumeFromSDF(std::int64_t sys, double min, double max, int id);
                {
                    XCLOG("message_reader_server.makeVolumeFromSDF")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto min = xc::messages::message_reader<hash_t>::template get_arg<double, arg<2, Version>, Version>(ptr, size, next);
                    auto max = xc::messages::message_reader<hash_t>::template get_arg<double, arg<3, Version>, Version>(ptr, size, next);
                    auto id = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<4, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->makeVolumeFromSDF(sys, min, max, id);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::moveSystem :                                                       // std::shared_ptr<System> Server::moveSystem(std::int64_t sys, std::int64_t parent, std::string usr = "");
                {
                    XCLOG("message_reader_server.moveSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<2, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 3)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->moveSystem(sys, parent, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::newSystem :                                                        // std::shared_ptr<System> Server::newSystem(std::int64_t parent, std::string childName, std::string usr = "");
                {
                    XCLOG("message_reader_server.newSystem")

                    std::uint8_t* next = nullptr;
                    auto parent = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto childName = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 3)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(parent, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->newSystem(parent, childName, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::refreshSolver :                                                    // bool Server::refreshSolver(std::int64_t sys);
                {
                    XCLOG("message_reader_server.refreshSolver")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->refreshSolver(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::removeAlgorithm :                                                  // bool Server::removeAlgorithm(std::int64_t sys, std::string algorithm);
                {
                    XCLOG("message_reader_server.removeAlgorithm")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto algorithm = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->removeAlgorithm(sys, algorithm);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::removeCondition :                                                  // bool Server::removeCondition(std::int64_t sys, int i);
                {
                    XCLOG("message_reader_server.removeCondition")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto i = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->removeCondition(sys, i);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::removeGeometry :                                                   // bool Server::removeGeometry(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.removeGeometry")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->removeGeometry(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::removeJob :                                                        // bool Server::removeJob(std::int64_t sys);
                {
                    XCLOG("message_reader_server.removeJob")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->removeJob(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::removeModel :                                                      // bool Server::removeModel(std::int64_t sys, std::string m);
                {
                    XCLOG("message_reader_server.removeModel")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto m = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->removeModel(sys, m);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::renameSystem :                                                     // std::shared_ptr<System> Server::renameSystem(std::int64_t sys, std::string newname, std::string usr = "");
                {
                    XCLOG("message_reader_server.renameSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto newname = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 3)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    std::shared_ptr<System> result = this->subject_->renameSystem(sys, newname, usr);
                    return broadcast_outcome(funcname_hash, !!result);                                              // serialize shptr as bool
                }


                case xc::messages::FunctionName::reorderJobs :                                                      // bool Server::reorderJobs(std::list<std::int64_t> jobs);
                {
                    XCLOG("message_reader_server.reorderJobs")

                    std::uint8_t* next = nullptr;

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<1, Version> >(ptr, size);
                    std::list<std::int64_t> jobs;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, Version>(arg<1, Version> + e, ptr, size, next);
                        jobs.emplace_back(ee);
                    }

                    bool result = this->subject_->reorderJobs(jobs);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::resetJob :                                                         // bool Server::resetJob(std::int64_t sys);
                {
                    XCLOG("message_reader_server.resetJob")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->resetJob(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::resetTransform :                                                   // bool Server::resetTransform(std::int64_t sys, std::string usr = "");
                                                                                                                    // bool Client::resetSystemTransform(Metaobject* meta);
                {
                    XCLOG("message_reader_server.resetTransform")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->resetTransform(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::reverseFaceWinding :                                               // bool Server::reverseFaceWinding(std::int64_t sys, std::vector<Messages::Zone> faces);
                {
                    XCLOG("message_reader_server.reverseFaceWinding")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::vector<Messages::Zone> faces;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<Messages::Zone, Version>(arg<2, Version> + e, ptr, size, next);
                        faces.emplace_back(ee);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->reverseFaceWinding(sys, faces);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::rotateSystem :                                                     // bool Server::rotateSystem(std::int64_t sys, double angle, VEC3 axis, std::string usr = "");
                {
                    XCLOG("message_reader_server.rotateSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto angle = xc::messages::message_reader<hash_t>::template get_arg<double, arg<2, Version>, Version>(ptr, size, next);

                    auto e0 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<3, Version>, Version>(ptr, size, next);
                    auto e1 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<4, Version>, Version>(ptr, size, next);
                    auto e2 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<5, Version>, Version>(ptr, size, next);
                    VEC3 axis(e0, e1, e2);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 6)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<4, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->rotateSystem(sys, angle, axis, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::save :                                                             // bool Server::save(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_server.save")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto path = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->save(sys, path);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::saveAll :                                                          // bool Server::saveAll();
                {
                    XCLOG("message_reader_server.saveAll")

                    //std::uint8_t* next = nullptr;

                    bool result = this->subject_->saveAll();
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::saveCL :                                                           // bool Server::saveCL(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_server.saveCL")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto path = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->saveCL(sys, path);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::scaleSystem :                                                      // bool Server::scaleSystem(std::int64_t sys, VEC3 magnitude, std::string usr = "");
                {
                    XCLOG("message_reader_server.scaleSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto e0 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<2, Version>, Version>(ptr, size, next);
                    auto e1 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<3, Version>, Version>(ptr, size, next);
                    auto e2 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<4, Version>, Version>(ptr, size, next);
                    VEC3 magnitude(e0, e1, e2);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 5)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->scaleSystem(sys, magnitude, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::seekFirst :                                                        // bool Server::seekFirst(std::int64_t sys);
                {
                    XCLOG("message_reader_server.seekFirst")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->seekFirst(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::seekLast :                                                         // bool Server::seekLast(std::int64_t sys);
                {
                    XCLOG("message_reader_server.seekLast")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->seekLast(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::seekNext :                                                         // bool Server::seekNext(std::int64_t sys);
                {
                    XCLOG("message_reader_server.seekNext")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->seekNext(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::seekPrevious :                                                     // bool Server::seekPrevious(std::int64_t sys);
                {
                    XCLOG("message_reader_server.seekPrevious")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->seekPrevious(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setAlgorithmOrder :                                                // bool Server::setAlgorithmOrder(std::int64_t sys, std::string pk, const std::vector<std::string>& newOrder);
                {
                    XCLOG("message_reader_server.setAlgorithmOrder")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto pk = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<4, Version> >(ptr, size);
                    std::vector<std::string> newOrder;
                    newOrder.reserve(len);
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<3, Version> + e, ptr, size, next);
                        newOrder[e] = ee;
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setAlgorithmOrder(sys, pk, newOrder);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setConditionSequence :                                             // bool Server::setConditionSequence(std::int64_t sys, std::vector<int> order);
                {
                    XCLOG("message_reader_server.setConditionSequence")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::vector<std::int32_t> order;
                    order.reserve(len);
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        order[e] = ee;
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setConditionSequence(sys, order);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setData :                                                          // bool Server::setData(std::int64_t sys, std::string model, Record data);
                {
                    XCLOG("message_reader_server.setData")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto model = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<3, Version> >(ptr, size);
                    using value_type = typename Record::value_type;
                    Record data;
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<value_type, Version>(arg<3, Version> + e, ptr, size, next);
                        data.Add(std::move(ee));
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setData(sys, model, data);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setDevice :                                                        // bool Server::setDevice(std::int64_t uid);
                                                                                                                    // bool Server::setDevice(std::int64_t sys, std::int64_t uid);
                {
                    XCLOG("message_reader_server.setDevice")

                    std::uint8_t* next = nullptr;
                    switch (fn_args_encoded(ptr))
                    {
                        case 1 :
                        {
                            auto uid = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(uid, this->cxn_);

                            bool result = this->subject_->setDevice(uid);
                            return broadcast_outcome(funcname_hash, result);
                        }

                        case 2 :
                        {
                            auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                            auto uid = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<2, Version>, Version>(ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(uid, this->cxn_);

                            bool result = this->subject_->setDevice(sys, uid);
                            return broadcast_outcome(funcname_hash, result);
                        }
                    }
                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::setDOFs :                                                          // bool Server::setDOFs(std::int64_t sys, std::string model, std::vector<std::string> newDofs);
                {
                    XCLOG("message_reader_server.setDOFs")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto model = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<3, Version> >(ptr, size);
                    std::vector<std::string> newDofs;
                    newDofs.reserve(len);
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(arg<3, Version> + e, ptr, size, next);
                        newDofs[e] = ee;
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setDOFs(sys, model, newDofs);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setInstructionData :                                               // bool Server::setInstructionData(std::int64_t sys, int instruction, Messages::Vector64 value);
                {
                    XCLOG("message_reader_server.setInstructionData")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto instruction = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);
                    auto value = xc::messages::message_reader<hash_t>::template get_arg<Messages::Vector64, arg<3, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setInstructionData(sys, instruction, value);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setMeta :                                                          // bool Server::setMeta(Messages::Meta& media);
                {
                    XCLOG("message_reader_server.setMeta")

                    std::uint8_t* next = nullptr;
                    auto media = xc::messages::message_reader<hash_t>::template get_arg<Messages::Meta, arg<1, Version>, Version>(ptr, size, next);

                    (void)this->subject_->setMeta(media);

                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::setSolver :                                                        // bool Server::setSolver(std::int64_t sys, std::string solverType);
                {
                    XCLOG("message_reader_server.setSolver")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto solverType = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setSolver(sys, solverType);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setSolverSequence :                                                // bool Server::setSolverSequence(std::int64_t sys, std::vector<int> order, int seqNum = 0);
                {
                    XCLOG("message_reader_server.setSolverSequence")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto len = xc::messages::message_reader<HashType>::template ctrsize<arg<2, Version> >(ptr, size);
                    std::vector<int> order;
                    order.reserve(len);
                    for (std::size_t e = 0; e != len; ++e)
                    {
                        auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, Version>(arg<2, Version> + e, ptr, size, next);
                        order[e] = ee;
                    }

                    switch (fn_args_encoded(ptr))
                    {
                        case 2 :
                        {
                            this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                            bool result = this->subject_->setSolverSequence(sys, order);
                            return broadcast_outcome(funcname_hash, result);
                        }

                        case 3 :
                        {
                            int seqNum = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<3, Version>, Version>(ptr, size, next);

                            this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                            bool result = this->subject_->setSolverSequence(sys, order, seqNum);
                            return broadcast_outcome(funcname_hash, result);
                        }
                    }
                    return false;                                                                                   // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
                }


                case xc::messages::FunctionName::setSystemData :                                                    // bool Server::setSystemData(std::int64_t sys, Messages::Vector64 value);
                {
                    XCLOG("message_reader_server.setSystemData")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto value = xc::messages::message_reader<hash_t>::template get_arg<Messages::Vector64, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setSystemData(sys, value);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setVariables :                                                     // bool Server::setVariables(std::int64_t sys, Messages::Variables options);
                {
                    XCLOG("message_reader_server.setVariables")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto options = xc::messages::message_reader<hash_t>::template get_arg<Messages::Variables, arg<2, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setVariables(sys, options);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::setWriteInterval :                                                 // bool Server::setWriteInterval(std::int64_t sys, int i, std::string usr = "");
                {
                    XCLOG("message_reader_server.setWriteInterval")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);
                    auto i = xc::messages::message_reader<hash_t>::template get_arg<std::int32_t, arg<2, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 3)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<3, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->setWriteInterval(sys, i, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::solve :                                                            // bool Server::solve(std::int64_t sys);
                {
                    XCLOG("message_reader_server.solve")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->solve(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::stopJob :                                                          // bool Server::stopJob(std::int64_t sys);
                {
                    XCLOG("message_reader_server.stopJob")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->stopJob(sys);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::translateSystem :                                                  // bool Server::translateSystem(std::int64_t sys, VEC3 delta, std::string usr = "");
                {
                    XCLOG("message_reader_server.translateSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    auto e0 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<2, Version>, Version>(ptr, size, next);
                    auto e1 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<3, Version>, Version>(ptr, size, next);
                    auto e2 = xc::messages::message_reader<hash_t>::template get_arg<typename VEC3::value_type, arg<4, Version>, Version>(ptr, size, next);
                    VEC3 delta(e0, e1, e2);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 5)                                                                  // sys(1), delta(3), usr(1)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<5, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->translateSystem(sys, delta, usr);
                    return broadcast_outcome(funcname_hash, result);
                }


                case xc::messages::FunctionName::unlockSystem :                                                     // bool Server::unlockSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_server.unlockSystem")

                    std::uint8_t* next = nullptr;
                    auto sys = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, arg<1, Version>, Version>(ptr, size, next);

                    std::string usr;
                    if (fn_args_encoded(ptr) == 2)
                    {
                        usr = xc::messages::message_reader<hash_t>::template get_arg<std::string, arg<2, Version>, Version>(ptr, size, next);
                    }

                    this->subject_->cache_uid_cxn_pair(sys, this->cxn_);

                    bool result = this->subject_->unlockSystem(sys, usr);
                    return broadcast_outcome(funcname_hash, result);
                }



                default:
                {
                    std::cerr << std::endl << "Message Failure: unsupported Server function-call!" << std::endl;
                    return false;
                }
            }

            return false;                                                                                           // *false* : only return true with ftor-semantics, or via broadcast_outcome when BROADCAST_RESULT _is_ defined, false otherwise
        }
    };

} }  // namespace xc::messages
