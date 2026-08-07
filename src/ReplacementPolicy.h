#pragma once

#include "CacheLine.h"
#include "CacheTypes.h"

class ReplacementPolicy
{
public:
    explicit ReplacementPolicy(ReplacementPolicyType type = ReplacementPolicyType::LRU);

    void setType(ReplacementPolicyType type);
    ReplacementPolicyType type() const;

    int selectVictim(const QVector<CacheLine> &lines) const;
    void onHit(CacheLine &line, qint64 tick) const;
    void onInsert(CacheLine &line, qint64 tick) const;
    void reset();

private:
    ReplacementPolicyType m_type;
};

