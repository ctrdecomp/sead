#pragma once

#include <prim/seadSafeString.h>

namespace sead { 
namespace hostio {

enum Command
{
    cPing,
    cGameReset,
    cGetRootNode,
    cPutRootNode,
    cGetObjInfo,
    cPutObjInfo,
    cPropertyEvent,
    cPutUpdateProperties,
    cNodeEvent,
    cExecExtensionFunc,
    cExtensionFuncResult,
    cAddPalette,
    cRemovePalette,
    cPaletteEvent,
    cRemoteControl,
    cFileDialog,
    cFileDialogResut,
    cMessageBox,
    cMessageBoxResut,
    cUpdatePaletteMeta,
    cFolderDialog,
    cFolderDialogResut,
    cAddSecurityUnlockFile,
    cForceSetFolderCache,
    cInputBox,
    cTextResut,
    cSetLiteralEncoding,
    cUnk, // TODO
    cNumCommand
};

enum NodeEventID
{
    cSelected,
    cExpanded,
    cCollapsed,
    cLocalNodeSelected,
    cLocalNodeExpanded,
    cLocalNodeCollapsed,
    cChildDisposed,
    cTabSelected,
    cLocalNodeTabSelected
};

enum PaletteEventType
{
    cExecute,
    cFileDrop,
    cUserEvent
};

struct FileInfo
{
    SafeString path;
    u64 size;
};

struct TextResult
{
    SafeString text;
    u64 size;
};

} // namespace hostio
} // namespace sead
