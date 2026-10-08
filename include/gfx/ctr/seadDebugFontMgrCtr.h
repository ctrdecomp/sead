#pragma once

#include <gfx/seadFontBase.h>
#include <gfx/ctr/seadRamCommandCacheCtr.h>
#include <heap/seadDisposer.h>

#include <nn/gr.h>

namespace sead 
{
class DebugFontMgrCtr : public FontBase
{
    SEAD_SINGLETON_DISPOSER(DebugFontMgrCtr);

public:
    DebugFontMgrCtr();
    ~DebugFontMgrCtr();

    void initialize(Heap* heap);

    virtual f32 getHeight() const{ return 0x41000000f; }
    virtual f32 getWidth() const{ return 0x41000000f; }
    virtual f32 getCharWidth(char16 letter) const{ return 0x41000000f; }
    virtual Encoding getEncoding() const{ return cUTF16; }
    virtual void begin() const;
    virtual void end() const;
    virtual void print(const Projection& projection, const Camera& camera, const Matrix34f& mtx,
        const Color4f& color, const void* text, s32 length) const;

protected:
    void setMatricesForDisplay_(const Projection& projection, const Camera& camera, const Matrix34f& mtx) const;

protected:
    RawCommandCacheCtr mTopScreenCommand;
    RawCommandCacheCtr mBtmScreenCommand;

    BindSymbolVSInput mPositionSymbol;
    BindSymbolVSInput mUVMapSymbol;
    BindSymbolVSInput mUVOffsetSymbol;
    BindSymbolVSFloat mWvpSymbol;
    BindSymbolVSFloat mColorSymbol;
    BindSymbolVSFloat mUvSymbol;

    Vertex mMenuVertex;
    Vertex::IndexStream mVertexStream;

    Vector3f mVector3[200]; // change name
    Vector4f mVector4[200]; // change name
};

} // namespace sead
