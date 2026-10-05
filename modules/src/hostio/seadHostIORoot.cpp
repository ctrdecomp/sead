#include <hostio/seadHostIORoot.h>

#include <seadVersion.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIOEvent.h>
#include <prim/seadEndian.h>

#include <nw/version.h>
#include <nn/version.h>

namespace sead {

#if defined(SEAD_DEBUG)
void HostIORoot::listenPropertyEvent(const hostio::PropertyEvent* ev)
{
    switch (ev->id)
    {
        case 'asrt':
        {
            SEAD_ASSERT_MSG(false, "Test that stops at SEAD_ASSERT_MSG");
            break;
        }

        case 'dtex':
        {
            // ...
            break;
        }

        case 'nnab':
        {
            // ...
            break;
        }
    }
}

void HostIORoot::genMessage(hostio::Context* ctx)
{
    const s32 cResultSize = 1024;
    const s32 cTmpSize = 128;

    char result[cResultSize];
    char tmp[cTmpSize];

    BufferedSafeString t(tmp, cTmpSize);
    BufferedSafeString r(result, cResultSize);

    r.format("");
    t.format("<h1>sead</h1>");

    r.append(t);
    r.append("<table>");

    t.format("<tr><td>sead Version</td><td>%d.%d.%d.%d</td></tr>", SEAD_VERSION_MAJOR, SEAD_VERSION_MINOR, SEAD_VERSION_MICRO, SEAD_VERSION_PATCH);
    r.append(t);

    t.format("<tr><td>nw4c Version</td><td>%d.%d.%d</td></tr>", NW_VERSION_MAJOR, NW_VERSION_MINOR, NW_VERSION_MICRO);
    r.append(t);

    t.format("<tr><td>ctrsdk Version</td><td>%d.%d.%d.%d</td></tr>", NN_VERSION_MAJOR, NN_VERSION_MINOR, NN_VERSION_MICRO, NN_VERSION_ID);
    r.append(t);

    if (Endian::getHostEndian() == Endian::cBig)
    {
        r.append("<tr><td>endian</td><td>Big</td></tr>");
    }
    else
    {
        r.append("<tr><td>endian</td><td>Little</td></tr>");
    }

#if defined(SEAD_DEBUG)
    r.append("<tr><td>build target</td><td>Debug</td></tr>");
#elif
    r.append("<tr><td>build target</td><td>Release</td></tr>");
#endif

    t.format("<tr><td>data model</td><td>INT(%d), LONG(%d), PTR(%d)</td></tr>", sizeof(int) * 8, sizeof(long) * 8, sizeof(void*) * 8);
    r.append(t);

    if (HeapMgr::instance()->getRootHeap(0))
    {
        t.format("<tr><td>memory size</td><td>%d [byte]</td></tr>", HeapMgr::instance()->getRootHeap(0)->getSize());
        r.append(t);
    }

    r.append("</table>");

    ctx->startLayout("Layout = Stack , Dir = X");
        ctx->genHTMLLabel(r, "");
    ctx->endLayout();

    ctx->startNode("Debug", "", 0, nullptr);
    {
        ctx->genButton("【Caution】Stop with SEAD_ASSERT_MSG", 'asrt', "", nullptr);
        ctx->genButton("【Caution】Stop with data access exception", 'dtex', "", nullptr);
        ctx->genButton("【Caution】Stop with NN_ABORT", 'nnab', "", nullptr);
    }
    ctx->endNode();
}
#endif // SEAD_TARGET_DEBUG

} // namespace sead