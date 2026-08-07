#include "CacheSet.h"

CacheSet::CacheSet()
{
}

CacheSet::CacheSet(int ways)
{
    resize(ways);
}

void CacheSet::resize(int ways)
{
    m_lines = QVector<CacheLine>(ways);
}

void CacheSet::reset()
{
    for (CacheLine &line : m_lines) {
        line.invalidate();
    }
}

int CacheSet::wayCount() const
{
    return m_lines.size();
}

int CacheSet::findHit(quint64 tag) const
{
    for (int i = 0; i < m_lines.size(); ++i) {
        const CacheLine &line = m_lines.at(i);
        if (line.valid() && line.tag() == tag) {
            return i;
        }
    }
    return -1;
}

int CacheSet::findFreeWay() const
{
    for (int i = 0; i < m_lines.size(); ++i) {
        if (!m_lines.at(i).valid()) {
            return i;
        }
    }
    return -1;
}

int CacheSet::selectVictim(const ReplacementPolicy &policy) const
{
    return policy.selectVictim(m_lines);
}

CacheLine &CacheSet::lineAt(int way)
{
    return m_lines[way];
}

const CacheLine &CacheSet::lineAt(int way) const
{
    return m_lines[way];
}

QVector<CacheLine> &CacheSet::lines()
{
    return m_lines;
}

const QVector<CacheLine> &CacheSet::lines() const
{
    return m_lines;
}

