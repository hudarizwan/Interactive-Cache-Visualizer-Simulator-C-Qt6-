#pragma once

#include "CacheLine.h"
#include "ReplacementPolicy.h"

class CacheSet
{
public:
    CacheSet();
    explicit CacheSet(int ways);

    void resize(int ways);
    void reset();

    int wayCount() const;
    int findHit(quint64 tag) const;
    int findFreeWay() const;
    int selectVictim(const ReplacementPolicy &policy) const;

    CacheLine &lineAt(int way);
    const CacheLine &lineAt(int way) const;
    QVector<CacheLine> &lines();
    const QVector<CacheLine> &lines() const;

private:
    QVector<CacheLine> m_lines;
};

