#include "CacheLine.h"

#include <utility>

CacheLine::CacheLine()
    : m_valid(false),
      m_dirty(false),
      m_tag(0),
      m_blockNumber(0),
      m_blockBaseAddress(0),
      m_loadedAt(0),
      m_lastUsedAt(0)
{
}

void CacheLine::load(quint64 tag, quint64 blockNumber, quint64 blockBaseAddress, QVector<quint64> words, qint64 loadedAt, qint64 lastUsedAt, bool dirty)
{
    m_valid = true;
    m_dirty = dirty;
    m_tag = tag;
    m_blockNumber = blockNumber;
    m_blockBaseAddress = blockBaseAddress;
    m_loadedAt = loadedAt;
    m_lastUsedAt = lastUsedAt;
    m_words = std::move(words);
}

void CacheLine::invalidate()
{
    m_valid = false;
    m_dirty = false;
    m_tag = 0;
    m_blockNumber = 0;
    m_blockBaseAddress = 0;
    m_loadedAt = 0;
    m_lastUsedAt = 0;
    m_words.clear();
}

void CacheLine::touch(qint64 tick)
{
    m_lastUsedAt = tick;
}

void CacheLine::markDirty()
{
    m_dirty = true;
}

bool CacheLine::valid() const
{
    return m_valid;
}

bool CacheLine::dirty() const
{
    return m_dirty;
}

quint64 CacheLine::tag() const
{
    return m_tag;
}

quint64 CacheLine::blockNumber() const
{
    return m_blockNumber;
}

quint64 CacheLine::blockBaseAddress() const
{
    return m_blockBaseAddress;
}

qint64 CacheLine::loadedAt() const
{
    return m_loadedAt;
}

qint64 CacheLine::lastUsedAt() const
{
    return m_lastUsedAt;
}

const QVector<quint64> &CacheLine::words() const
{
    return m_words;
}

