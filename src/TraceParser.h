#pragma once

#include "CacheTypes.h"

class TraceParser
{
public:
    static QVector<TraceLine> parse(const QString &text);
};

