#pragma once

namespace sead
{
/// For storing an enum with a particular storage size when specifying the underlying type of the
/// enum is not an option.
template <typename Enum, typename Storage>
struct SizedEnum
{
    explicit SizedEnum(){ }
    explicit SizedEnum(Enum value) { *this = value; }
    operator Enum() const { return static_cast<Enum>(mValue); }
    SizedEnum& operator=(Enum value)
    {
        mValue = static_cast<Storage>(value);
        return *this;
    }

    Storage mValue;
};
}  // namespace sead
