#pragma once

#include <optional>
#include <utility>

namespace sead
{
template <typename Function>
class ScopeGuard
{
public:
    explicit ScopeGuard(const Function& function) : mFunction(function) {}
    ~ScopeGuard() { exit(); }

    void dismiss() { mFunction = Function(); }

    void exit()
    {
        if (!mFunction)
            return;
        (*mFunction)();
        dismiss();
    }

private:
    Function mFunction;
};

}  // namespace sead

