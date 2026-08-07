#include "AddressDecoder.h"

DecodedAddress AddressDecoder::decode(quint64 address, const CacheConfig &config)
{
    DecodedAddress decoded;
    decoded.address = address;
    decoded.offset = config.blockSizeBytes == 0 ? 0 : address % config.blockSizeBytes;
    decoded.blockBaseAddress = address - decoded.offset;
    decoded.blockNumber = config.blockSizeBytes == 0 ? 0 : address / config.blockSizeBytes;

    const quint64 sets = static_cast<quint64>(config.setCount());
    if (sets == 0) {
        decoded.setIndex = 0;
        decoded.tag = decoded.blockNumber;
        return decoded;
    }

    decoded.setIndex = decoded.blockNumber % sets;
    decoded.tag = decoded.blockNumber / sets;
    return decoded;
}

