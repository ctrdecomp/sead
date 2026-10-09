#pragma once

#include "basis/seadTypes.h"

namespace sead { 
namespace hostio {

class Config
{
public:
    Config(): 
        mSendBufferSize(0x10000), 
        mRecvBufferSize(0x2000), 
        mMaxRecvDataSize(0x2000), 
        //mWinTCPPortNumber(9010), 
        mCTRSerialChannel(1), 
        mORBehavior(13)
    {
        setPacketTransferBlank(20);
    }

    void setPacketTransferBlank(s32 milisec)
    {
        mORBehavior &= 0xFF;
        mORBehavior |= (milisec << 8);
    }

    u32 getSendBufferSize() const { return mSendBufferSize; }
    u32 getRecvBufferSize() const { return mRecvBufferSize; }
    u32 getRecvMaxDataSize() const { return mMaxRecvDataSize; }
    u32 getORBehavior() const { return mORBehavior; }
    //u32 getWinTCPPortNumber() const { return mWinTCPPortNumber; }
    u32 getCTRSerialChannel() const { return mCTRSerialChannel; }

private:
    u32 mSendBufferSize;
    u32 mRecvBufferSize;
    u32 mMaxRecvDataSize;
    //u32 mWinTCPPortNumber;
    u32 mCTRSerialChannel;
    u32 mORBehavior;
};

} // namespace hostio
} // namespace sead
