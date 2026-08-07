#pragma once

#include "CacheTypes.h"

class AddressDecoder
{
public:
    static DecodedAddress decode(quint64 address, const CacheConfig &config);
};

