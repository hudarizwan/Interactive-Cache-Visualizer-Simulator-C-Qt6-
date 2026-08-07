#pragma once

#include "AddressDecoder.h"
#include "CacheSet.h"
#include "Memory.h"

class CacheEngine
{
public:
    CacheEngine();

    bool configure(const CacheConfig &config, QString *errorText = nullptr);
    void reset();

    bool configured() const;
    const CacheConfig &config() const;
    const CacheStats &stats() const;
    QVector<CacheSet> &sets();
    const QVector<CacheSet> &sets() const;

    CacheAccessResult access(AccessType type, quint64 address);

private:
    CacheConfig m_config;
    CacheStats m_stats;
    QVector<CacheSet> m_sets;
    Memory m_memory;
    ReplacementPolicy m_policy;
    qint64 m_clock;
};

