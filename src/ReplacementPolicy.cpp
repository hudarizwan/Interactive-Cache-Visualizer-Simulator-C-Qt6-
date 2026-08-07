#include "ReplacementPolicy.h"

#include <QRandomGenerator>

ReplacementPolicy::ReplacementPolicy(ReplacementPolicyType type)
    : m_type(type)
{
}

void ReplacementPolicy::setType(ReplacementPolicyType type)
{
    m_type = type;
}

ReplacementPolicyType ReplacementPolicy::type() const
{
    return m_type;
}

int ReplacementPolicy::selectVictim(const QVector<CacheLine> &lines) const
{
    if (lines.isEmpty()) {
        return -1;
    }

    if (m_type == ReplacementPolicyType::Random) {
        return static_cast<int>(QRandomGenerator::global()->bounded(lines.size()));
    }

    int victim = 0;
    qint64 bestStamp = lines.at(0).loadedAt();

    if (m_type == ReplacementPolicyType::LRU) {
        bestStamp = lines.at(0).lastUsedAt();
        for (int i = 1; i < lines.size(); ++i) {
            const qint64 stamp = lines.at(i).lastUsedAt();
            if (stamp < bestStamp) {
                bestStamp = stamp;
                victim = i;
            }
        }
        return victim;
    }

    for (int i = 1; i < lines.size(); ++i) {
        const qint64 stamp = lines.at(i).loadedAt();
        if (stamp < bestStamp) {
            bestStamp = stamp;
            victim = i;
        }
    }
    return victim;
}

void ReplacementPolicy::onHit(CacheLine &line, qint64 tick) const
{
    if (m_type == ReplacementPolicyType::LRU) {
        line.touch(tick);
    }
}

void ReplacementPolicy::onInsert(CacheLine &line, qint64 tick) const
{
    line.touch(tick);
}

void ReplacementPolicy::reset()
{
}

