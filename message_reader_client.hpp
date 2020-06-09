#pragma once

#include "CORE/Common/io/payload.hpp"
#include "CORE/Common/serialization/detail/packet_header.hpp"
#include "CORE/Common/utilities/severity.hpp"
#include "CORE/Messages/function_enumerants.hpp"
#include "CORE/Messages/message_reader_base.hpp"
#include "CORE/Messages/message_types.hpp"
#include "CORE/Messages/meta.pb.h"
#include "CORE/Messages/spatial.pb.h"
#include "CORE/Messages/system.pb.h"
#include "CORE/Messages/vector.pb.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>



namespace xc {


    /* Forward Declarations */

    class Client;


namespace messages {

    template <typename HashType>
    class message_reader_t<xc::Client, HashType> : public xc::messages::message_reader_base_t<xc::Client, HashType, message_reader_t>
    {
    public:

        using hash_t = HashType;
        using subject_t = xc::Client;
        using base_t = xc::messages::message_reader_base_t<subject_t, HashType, message_reader_t>;


        message_reader_t(subject_t* s)
        : base_t(s)
        { }


        template <std::uint16_t Version, typename F>
        bool
        process_packet(std::uint8_t* ptr,
                       std::size_t size,
                       typename xc::io::payload_t<hash_t>::result_type& context,
                       F&& ftor)
        {
            // **************************************************************************************************************************************************************
            // -- CALLS ON CLIENT FROM SERVER                               server::foo(..) { client->bar(..); }
            // -- RETURN VALUES FROM SERVER TO FORWARD TO CLIENT            RT client::spod(..) { return server->foo(..); }
            // **************************************************************************************************************************************************************


            // Protocol v1.0:  Pkt { hash(fn-name), Callable_|Data_|Log_, arg1, .., argN }
            // Are potential calls Server makes on the Client:  server::bar(..) { client->func(..); }


            //auto fn_args_encoded = [](const std::uint8_t* ptr) -> std::uint16_t
            //{
            //    // return count of function-arguments encoded in the Pkt
            //
            //    const auto pkthdr    = *reinterpret_cast<const xc::serialization::detail::packet_header_t *> (ptr);
            //    std::uint16_t msgcnt = pkthdr.message_count();
            //
            //    return msgcnt - xc::messages::zero_offset<Version>;                                                 // version 1.0 protocol:  Pkt { func-name, Callable_|Data_|Log_, param1, .., paramN };  subtract two to account for `func-name' && 'Callable_'|'Data_|Log_'
            //};


            auto broadcast_outcome = [ptr, size](xc::messages::MsgType e) -> bool                                   // xc::messages::Log Server::broadcast(const xc::messages::Log& message);
            {
                if (e == xc::messages::MsgType::Log_)
                {
                    auto result = xc::messages::message_reader<hash_t>::template get_arg<xc::messages::Log, xc::messages::return_slot<Version>, Version>(ptr, size);
                    std::string severity(xc::utilities::severity_to_string(result.severity));
                    std::transform(severity.begin(), severity.end(), severity.begin(), [](std::uint8_t c){ return std::toupper(c); });

                    std::ostringstream oss;
                    oss << "[" << severity << "]: " << result.text;
                    if (result.severity <= xc::utilities::severity::error)
                    {
                        // stderr:  at or above the error-threshold -  { emergency, alert, critical, error }
                        std::cerr << std::endl << oss.str() << std::endl;
                    }
                    else
                    {
                        // stdout:  anything below the error-threshold -  { warning, notice, info, debug }
                        std::cout << std::endl << oss.str() << std::endl;
                    }
                }
                return false;                                                                                       // *false* : because we do not use ftor semantics
            };



            // NOTE: unfortunately, the compile-time equivalent has too heavy a CT burden!

            // all messages from server to client may be either "client-callable", or "client-data" packets!
            xc::messages::FunctionName funcname_hash = xc::messages::message_reader<hash_t>::template get_arg<xc::messages::FunctionName, xc::messages::funcname<Version>, Version>(ptr, size);
            xc::messages::MsgType mt = xc::messages::message_reader<hash_t>::template get_arg<xc::messages::MsgType, xc::messages::msgtype<Version>, Version>(ptr, size);
            switch (funcname_hash)
            {
                // FIXME:  flesh out all function names..
                // FIXME:  case-statements can be auto-generated!


                // *************************************
                // **** CALLS ON CLIENT FROM SERVER ****
                // *************************************

                case xc::messages::FunctionName::setMeta :                                                          // bool Client::setMeta(Messages::Meta msg);
                {
                    XCLOG("message_reader_client.setMeta")

                    if (mt == xc::messages::MsgType::Callable_)
                    {
                        /*
                        // Elide call to Client::setMeta - causes lock-contention w/ UI-thread
                        //
                        //std::uint8_t* next = nullptr;
                        //auto msg = xc::messages::message_reader<hash_t>::template get_arg<Messages::Meta, arg<1, Version>, Version>(ptr, size, next);
                        //
                        //bool result = this->subject_->setMeta(msg);
                        //context.reset(result, hash_t::bool_);
                        //ftor(funcname_hash, context);
                        */

                        // ----

                        // Try an interface update instead..

                        std::uint8_t* next = nullptr;
                        auto msg = xc::messages::message_reader<hash_t>::template get_arg<Messages::Meta, arg<1, Version>, Version>(ptr, size, next);

                        // get or create a corresponding metaobject
                        auto meta = this->subject_->interface->scene->getMetaobject(msg.uid());                     // get existing or create! guaranteed metaobject return

                        // copy to the metaobject buffer
                        meta->buffer = msg;


                        // By-pass the refresh..
                        // refresh the metaobject (which updates internal vectors if required)
                        //return meta->refresh();                                                                     // checks revision numbers for extent required (usually only updates meta data including model matrix M)

                        // Try the interface update instead of the meta->refresh() ..
                        this->subject_->interface->update();

                        return true;
                    }
                    return false;
                }


                // ************************************************************
                // **** RETURN VALUES FROM SERVER TO BE CONSUMED BY CLIENT ****
                // ************************************************************


                // FIXME:  case-statements can be auto-generated!


                case xc::messages::FunctionName::addAlgorithm :                                                     // bool Server::addAlgorithm(std::int64_t sys, std::string algorithm);
                {
                    XCLOG("message_reader_client.addAlgorithm")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::addCondition :                                                     // Messages::Command Server::addCondition(std::int64_t sys, std::string algorithm);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.addCondition")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Command, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Command);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::addModel :                                                         // bool Server::addModel(std::int64_t sys, std::string model);
                {
                    XCLOG("message_reader_client.addModel")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::addRegionToInstruction :                                           // bool Server::addRegionToInstruction(std::int64_t sys, int instr, int id, int dim, bool overwrite);
                {
                    XCLOG("message_reader_client.addRegionToInstruction")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::addSlice :                                                         // bool Server::addSlice(std::int64_t sys, Messages::SliceControl control);
                {
                    XCLOG("message_reader_client.addSlice")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::assignSymbol :                                                     // bool Server::assignSymbol(std::int64_t sys, std::string filepath);
                {
                    XCLOG("message_reader_client.assignSymbol")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::broadcast :                                                        // xc::messages::Log broadcast(const xc::messages::Log& msg, xc::messages::FunctionName hash = xc::messages::FunctionName::broadcast);
                {
                    //XCLOG("message_reader_client.broadcast")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::canSolve :                                                         // bool Server::canSolve(std::int64_t sys);
                {
                    XCLOG("message_reader_client.canSolve")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::clearSubsystems :                                                  // bool Server::clearSubsystems(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.clearSubsystems")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::convertToMesh :                                                    // bool Server::convertToMesh(std::int64_t sys);
                {
                    XCLOG("message_reader_client.convertToMesh")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::deleteAllIterations :                                              // bool Server::deleteAllIterations(std::int64_t sys);
                {
                    XCLOG("message_reader_client.deleteAllIterations")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::deleteGroup :                                                      // bool Server::deleteGroup(std::int64_t sys, int id);
                {
                    XCLOG("message_reader_client.deleteGroup")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::deleteIteration :                                                  // bool Server::deleteIteration(std::int64_t sys);
                {
                    XCLOG("message_reader_client.deleteIteration")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::deleteSystem :                                                     // std::shared_ptr<System> Server::deleteSystem(std::int64_t sys, bool recursive = false, std::string usr = "");
                {
                    XCLOG("message_reader_client.deleteSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::deleteVolume :                                                     // bool Server::deleteVolume(std::int64_t sys, int id);
                {
                    XCLOG("message_reader_client.deleteVolume")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::disableSystem :                                                    // bool Server::disableSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.disableSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::disableWrite :                                                     // bool Server::disableWrite(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.disableWrite")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::enableSystem :                                                     // bool Server::enableSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.enableSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::enableWrite :                                                      // bool Server::enableWrite(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.enableWrite")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::enqueueJob :                                                       // bool Server::enqueueJob(std::int64_t sys);
                {
                    XCLOG("message_reader_client.enqueueJob")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::eraseJob :                                                         // bool Server::eraseJob(std::int64_t sys);
                {
                    XCLOG("message_reader_client.eraseJob")

                    return broadcast_outcome(mt);
                }



                case xc::messages::FunctionName::eraseRegionFromInstruction :                                       // bool Server::eraseRegionFromInstruction(std::int64_t sys, int instr, int id, int dim);
                {
                    XCLOG("message_reader_client.eraseRegionFromInstruction")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::estimateMeshTime :                                                 // std::time_t Server::getMeshTimeEstimate(std::int64_t sys, std::set<int>& surfs, Messages::MeshControl& mcontrol);
                {
                    XCLOG("message_reader_client.estimateMeshTime")

                    // ** client-side blocks for result

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        // std::time_t ~ std::int64_t
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::time_t, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::int64_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::exportFile :                                                       // bool Server::exportFile(std::int64_t sys, std::string fname);
                {
                    XCLOG("message_reader_client.exportFile")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::getAlgorithmNames :                                                // std::vector<std::string> Server::getAlgorithmNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getAlgoirthmNames")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::fuseFaces :                                                        // bool Server::fuseFaces(std::int64_t sys);
                {
                    XCLOG("message_reader_client.fuseFaces")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::getAlgorithmInfo :                                                 // Messages::Function Server::getAlgorithmInfo(std::string algorithmType);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getAlgorithmInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Function, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Function);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getAttribute :                                                     // Messages::Vector32 getAttribute(std::int64_t sys, std::string pk);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getAttribute")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Vector32, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Vector32);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getAvailableModels :                                               // std::set<std::string> Server::getAvailableModels(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getAvailableModels")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::set<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getAverageResidual :                                               // double Server::getAverageResidual(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getAverageResidual")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<double, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::double_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getCLcode :                                                        // std::string Server::getCLcode(std::int64_t instructionID) const;
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getCLcode")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::string, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::string_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getConditionsInfo :                                                // Commands Server::getConditionsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getConditionsInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        using value_type = typename Commands::value_type;

                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        Commands result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<value_type, Version>(return_slot<Version> + e, ptr, size, next);
                            result.Add(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getDefaultDevice :                                                 // std::string Server::getDefaultDevice();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getDefaultDevice")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::string, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::string_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getDeviceNames :                                                   // std::vector<std::string> Server::getDeviceNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getDeviceNames")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getFacesByAngle :                                                  // std::vector<Messages::Zone> Server::getFacesByAngle(std::int64_t sys, std::vector<Messages::Zone> faceIndices, double angle, bool absoluteAngle);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getFacesByAngle")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<Messages::Zone> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<Messages::Zone, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getGeometryNames :                                                 // std::vector<std::string> Server::getGeometryNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getGeometryNames")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getInstructionData :                                               // Record Server::getInstructionData(std::int64_t i);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getInstructionData")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        using value_type = typename Record::value_type;

                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        Record result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<value_type, Version>(return_slot<Version> + e, ptr, size, next);
                            result.Add(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getInstructionRegion :                                             // Messages::Zone Server::getInstructionRegion(std::int64_t i, std::int32_t d);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getInstructionRegion")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Zone, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::string_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getJobsList :                                                      // std::list<Messages::Script> Server::getJobsList();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getJobsList")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::list<Messages::Script> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<Messages::Script, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getModelInfo :                                                     // Messages::Model Server::getModelInfo(std::int64_t sys, std::string modelType);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getModelInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Model, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Model);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getModelNames :                                                    // std::vector<std::string> Server::getModelNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getModelNames")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getModelsInfo :                                                    // Models Server::getModelsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getModelsInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        using value_type = typename Models::value_type;

                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        Models result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<value_type, Version>(return_slot<Version> + e, ptr, size, next);
                            result.Add(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getParent :                                                        // std::int64_t Server::getParent(std::int64_t uid) const;
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getParent")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::int64_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getPhysicsInfo :                                                   // Messages::Model Server::getPhysicsInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getPhysicsInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Model, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Model);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getPhysicsProperties :                                             // std::set<std::string> Server::getPhysicsProperties(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getPhysicsProperties")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::set<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getRootUID :                                                       // std::int64_t Server::getRootUID();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getRootUID")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::int64_t, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::int64_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getServerConfig :                                                  // Messages::Variables Server::getServerConfig();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getServerConfig")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Variables, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Variables);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getServerInfo :                                                    // std::string Server::getServerInfo();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getServerInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::string, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::string_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getServerName :                                                    // std::string Server::getServerName() const;
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getServerName")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<std::string, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::string_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getSolverInfo :                                                    // Messages::Script Server::getSolverInfo(std::int64_t sys);
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getSolverInfo")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Script, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Script);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getSystemNames :                                                   // std::vector<std::string> Server::getSystemNames();
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getSystemNames")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        std::uint8_t* next = nullptr;
                        auto len = xc::messages::message_reader<HashType>::template ctrsize<return_slot<Version> >(ptr, size);
                        std::vector<std::string> result;
                        for (std::size_t e = 0; e != len; ++e)
                        {
                            auto ee = xc::messages::message_reader<hash_t>::template get_arg<std::string, Version>(return_slot<Version> + e, ptr, size, next);
                            result.emplace_back(std::move(ee));
                        }

                        context.reset(result, hash_t::noval_t_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::getVisual :                                                        // Messages::Visual Server::getVisual(std::int64_t sys, int dimType, int id)
                {
                    // ** client-side blocks for result

                    XCLOG("message_reader_client.getVisual")

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<Messages::Visual, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::Visual);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::importFile :                                                       // bool Server::importFile(std::int64_t sys, std::string fname, bool makeChild = false, std::string usr = "");
                {
                    XCLOG("message_reader_client.importFile")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::isReady :                                                          // bool Server::isReady();
                {
                    XCLOG("message_reader_client.isReady")

                    // ** client-side blocks for result

                    if (mt == xc::messages::MsgType::Data_)
                    {
                        auto result = xc::messages::message_reader<hash_t>::template get_arg<bool, return_slot<Version>, Version>(ptr, size);
                        context.reset(result, hash_t::bool_);
                        ftor(funcname_hash, context);
                        return true;
                    }
                    return false;
                }


                case xc::messages::FunctionName::load :                                                             // bool Server::load(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_client.load")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::loadCLsource :                                                     // bool Server::loadCLsource(std::int64_t sys, std::string sourceCode);
                {
                    XCLOG("message_reader_client.loadCLsource")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::loadConfig :                                                       // bool Server::loadConfig(Messages::Variables vars);
                                                                                                                    // bool Client::loadServerConfig(Messages::Variables vars);
                {
                    XCLOG("message_reader_client.loadConfig")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::lockSystem :                                                       // bool Server::lockSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.lockSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeBackgroundWithSDF :                                            // std::shared_ptr<System> Server::makeBackgroundWithSDF(std::int64_t parent, std::set<int> surfaces, Messages::GridControl& gcontrol, std::string usr = "");
                {
                    XCLOG("message_reader_client.makeBackgroundWithSDF")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeGrid :                                                         // std::shared_ptr<System> Server::makeGrid(std::int64_t parent, Messages::GridControl& gcontrol, std::string usr = "");
                {
                    XCLOG("message_reader_client.makeGrid")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeGroupFromSDF :                                                 // bool Server::makeGroupFromSDF(std::int64_t sys, double min, double max, int id);
                {
                    XCLOG("message_reader_client.makeGroupFromSDF")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeMesh :                                                         // std::shared_ptr<System> Server::makeMesh(std::int64_t parent, std::set<int> surfaces, Messages::MeshControl& mcontrol, std::string sysname = "", std::string usr = "");
                {
                    XCLOG("message_reader_client.makeMesh")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeOctree :                                                       // std::shared_ptr<System> Server::makeOctree(std::int64_t parent, std::set<int> surfIds, Messages::TreeControl& ocontrol, std::string sysname = "", std::string usr = "");
                {
                    XCLOG("message_reader_client.makeOctree")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeSurfaceFromFaces :                                             // bool Server::makeSurfaceFromFaces(std::int64_t sys, std::vector<Messages::Zone> faces, int regionId);
                {
                    XCLOG("message_reader_client.makeSurfaceFromFaces")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeSurfaceFromVolume :                                            // bool Server::makeSurfaceFromVolume(std::int64_t sys, int vol, int surfId);
                {
                    XCLOG("message_reader_client.makeSurfaceFromVolume")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::makeVolumeFromSDF :                                                // bool Server::makeVolumeFromSDF(std::int64_t sys, double min, double max, int id);
                {
                    XCLOG("message_reader_client.makeVolumeFromSDF")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::moveSystem :                                                       // std::shared_ptr<System> Server::moveSystem(std::int64_t sys, std::int64_t parent, std::string usr = "");
                {
                    XCLOG("message_reader_client.moveSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::newSystem :                                                        // std::shared_ptr<System> Server::newSystem(std::int64_t parent, std::string childName, std::string usr = "");
                {
                    XCLOG("message_reader_client.newSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::refreshSolver :                                                    // bool Server::refreshSolver(std::int64_t sys);
                {
                    XCLOG("message_reader_client.refreshSolver")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::removeAlgorithm :                                                  // bool Server::removeAlgorithm(std::int64_t sys, std::string algorithm);
                {
                    XCLOG("message_reader_client.removeAlgorithm")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::removeCondition :                                                  // bool Server::removeCondition(std::int64_t sys, int i);
                {
                    XCLOG("message_reader_client.removeCondition")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::removeGeometry :                                                   // bool Server::removeGeometry(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.removeGeometry")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::removeJob :                                                        // bool Server::removeJob(std::int64_t sys);
                {
                    XCLOG("message_reader_client.removeJob")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::removeModel :                                                      // bool Server::removeModel(std::int64_t sys, std::string m);
                {
                    XCLOG("message_reader_client.removeModel")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::renameSystem :                                                     // std::shared_ptr<System> Server::renameSystem(std::int64_t sys, std::string newname, std::string usr = "");
                {
                    XCLOG("message_reader_client.renameSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::reorderJobs :                                                      // bool Server::reorderJobs(std::list<std::int64_t> jobs);
                {
                    XCLOG("message_reader_client.reorderJobs")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::resetJob :                                                         // bool Server::resetJob(std::int64_t sys);
                {
                    XCLOG("message_reader_client.resetJob")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::resetTransform :                                                   // bool Server::resetTransform(std::int64_t sys, std::string usr = "");
                                                                                                                    // bool Client::resetSystemTransform(Metaobject* meta);
                {
                    XCLOG("message_reader_client.resetTransform")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::reverseFaceWinding :                                               // bool Server::reverseFaceWinding(std::int64_t sys, std::vector<Messages::Zone> faces);
                {
                    XCLOG("message_reader_client.reverseFaceWinding")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::rotateSystem :                                                     // bool Server::rotateSystem(std::int64_t sys, double angle, VEC3 axis, std::string usr = "");
                {
                    XCLOG("message_reader_client.rotateSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::save :                                                             // bool Server::save(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_client.save")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::saveAll :                                                          // bool Server::saveAll();
                {
                    XCLOG("message_reader_client.saveAll")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::saveCL :                                                           // bool Server::saveCL(std::int64_t sys, std::string path);
                {
                    XCLOG("message_reader_client.saveCL")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::scaleSystem :                                                      // bool Server::scaleSystem(std::int64_t sys, VEC3 magnitude, std::string usr = "");
                {
                    XCLOG("message_reader_client.scaleSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::seekFirst :                                                        // bool Server::seekFirst(std::int64_t sys);
                {
                    XCLOG("message_reader_client.seekFirst")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::seekLast :                                                         // bool Server::seekLast(std::int64_t sys);
                {
                    XCLOG("message_reader_client.seekLast")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::seekNext :                                                         // bool Server::seekNext(std::int64_t sys);
                {
                    XCLOG("message_reader_client.seekNext")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::seekPrevious :                                                     // bool Server::seekPrevious(std::int64_t sys);
                {
                    XCLOG("message_reader_client.seekPrevious")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setAlgorithmOrder :                                                // bool Server::setAlgorithmOrder(std::int64_t sys, std::string pk, const std::vector<std::string>& newOrder);
                {
                    XCLOG("message_reader_client.setAlgorithmOrder")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setConditionSequence :                                             // bool Server::setConditionSequence(std::int64_t sys, std::vector<int> order);
                {
                    XCLOG("message_reader_client.setConditionSequence")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setData :                                                          // bool Server::setData(std::int64_t sys, std::string model, Record data);
                {
                    XCLOG("message_reader_client.setData")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setDevice :                                                        // bool Server::setDevice(std::int64_t uid);
                                                                                                                    // bool Server::setDevice(std::int64_t sys, std::int64_t uid);
                {
                    XCLOG("message_reader_client.setDevice")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setDOFs :                                                          // bool Server::setDOFs(std::int64_t sys, std::string model, std::vector<std::string> newDofs);
                {
                    XCLOG("message_reader_client.setDOFs")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setInstructionData :                                               // bool Server::setInstructionData(std::int64_t sys, int instruction, Messages::Vector64 value);
                {
                    XCLOG("message_reader_client.setInstructionData")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setSolver :                                                        // bool Server::setSolver(std::int64_t sys, std::string solverType);
                {
                    XCLOG("message_reader_client.setSolver")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setSolverSequence :                                                // bool Server::setSolverSequence(std::int64_t sys, std::vector<int> order, int seqNum = 0);
                {
                    XCLOG("message_reader_client.setSolverSequence")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setSystemData :                                                    // bool Server::setSystemData(std::int64_t sys, Messages::Vector64 value);
                {
                    XCLOG("message_reader_client.setSystemData")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setVariables :                                                     // bool Server::setVariables(std::int64_t sys, Messages::Variables options);
                {
                    XCLOG("message_reader_client.setVariables")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::setWriteInterval :                                                 // bool Server::setWriteInterval(std::int64_t sys, int i, std::string usr = "");
                {
                    XCLOG("message_reader_client.setWriteInterval")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::solve :                                                            // bool Server::solve(std::int64_t sys);
                {
                    XCLOG("message_reader_client.solve")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::stopJob :                                                          // bool Server::stopJob(std::int64_t sys);
                {
                    XCLOG("message_reader_client.stopJob")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::translateSystem :                                                  // bool Server::translateSystem(std::int64_t sys, VEC3 delta, std::string usr = "");
                {
                    XCLOG("message_reader_client.translateSystem")

                    return broadcast_outcome(mt);
                }


                case xc::messages::FunctionName::unlockSystem :                                                     // bool Server::unlockSystem(std::int64_t sys, std::string usr = "");
                {
                    XCLOG("message_reader_client.unlockSystem")

                    return broadcast_outcome(mt);
                }



                default:
                {
                    std::cerr << std::endl << "Message Failure: unsupported Client function-call, or Server function-return!" << std::endl;
                    return false;
                }
            }

            return false;                                                                                           // *false* :  only return true if we use ftor-semantics
        }
    };

} }  // namespace xc::messages
