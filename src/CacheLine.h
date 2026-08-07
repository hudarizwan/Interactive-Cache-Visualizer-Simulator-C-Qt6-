#pragma once

#include <QVector>
#include <QtGlobal>

class CacheLine
{
public:
    CacheLine();

    void load(quint64 tag, quint64 blockNumber, quint64 blockBaseAddress, QVector<quint64> words, qint64 loadedAt, qint64 lastUsedAt, bool dirty);
    void invalidate();
    void touch(qint64 tick);
    void markDirty();

    bool valid() const;
    bool dirty() const;
    quint64 tag() const;
    quint64 blockNumber() const;
    quint64 blockBaseAddress() const;
    qint64 loadedAt() const;
    qint64 lastUsedAt() const;
    const QVector<quint64> &words() const;

private:
    bool m_valid;
    bool m_dirty;
    quint64 m_tag;
    quint64 m_blockNumber;
    quint64 m_blockBaseAddress;
    qint64 m_loadedAt;
    qint64 m_lastUsedAt;
    QVector<quint64> m_words;
};

