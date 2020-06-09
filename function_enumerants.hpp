/* This is a generated file.  DO NOT EDIT! */


#pragma once

#include <CORE/Common/mpl/hash.hpp>


#include <cstdint>
#include <string>


namespace xc {
namespace messages {

    enum struct FunctionName : std::uint32_t
    {
        noval_t_ = xc::mpl::hash("noval_t") , 

        addAlgorithm = xc::mpl::hash("addAlgorithm") ,
        addCondition = xc::mpl::hash("addCondition") ,
        addModel = xc::mpl::hash("addModel") ,
        addRegionToInstruction = xc::mpl::hash("addRegionToInstruction") ,
        addSlice = xc::mpl::hash("addSlice") ,
        assignSymbol = xc::mpl::hash("assignSymbol") ,
        broadcast = xc::mpl::hash("broadcast") ,
        canSolve = xc::mpl::hash("canSolve") ,
        clearSubsystems = xc::mpl::hash("clearSubsystems") ,
        convertToMesh = xc::mpl::hash("convertToMesh") ,
        deleteAllIterations = xc::mpl::hash("deleteAllIterations") ,
        deleteGroup = xc::mpl::hash("deleteGroup") ,
        deleteIteration = xc::mpl::hash("deleteIteration") ,
        deleteSystem = xc::mpl::hash("deleteSystem") ,
        deleteVolume = xc::mpl::hash("deleteVolume") ,
        disableSystem = xc::mpl::hash("disableSystem") ,
        disableWrite = xc::mpl::hash("disableWrite") ,
        enableSystem = xc::mpl::hash("enableSystem") ,
        enableWrite = xc::mpl::hash("enableWrite") ,
        enqueueJob = xc::mpl::hash("enqueueJob") ,
        eraseJob = xc::mpl::hash("eraseJob") ,
        eraseRegionFromInstruction = xc::mpl::hash("eraseRegionFromInstruction") ,
        estimateMeshTime = xc::mpl::hash("estimateMeshTime") ,
        exportFile = xc::mpl::hash("exportFile") ,
        fuseFaces = xc::mpl::hash("fuseFaces") ,
        getAlgorithmInfo = xc::mpl::hash("getAlgorithmInfo") ,
        getAlgorithmNames = xc::mpl::hash("getAlgorithmNames") ,
        getAttribute = xc::mpl::hash("getAttribute") ,
        getAvailableModels = xc::mpl::hash("getAvailableModels") ,
        getAverageResidual = xc::mpl::hash("getAverageResidual") ,
        getChildNames = xc::mpl::hash("getChildNames") ,
        getCLcode = xc::mpl::hash("getCLcode") ,
        getConditionsInfo = xc::mpl::hash("getConditionsInfo") ,
        getDefaultDevice = xc::mpl::hash("getDefaultDevice") ,
        getDeviceNames = xc::mpl::hash("getDeviceNames") ,
        getFacesByAngle = xc::mpl::hash("getFacesByAngle") ,
        getGeometryNames = xc::mpl::hash("getGeometryNames") ,
        getInstructionData = xc::mpl::hash("getInstructionData") ,
        getInstructionRegion = xc::mpl::hash("getInstructionRegion") ,
        getJobsList = xc::mpl::hash("getJobsList") ,
        getModelInfo = xc::mpl::hash("getModelInfo") ,
        getModelNames = xc::mpl::hash("getModelNames") ,
        getModelsInfo = xc::mpl::hash("getModelsInfo") ,
        getParent = xc::mpl::hash("getParent") ,
        getPhysicsInfo = xc::mpl::hash("getPhysicsInfo") ,
        getPhysicsProperties = xc::mpl::hash("getPhysicsProperties") ,
        getRootUID = xc::mpl::hash("getRootUID") ,
        getServerConfig = xc::mpl::hash("getServerConfig") ,
        getServerInfo = xc::mpl::hash("getServerInfo") ,
        getServerName = xc::mpl::hash("getServerName") ,
        getSolverInfo = xc::mpl::hash("getSolverInfo") ,
        getSystemNames = xc::mpl::hash("getSystemNames") ,
        getVisual = xc::mpl::hash("getVisual") ,
        importFile = xc::mpl::hash("importFile") ,
        isReady = xc::mpl::hash("isReady") ,
        load = xc::mpl::hash("load") ,
        loadCLsource = xc::mpl::hash("loadCLsource") ,
        loadConfig = xc::mpl::hash("loadConfig") ,
        loadServerConfig = xc::mpl::hash("loadServerConfig") ,
        lockSystem = xc::mpl::hash("lockSystem") ,
        makeBackgroundWithSDF = xc::mpl::hash("makeBackgroundWithSDF") ,
        makeGrid = xc::mpl::hash("makeGrid") ,
        makeGroupFromSDF = xc::mpl::hash("makeGroupFromSDF") ,
        makeMesh = xc::mpl::hash("makeMesh") ,
        makeOctree = xc::mpl::hash("makeOctree") ,
        makeSurfaceFromFaces = xc::mpl::hash("makeSurfaceFromFaces") ,
        makeSurfaceFromVolume = xc::mpl::hash("makeSurfaceFromVolume") ,
        makeVolumeFromSDF = xc::mpl::hash("makeVolumeFromSDF") ,
        moveSystem = xc::mpl::hash("moveSystem") ,
        newSystem = xc::mpl::hash("newSystem") ,
        refreshSolver = xc::mpl::hash("refreshSolver") ,
        removeAlgorithm = xc::mpl::hash("removeAlgorithm") ,
        removeCondition = xc::mpl::hash("removeCondition") ,
        removeGeometry = xc::mpl::hash("removeGeometry") ,
        removeJob = xc::mpl::hash("removeJob") ,
        removeModel = xc::mpl::hash("removeModel") ,
        renameSystem = xc::mpl::hash("renameSystem") ,
        reorderJobs = xc::mpl::hash("reorderJobs") ,
        resetJob = xc::mpl::hash("resetJob") ,
        resetTransform = xc::mpl::hash("resetTransform") ,
        resetSystemTransform = xc::mpl::hash("resetSystemTransform") ,
        reverseFaceWinding = xc::mpl::hash("reverseFaceWinding") ,
        rotateSystem = xc::mpl::hash("rotateSystem") ,
        save = xc::mpl::hash("save") ,
        saveAll = xc::mpl::hash("saveAll") ,
        saveCL = xc::mpl::hash("saveCL") ,
        scaleSystem = xc::mpl::hash("scaleSystem") ,
        seekFirst = xc::mpl::hash("seekFirst") ,
        seekLast = xc::mpl::hash("seekLast") ,
        seekNext = xc::mpl::hash("seekNext") ,
        seekPrevious = xc::mpl::hash("seekPrevious") ,
        setAlgorithmOrder = xc::mpl::hash("setAlgorithmOrder") ,
        setConditionSequence = xc::mpl::hash("setConditionSequence") ,
        setData = xc::mpl::hash("setData") ,
        setDevice = xc::mpl::hash("setDevice") ,
        setDOFs = xc::mpl::hash("setDOFs") ,
        setInstructionData = xc::mpl::hash("setInstructionData") ,
        setMeta = xc::mpl::hash("setMeta") ,
        setSolver = xc::mpl::hash("setSolver") ,
        setSolverSequence = xc::mpl::hash("setSolverSequence") ,
        setSystemData = xc::mpl::hash("setSystemData") ,
        setVariables = xc::mpl::hash("setVariables") ,
        setWriteInterval = xc::mpl::hash("setWriteInterval") ,
        solve = xc::mpl::hash("solve") ,
        stopJob = xc::mpl::hash("stopJob") ,
        translateSystem = xc::mpl::hash("translateSystem") ,
        unlockSystem = xc::mpl::hash("unlockSystem") ,
    };



    inline
    std::string
    funcname_hash_to_string(xc::messages::FunctionName hash)
    {
        switch (hash)
        {
            case xc::messages::FunctionName::addAlgorithm :
            {
                return "addAlgorithm";
            }

            case xc::messages::FunctionName::addCondition :
            {
                return "addCondition";
            }

            case xc::messages::FunctionName::addModel :
            {
                return "addModel";
            }

            case xc::messages::FunctionName::addRegionToInstruction :
            {
                return "addRegionToInstruction";
            }

            case xc::messages::FunctionName::addSlice :
            {
                return "addSlice";
            }

            case xc::messages::FunctionName::assignSymbol :
            {
                return "assignSymbol";
            }

            case xc::messages::FunctionName::broadcast :
            {
                return "broadcast";
            }

            case xc::messages::FunctionName::canSolve :
            {
                return "canSolve";
            }

            case xc::messages::FunctionName::clearSubsystems :
            {
                return "clearSubsystems";
            }

            case xc::messages::FunctionName::convertToMesh :
            {
                return "convertToMesh";
            }

            case xc::messages::FunctionName::deleteAllIterations :
            {
                return "deleteAllIterations";
            }

            case xc::messages::FunctionName::deleteGroup :
            {
                return "deleteGroup";
            }

            case xc::messages::FunctionName::deleteIteration :
            {
                return "deleteIteration";
            }

            case xc::messages::FunctionName::deleteSystem :
            {
                return "deleteSystem";
            }

            case xc::messages::FunctionName::deleteVolume :
            {
                return "deleteVolume";
            }

            case xc::messages::FunctionName::disableSystem :
            {
                return "disableSystem";
            }

            case xc::messages::FunctionName::disableWrite :
            {
                return "disableWrite";
            }

            case xc::messages::FunctionName::enableSystem :
            {
                return "enableSystem";
            }

            case xc::messages::FunctionName::enableWrite :
            {
                return "enableWrite";
            }

            case xc::messages::FunctionName::enqueueJob :
            {
                return "enqueueJob";
            }

            case xc::messages::FunctionName::eraseJob :
            {
                return "eraseJob";
            }

            case xc::messages::FunctionName::eraseRegionFromInstruction :
            {
                return "eraseRegionFromInstruction";
            }

            case xc::messages::FunctionName::estimateMeshTime :
            {
                return "estimateMeshTime";
            }

            case xc::messages::FunctionName::exportFile :
            {
                return "exportFile";
            }

            case xc::messages::FunctionName::fuseFaces :
            {
                return "fuseFaces";
            }

            case xc::messages::FunctionName::getAlgorithmInfo :
            {
                return "getAlgorithmInfo";
            }

            case xc::messages::FunctionName::getAlgorithmNames :
            {
                return "getAlgorithmNames";
            }

            case xc::messages::FunctionName::getAttribute :
            {
                return "getAttribute";
            }

            case xc::messages::FunctionName::getAvailableModels :
            {
                return "getAvailableModels";
            }

            case xc::messages::FunctionName::getAverageResidual :
            {
                return "getAverageResidual";
            }

            case xc::messages::FunctionName::getChildNames :
            {
                return "getChildNames";
            }

            case xc::messages::FunctionName::getCLcode :
            {
                return "getCLcode";
            }

            case xc::messages::FunctionName::getConditionsInfo :
            {
                return "getConditionsInfo";
            }

            case xc::messages::FunctionName::getDefaultDevice :
            {
                return "getDefaultDevice";
            }

            case xc::messages::FunctionName::getDeviceNames :
            {
                return "getDeviceNames";
            }

            case xc::messages::FunctionName::getFacesByAngle :
            {
                return "getFacesByAngle";
            }

            case xc::messages::FunctionName::getGeometryNames :
            {
                return "getGeometryNames";
            }

            case xc::messages::FunctionName::getInstructionData :
            {
                return "getInstructionData";
            }

            case xc::messages::FunctionName::getInstructionRegion :
            {
                return "getInstructionRegion";
            }

            case xc::messages::FunctionName::getJobsList :
            {
                return "getJobsList";
            }

            case xc::messages::FunctionName::getModelInfo :
            {
                return "getModelInfo";
            }

            case xc::messages::FunctionName::getModelNames :
            {
                return "getModelNames";
            }

            case xc::messages::FunctionName::getModelsInfo :
            {
                return "getModelsInfo";
            }

            case xc::messages::FunctionName::getParent :
            {
                return "getParent";
            }

            case xc::messages::FunctionName::getPhysicsInfo :
            {
                return "getPhysicsInfo";
            }

            case xc::messages::FunctionName::getPhysicsProperties :
            {
                return "getPhysicsProperties";
            }

            case xc::messages::FunctionName::getRootUID :
            {
                return "getRootUID";
            }

            case xc::messages::FunctionName::getServerConfig :
            {
                return "getServerConfig";
            }

            case xc::messages::FunctionName::getServerInfo :
            {
                return "getServerInfo";
            }

            case xc::messages::FunctionName::getServerName :
            {
                return "getServerName";
            }

            case xc::messages::FunctionName::getSolverInfo :
            {
                return "getSolverInfo";
            }

            case xc::messages::FunctionName::getSystemNames :
            {
                return "getSystemNames";
            }

            case xc::messages::FunctionName::getVisual :
            {
                return "getVisual";
            }

            case xc::messages::FunctionName::importFile :
            {
                return "importFile";
            }

            case xc::messages::FunctionName::isReady :
            {
                return "isReady";
            }

            case xc::messages::FunctionName::load :
            {
                return "load";
            }

            case xc::messages::FunctionName::loadCLsource :
            {
                return "loadCLsource";
            }

            case xc::messages::FunctionName::loadConfig :
            {
                return "loadConfig";
            }

            case xc::messages::FunctionName::loadServerConfig :
            {
                return "loadServerConfig";
            }

            case xc::messages::FunctionName::lockSystem :
            {
                return "lockSystem";
            }

            case xc::messages::FunctionName::makeBackgroundWithSDF :
            {
                return "makeBackgroundWithSDF";
            }

            case xc::messages::FunctionName::makeGrid :
            {
                return "makeGrid";
            }

            case xc::messages::FunctionName::makeGroupFromSDF :
            {
                return "makeGroupFromSDF";
            }

            case xc::messages::FunctionName::makeMesh :
            {
                return "makeMesh";
            }

            case xc::messages::FunctionName::makeOctree :
            {
                return "makeOctree";
            }

            case xc::messages::FunctionName::makeSurfaceFromFaces :
            {
                return "makeSurfaceFromFaces";
            }

            case xc::messages::FunctionName::makeSurfaceFromVolume :
            {
                return "makeSurfaceFromVolume";
            }

            case xc::messages::FunctionName::makeVolumeFromSDF :
            {
                return "makeVolumeFromSDF";
            }

            case xc::messages::FunctionName::moveSystem :
            {
                return "moveSystem";
            }

            case xc::messages::FunctionName::newSystem :
            {
                return "newSystem";
            }

            case xc::messages::FunctionName::refreshSolver :
            {
                return "refreshSolver";
            }

            case xc::messages::FunctionName::removeAlgorithm :
            {
                return "removeAlgorithm";
            }

            case xc::messages::FunctionName::removeCondition :
            {
                return "removeCondition";
            }

            case xc::messages::FunctionName::removeGeometry :
            {
                return "removeGeometry";
            }

            case xc::messages::FunctionName::removeJob :
            {
                return "removeJob";
            }

            case xc::messages::FunctionName::removeModel :
            {
                return "removeModel";
            }

            case xc::messages::FunctionName::renameSystem :
            {
                return "renameSystem";
            }

            case xc::messages::FunctionName::reorderJobs :
            {
                return "reorderJobs";
            }

            case xc::messages::FunctionName::resetJob :
            {
                return "resetJob";
            }

            case xc::messages::FunctionName::resetTransform :
            {
                return "resetTransform";
            }

            case xc::messages::FunctionName::resetSystemTransform :
            {
                return "resetSystemTransform";
            }

            case xc::messages::FunctionName::reverseFaceWinding :
            {
                return "reverseFaceWinding";
            }

            case xc::messages::FunctionName::rotateSystem :
            {
                return "rotateSystem";
            }

            case xc::messages::FunctionName::save :
            {
                return "save";
            }

            case xc::messages::FunctionName::saveAll :
            {
                return "saveAll";
            }

            case xc::messages::FunctionName::saveCL :
            {
                return "saveCL";
            }

            case xc::messages::FunctionName::scaleSystem :
            {
                return "scaleSystem";
            }

            case xc::messages::FunctionName::seekFirst :
            {
                return "seekFirst";
            }

            case xc::messages::FunctionName::seekLast :
            {
                return "seekLast";
            }

            case xc::messages::FunctionName::seekNext :
            {
                return "seekNext";
            }

            case xc::messages::FunctionName::seekPrevious :
            {
                return "seekPrevious";
            }

            case xc::messages::FunctionName::setAlgorithmOrder :
            {
                return "setAlgorithmOrder";
            }

            case xc::messages::FunctionName::setConditionSequence :
            {
                return "setConditionSequence";
            }

            case xc::messages::FunctionName::setData :
            {
                return "setData";
            }

            case xc::messages::FunctionName::setDevice :
            {
                return "setDevice";
            }

            case xc::messages::FunctionName::setDOFs :
            {
                return "setDOFs";
            }

            case xc::messages::FunctionName::setInstructionData :
            {
                return "setInstructionData";
            }

            case xc::messages::FunctionName::setMeta :
            {
                return "setMeta";
            }

            case xc::messages::FunctionName::setSolver :
            {
                return "setSolver";
            }

            case xc::messages::FunctionName::setSolverSequence :
            {
                return "setSolverSequence";
            }

            case xc::messages::FunctionName::setSystemData :
            {
                return "setSystemData";
            }

            case xc::messages::FunctionName::setVariables :
            {
                return "setVariables";
            }

            case xc::messages::FunctionName::setWriteInterval :
            {
                return "setWriteInterval";
            }

            case xc::messages::FunctionName::solve :
            {
                return "solve";
            }

            case xc::messages::FunctionName::stopJob :
            {
                return "stopJob";
            }

            case xc::messages::FunctionName::translateSystem :
            {
                return "translateSystem";
            }

            case xc::messages::FunctionName::unlockSystem :
            {
                return "unlockSystem";
            }


            default :
            {
                return "FunctionName unknown";
            }
        }
    }

} }  // namespace xc::messages
