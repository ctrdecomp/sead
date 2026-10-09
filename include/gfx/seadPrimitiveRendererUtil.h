#ifndef SEAD_PRIMITIVE_RENDERER_UTIL_H_
#define SEAD_PRIMITIVE_RENDERER_UTIL_H_

#include <basis/seadTypes.h>
#include <gfx/seadColor.h>
#include <math/seadVector.h>

namespace sead
{
namespace PrimitiveRendererUtil
{
class Vertex
{
public:
    Vertex():
        pos(),
        uv(),
        color()
    {
    }

    Vertex(const Vector3f& pos, const Vector2f& uv, const Color4f& color): 
        pos(pos), 
        uv(uv), 
        color(color)
    {
    }

    Vector3f pos;
    Vector2f uv;
    Color4f color;
};

void setQuadVertex(Vertex* vtx, u16* idx);
void setLineVertex(Vertex* vtx, u16* idx);
void setCubeVertex(Vertex* vtx, u16* idx);
void setWireCubeVertex(Vertex* vtx, u16* idx);
void setSphereVertex(Vertex* vtx, u16* idx, s32 x, s32 y);
void setDiskVertex(Vertex* vtx, u16* idx, s32 div);
void setCylinderVertex(Vertex* vtx, u16* idx, s32 div);

inline s32 calcDiskVertexNum(s32 divNum){ return divNum + 1; }
inline s32 calcDiskIndexNum(s32 divNum) { return divNum * 3; }

inline s32 calcSphereVertexNum(s32 x, s32 y) { return x * y + 2; }
inline s32 calcSphereIndexNum(s32 x, s32 y) { return (x * 2 * (y + -1) + x * 2) * 3; }

inline s32 calcCylinderVertexNum(s32 divNum){ return calcDiskVertexNum(divNum) << 1; }
inline s32 calcCylinderIndexNum(s32 divNum){ return calcDiskIndexNum(divNum) * 2 + divNum * 6; }

}  // namespace PrimitiveRendererUtil
}  // namespace sead

#endif  // SEAD_PRIMITIVE_RENDERER_UTIL_H_

