cmake_minimum_required(VERSION 3.24)

project(sead C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED ENV{ARMCC_PATH})
    message(FATAL_ERROR "please set ARMCC_PATH")
endif()

find_program(ARM_ASM NAMES armasm.exe PATHS $ENV{ARMCC_PATH}/bin REQUIRED NO_DEFAULT_PATH)
find_program(ARM_AR NAMES armar.exe PATHS $ENV{ARMCC_PATH}/bin REQUIRED NO_DEFAULT_PATH)
find_program(ARM_CC NAMES armcc.exe PATHS $ENV{ARMCC_PATH}/bin REQUIRED NO_DEFAULT_PATH)
find_program(ARM_LINK NAMES armlink.exe PATHS $ENV{ARMCC_PATH}/bin REQUIRED NO_DEFAULT_PATH)

macro (set_compilers)
    set(CMAKE_CXX_COMPILER_ID ARMCC)

    set(CMAKE_AR ${ARM_AR})
    set(CMAKE_C_COMPILER ${ARM_CC})
    set(CMAKE_CXX_COMPILER ${ARM_CC})
    set(CMAKE_ASM_COMPILER ${ARM_ASM})
    set(CMAKE_LINKER ${ARM_LINK})
endmacro()

if (WIN32)
    set_compilers()
    set(CMAKE_C_COMPILER_WORKS TRUE)
    set(CMAKE_CXX_COMPILER_WORKS TRUE)
    set(CMAKE_ASM_COMPILER_WORKS TRUE)
endif()

__compiler_armcc(C)
__compiler_armcc(CXX)
__compiler_armcc(ASM)

option(SEAD_DEBUG "Enable debug assertions and warnings" OFF)
set(SEAD_PLATFORM "ctr" CACHE STRING "Target platform (default: ctr)")
set_property(
  CACHE SEAD_PLATFORM
  PROPERTY STRINGS "ctr"
)

# LibMessageStudio

add_subdirectory(addins/libms)

# Main Library

add_library(sead OBJECT
  include/audio/seadAudioGlobal.h
  include/audio/seadAudioPlayer.h
  include/audio/seadAudioSettingParameter.h
  include/audio/seadAudioSystem.h

  include/basis/seadAssert.h
  include/basis/seadDefines.h
  include/basis/seadNew.h
  include/basis/seadRawPrint.h
  include/basis/seadTypes.h
  include/basis/seadWarning.h
  modules/src/basis/seadAssert.cpp
  modules/src/basis/seadNew.cpp
  modules/src/basis/seadRawPrint.cpp
  modules/src/basis/seadWarning.cpp

  include/codec/seadBase64.h
  include/codec/seadHashCRC16.h
  include/codec/seadHashCRC32.h
  modules/src/codec/seadHashCRC16.cpp
  modules/src/codec/seadHashCRC32.cpp

  include/container/seadBuffer.h
  include/container/seadFreeList.h
  include/container/seadListImpl.h
  include/container/seadObjArray.h
  include/container/seadObjList.h
  include/container/seadOffsetList.h
  include/container/seadOrderedSet.h
  include/container/seadPtrArray.h
  include/container/seadRingBuffer.h
  include/container/seadSafeArray.h
  include/container/seadStrTreeMap.h
  include/container/seadTList.h
  include/container/seadTreeMap.h
  include/container/seadTreeNode.h
  modules/src/container/seadListImpl.cpp
  modules/src/container/seadPtrArray.cpp
  modules/src/container/seadTreeNode.cpp

  include/controller/seadAccelerometerAddon.h
  include/controller/seadControlDevice.h
  include/controller/seadController.h
  include/controller/seadControllerAddon.h
  include/controller/seadControllerBase.h
  include/controller/seadControllerDefine.h
  include/controller/seadControllerMgr.h
  include/controller/seadControllerWrapper.h
  include/controller/seadControllerWrapperBase.h
  include/controller/seadPatternRumbleAddon.h
  modules/src/controller/seadController.cpp
  modules/src/controller/seadControllerBase.cpp
  modules/src/controller/seadControllerMgr.cpp
  modules/src/controller/seadControllerWrapper.cpp
  modules/src/controller/seadControllerWrapperBase.cpp
  modules/src/controller/seadPatternRumbleAddon.cpp

  include/devenv/seadAssertConfig.h
  include/devenv/seadEnvUtil.h
  include/devenv/seadFontMgr.h
  #include/devenv/seadGameConfig.h
  include/devenv/seadPrintConfig.h
  include/devenv/seadSeadMenuMgr.h
  include/devenv/seadStackTrace.h
  modules/src/devenv/seadAssertConfig.cpp
  modules/src/devenv/seadFontMgr.cpp
  #modules/src/devenv/seadGameConfig.cpp
  modules/src/devenv/seadPrintConfig.cpp
  modules/src/devenv/seadStackTrace.cpp

  include/filedevice/seadAnyFileDevice.h
  include/filedevice/seadArchiveFileDevice.h
  include/filedevice/seadFileDevice.h
  include/filedevice/seadFileDeviceMgr.h
  include/filedevice/seadMainFileDevice.h
  include/filedevice/seadPath.h
  modules/src/filedevice/seadAnyFileDevice.cpp
  modules/src/filedevice/seadArchiveFileDevice.cpp
  modules/src/filedevice/seadFileDevice.cpp
  modules/src/filedevice/seadFileDeviceMgr.cpp
  modules/src/filedevice/seadMainFileDevice.cpp
  modules/src/filedevice/seadPath.cpp

  include/framework/seadDualScreenMethodTreeMgr.h
  include/framework/seadDualScreenTask.h
  include/framework/seadCalculateTask.h
  include/framework/seadFaderTask.h
  include/framework/seadFramework.h
  include/framework/seadGameFramework.h
  include/framework/seadHeapPolicies.h
  include/framework/seadInfLoopChecker.h
  include/framework/seadInfLoopCheckerThread.h
  include/framework/seadMethodTree.h
  include/framework/seadMethodTreeMgr.h
  include/framework/seadProcessMeter.h
  include/framework/seadProcessMeterBar.h
  include/framework/seadSingleScreenMethodTreeMgr.h
  include/framework/seadTask.h
  include/framework/seadTaskBase.h
  include/framework/seadTaskID.h
  include/framework/seadTaskMgr.h
  include/framework/seadTaskParameter.h
  modules/src/framework/seadDualScreenMethodTreeMgr.cpp
  modules/src/framework/seadDualScreenTask.cpp
  modules/src/framework/seadCalculateTask.cpp
  modules/src/framework/seadFaderTask.cpp
  modules/src/framework/seadFramework.cpp
  modules/src/framework/seadGameFramework.cpp
  modules/src/framework/seadInfLoopChecker.cpp
  modules/src/framework/seadInfLoopCheckerThread.cpp
  modules/src/framework/seadMethodTree.cpp
  modules/src/framework/seadMethodTreeMgr.cpp
  modules/src/framework/seadProcessMeter.cpp
  modules/src/framework/seadProcessMeterBar.cpp
  modules/src/framework/seadSingleScreenMethodTreeMgr.cpp
  modules/src/framework/seadTask.cpp
  modules/src/framework/seadTaskBase.cpp
  modules/src/framework/seadTaskID.cpp
  modules/src/framework/seadTaskMgr.cpp

  include/geom/seadLine.h
  modules/src/geom/seadLine.cpp

  include/gfx/seadCamera.h
  include/gfx/seadColor.h
  include/gfx/seadDrawLockContext.h
  include/gfx/seadFontBase.h
  include/gfx/seadFrameBuffer.h
  include/gfx/seadGraphics.h
  include/gfx/seadGraphicsContext.h
  include/gfx/seadPrimitiveDrawer.h
  include/gfx/seadPrimitiveDrawMgrBase.h
  include/gfx/seadPrimitiveRenderer.h
  include/gfx/seadPrimitiveRendererUtil.h
  include/gfx/seadProjection.h
  include/gfx/seadTexture.h
  include/gfx/seadTextureUtil.h
  include/gfx/seadTextWriter.h
  include/gfx/seadViewport.h
  modules/src/gfx/seadCamera.cpp
  modules/src/gfx/seadColor.cpp
  modules/src/gfx/seadDrawLockContext.cpp
  modules/src/gfx/seadFrameBuffer.cpp
  modules/src/gfx/seadGraphics.cpp
  modules/src/gfx/seadGraphicsContext.cpp
  modules/src/gfx/seadPrimitiveDrawer.cpp
  modules/src/gfx/seadPrimitiveRenderer.cpp
  modules/src/gfx/seadPrimitiveRendererUtil.cpp
  modules/src/gfx/seadProjection.cpp
  modules/src/gfx/seadTextureUtil.cpp
  modules/src/gfx/seadTextWriter.cpp
  modules/src/gfx/seadViewport.cpp

  include/heap/seadArena.h
  include/heap/seadDisposer.h
  include/heap/seadExpHeap.h
  include/heap/seadFrameHeap.h
  include/heap/seadHeap.h
  include/heap/seadHeapMgr.h
  include/heap/seadMemBlock.h
  modules/src/heap/seadDisposer.cpp
  modules/src/heap/seadExpHeap.cpp
  modules/src/heap/seadHeap.cpp
  modules/src/heap/seadHeapMgr.cpp

  include/math/seadBoundBox.h
  include/math/seadBoundBox.hpp
  include/math/seadMathBase.h
  include/math/seadMathCalcCommon.h
  include/math/seadMathCalcCommon.hpp
  include/math/seadMathNumbers.h
  include/math/seadMathPolicies.h
  include/math/seadMatrix.h
  include/math/seadMatrix.hpp
  include/math/seadMatrixCalcCommon.h
  include/math/seadMatrixCalcCommon.hpp
  include/math/seadQuat.h
  include/math/seadQuat.hpp
  include/math/seadQuatCalcCommon.h
  include/math/seadQuatCalcCommon.hpp
  include/math/seadVector.h
  include/math/seadVector.hpp
  include/math/seadVectorCalcCommon.h
  include/math/seadVectorCalcCommon.hpp
  include/math/seadVectorFwd.h
  modules/src/math/seadBoundBox.cpp
  modules/src/math/seadMathCalcCommon.cpp
  modules/src/math/seadMathNumbers.cpp
  modules/src/math/seadMatrix.cpp
  modules/src/math/seadQuat.cpp
  modules/src/math/seadVector.cpp

  include/prim/seadBitFlag.h
  include/prim/seadBitUtil.h
  include/prim/seadContainerIterator.h
  include/prim/seadDelegate.h
  include/prim/seadDelegateEventSlot.h
  include/prim/seadEndian.h
  include/prim/seadEnum.h
  include/prim/seadFormatPrint.h
  include/prim/seadFormatPrint.hpp
  include/prim/seadLongBitFlag.h
  include/prim/seadMemUtil.h
  include/prim/seadNamable.h
  include/prim/seadPtrUtil.h
  include/prim/seadRuntimeTypeInfo.h
  include/prim/seadSafeString.h
  include/prim/seadSafeString.hpp
  include/prim/seadScopedLock.h
  include/prim/seadScopeGuard.h
  include/prim/seadSizedEnum.h
  include/prim/seadStorageFor.h
  include/prim/seadStringBuilder.h
  include/prim/seadStringUtil.h
  include/prim/seadTypedBitFlag.h
  include/prim/seadTypedLongBitFlag.h
  modules/src/prim/seadBitFlag.cpp
  modules/src/prim/seadEndian.cpp
  modules/src/prim/seadEnum.cpp
  modules/src/prim/seadFormatPrint.cpp
  modules/src/prim/seadMemUtil.cpp
  modules/src/prim/seadSafeString.cpp
  modules/src/prim/seadStringBuilder.cpp
  modules/src/prim/seadStringUtil.cpp

  include/random/seadGlobalRandom.h
  include/random/seadRandom.h
  modules/src/random/seadGlobalRandom.cpp
  modules/src/random/seadRandom.cpp

  include/resource/seadArchiveRes.h
  include/resource/seadDecompressor.h
  include/resource/seadResource.h
  include/resource/seadResourceMgr.h
  include/resource/seadSharcArchiveRes.h
  include/resource/seadSZSDecompressor.h
  modules/src/resource/seadArchiveRes.cpp
  modules/src/resource/seadResource.cpp
  modules/src/resource/seadResourceMgr.cpp
  modules/src/resource/seadSharcArchiveRes.cpp
  modules/src/resource/seadSZSDecompressor.cpp

  include/stream/seadBufferStream.h
  include/stream/seadFileDeviceStream.h
  include/stream/seadPrintStream.h
  include/stream/seadRamStream.h
  include/stream/seadStream.h
  include/stream/seadStreamFormat.h
  include/stream/seadStreamSrc.h
  modules/src/stream/seadBufferStream.cpp
  modules/src/stream/seadFileDeviceStream.cpp
  modules/src/stream/seadPrintStream.cpp
  modules/src/stream/seadRamStream.cpp
  modules/src/stream/seadStream.cpp
  modules/src/stream/seadStreamFormat.cpp

  include/thread/seadAtomic.h
  include/thread/seadCriticalSection.h
  include/thread/seadDelegateThread.h
  include/thread/seadEvent.h
  include/thread/seadMessageQueue.h
  include/thread/seadMutex.h
  include/thread/seadThread.h
  include/thread/seadThreadLocalStorage.h
  include/thread/seadThreadUtil.h
  modules/src/thread/seadDelegateThread.cpp
  modules/src/thread/seadThread.cpp
  modules/src/thread/seadThreadUtil.cpp

  include/time/seadCalendarSpan.h
  include/time/seadCalendarTime.h
  include/time/seadDateSpan.h
  include/time/seadDateTime.h
  include/time/seadDateUtil.h
  include/time/seadTickSpan.h
  include/time/seadTickTime.h
  modules/src/time/seadCalendarTime.cpp
  modules/src/time/seadDateSpan.cpp
  modules/src/time/seadDateTime.cpp
  modules/src/time/seadDateUtil.cpp
  modules/src/time/seadTickSpan.cpp
  modules/src/time/seadTickTime.cpp
)

target_sources(sead PRIVATE
  include/hostio/seadHostIOCurve.h
  modules/src/hostio/seadHostIOCurve.cpp
)

# Debug



if(SEAD_DEBUG)
  target_sources(sead PRIVATE
    include/hostio/seadHostIOConfig.h
    include/hostio/seadHostIOContext.h
    include/hostio/seadHostIOCurve.h
    include/hostio/seadHostIODefine.h
    include/hostio/seadHostIOEventListener.h
    include/hostio/seadHostIOFramework.h
    include/hostio/seadHostIOMgr.h
    include/hostio/seadHostIONode.h
    include/hostio/seadHostIOPortBase.h
    include/hostio/seadHostIOReflexible.h
    include/hostio/seadHostIORoot.h
    include/hostio/seadHostIOServer.h
    include/hostio/seadHostIOThreadLock.h
    modules/src/hostio/seadHostIOContext.cpp
    modules/src/hostio/seadHostIOCurve.cpp
    modules/src/hostio/seadHostIOEventListener.cpp
    modules/src/hostio/seadHostIOFramework.cpp
    modules/src/hostio/seadHostIOMgr.cpp
    modules/src/hostio/seadHostIONode.cpp
    modules/src/hostio/seadHostIOReflexible.cpp
    modules/src/hostio/seadHostIORoot.cpp
    modules/src/hostio/seadHostIOServer.cpp
    modules/src/hostio/seadHostIOThreadLock.cpp
  )
endif()

target_link_libraries(sead PUBLIC LibMessageStudio)

# PLATFORM: CTR

if(SEAD_PLATFORM STREQUAL "ctr")
  if(NOT TARGET CTR_SDK)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}../CTR_SDK) # change depend on prj
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}../NintendoWare) # change depend on prj
  endif()
  target_link_libraries(sead PUBLIC CTR_SDK)

  target_compile_definitions(sead PRIVATE SEAD_PLATFORM_CTR=1 SEAD_USE_NW4C=1)

  if(SEAD_DEBUG)
    add_compile_definitions(SEAD_DEBUG)
  else()
    add_compile_definitions(SEAD_RELEASE)
  endif()

  target_sources(sead PRIVATE
    modules/src/basis/ctr/seadRawPrintCtr.cpp

    include/controller/ctr/seadCtrAccelerometerAddon.h
    include/controller/ctr/seadCtrController.h
    include/controller/ctr/seadCtrDebugControllerCtr.h
    include/controller/ctr/seadCtrHidDeviceCtr.h
    modules/src/controller/ctr/seadCtrAccelerometerAddon.cpp
    modules/src/controller/ctr/seadCtrController.cpp
    modules/src/controller/ctr/seadCtrDebugControllerCtr.cpp
    modules/src/controller/ctr/seadCtrHidDeviceCtr.cpp

    include/devenv/ctr/seadDirectPrintCtr.h
    include/devenv/ctr/seadExceptionScreenCtr.h
    modules/src/devenv/ctr/seadAssertCtr.cpp
    modules/src/devenv/ctr/seadDirectPrintCtr.cpp
    modules/src/devenv/ctr/seadExceptionScreenCtr.cpp

    include/filedevice/ctr/seadCtrFileStreamFileDeviceCtr.h
    include/filedevice/ctr/seadCtrFileDevice.h
    include/filedevice/ctr/seadCtrBackupFileDevice.h
    #include/filedevice/ctr/seadCtrHostIOFileDevice.h
    include/filedevice/ctr/seadCtrSDFileDeviceCtr.h
    modules/src/filedevice/ctr/seadCtrFileStreamFileDeviceCtr.cpp
    modules/src/filedevice/ctr/seadCtrFileDevice.cpp
    modules/src/filedevice/ctr/seadCtrBackupFileDevice.cpp
    #modules/src/filedevice/ctr/seadCtrHostIOFileDevice.cpp
    modules/src/filedevice/ctr/seadCtrSDFileDeviceCtr.cpp

    include/framework/seadUlcdMethodTreeMgr.h
    include/framework/seadUlcdTask.h
    include/framework/ctr/seadConsoleFrameworkCtr.h
    include/framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h
    include/framework/ctr_nw4c/seadDoubleCmdGameFrameworkCtrNw4c.h
    include/framework/ctr_nw4c/seadSeriesGameFrameworkCtrNw4c.h
    include/framework/ctr_nw4c/seadUlcdDoubleCmdGameFrameworkCtrNw4c.h
    include/framework/ctr_nw4c/seadUlcdGameFrameworkCtrNw4c.h
    modules/src/framework/seadUlcdMethodTreeMgr.cpp
    modules/src/framework/seadUlcdTask.cpp
    modules/src/framework/ctr/seadConsoleFrameworkCtr.cpp
    modules/src/framework/ctr_nw4c/seadGameFrameworkCtrNw4c.cpp
    modules/src/framework/ctr_nw4c/seadDoubleCmdFrameworkCtrNw4c.cpp
    modules/src/framework/ctr_nw4c/seadSeriesGameFrameworkCtrNw4c.cpp
    modules/src/framework/ctr_nw4c/seadUlcdDoubleCmdGameFrameworkCtrNw4c.cpp
    modules/src/framework/ctr_nw4c/seadUlcdGameFrameworkCtrNw4c.cpp

    include/gfx/seadStereoCameraWrapper.h
    include/gfx/seadStereoUtil.h
    include/gfx/ctr/seadFrameBufferCtr.h
    include/gfx/ctr/seadGfxMemoryMgrCtr.h
    include/gfx/ctr/seadGraphicsCtr.h
    include/gfx/ctr/seadPrimitiveRendererCtr.h
    include/gfx/ctr/seadRamCommandCacheCtr.h
    include/gfx/ctr/seadRamCommandCacheCtr.hpp
    include/gfx/ctr/seadStereoUtilCtr.h
    include/gfx/ctr/seadTextureCtr.h
    include/gfx/ctr/seadTextureCtrGL.h
    include/gfx/ctr/seadTextureCtrGR.h
    modules/src/gfx/seadStereoCameraWrapper.cpp
    modules/src/gfx/seadStereoUtil.cpp
    modules/src/gfx/ctr/seadFrameBufferCtr.cpp
    modules/src/gfx/ctr/seadGfxMemoryMgrCtr.cpp
    modules/src/gfx/ctr/seadGraphicsCtr.cpp
    modules/src/gfx/ctr/seadPrimitiveRendererCtr.cpp
    modules/src/gfx/ctr/seadStereoUtilCtr.cpp
    modules/src/gfx/ctr/seadTextureCtr.cpp
    modules/src/gfx/ctr/seadTextureCtrGL.cpp
    modules/src/gfx/ctr/seadTextureCtrGR.cpp

    modules/src/heap/ctr/seadArenaCtr.cpp

    include/math/ctr/seadMathCalcCtr.hpp
    include/math/ctr/seadMatrixCalcCtr.h
    include/math/ctr/seadQuatCalcCtr.h
    include/math/ctr/seadQuatCalcCtr.hpp
    include/math/ctr/seadVectorCalcCtr.h
    include/math/ctr/seadVectorCalcCtr.hpp
    modules/src/math/ctr/seadMatrixCalcCtr.cpp

    include/message/seadMessageSet.h
    modules/src/message/seadMessageSet.cpp

    include/prim/ctr/seadMemUtilCtr.hpp
    modules/src/prim/ctr/seadMemUtilCtr.cpp

    include/stream/ctr/seadMemoryMapCtr.h
    include/stream/ctr/seadMemoryMapCtr.hpp

    include/tentative/seadBitmapBuilder.h
    modules/src/tentative/seadBitmapBuilder.cpp

    include/thread/ctr/seadThreadLocalStorageCtr.hpp
    modules/src/thread/ctr/seadCriticalSectionCtr.cpp
    modules/src/thread/ctr/seadEventCtr.cpp
    modules/src/thread/ctr/seadMessageQueueCtr.cpp
    modules/src/thread/ctr/seadMutexCtr.cpp
    modules/src/thread/ctr/seadThreadCtr.cpp
  )
endif()

target_include_directories(sead PUBLIC include/)
target_include_directories(sead PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/addins/libms/include)

if(SEAD_DEBUG)
  target_compile_definitions(sead PUBLIC SEAD_DEBUG=1)
endif()
