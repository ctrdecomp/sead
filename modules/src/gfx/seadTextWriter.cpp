#include <gfx/seadTextWriter.h>

#include <gfx/seadGraphicsContext.h>

class GraphicsContextForTextWriter : public sead::GraphicsContext
{
public:
    GraphicsContextForTextWriter(): 
        sead::GraphicsContext()
    {
        setCullingMode(sead::Graphics::CullingMode::cNone);
        setDepthEnable(false, false);
    }
};

static GraphicsContextForTextWriter sGraphicsContext;

namespace sead 
{

TextWriter::~TextWriter()
{
}

void TextWriter::setupGraphics()
{
    sGraphicsContext.apply();
}

} // namespace sead