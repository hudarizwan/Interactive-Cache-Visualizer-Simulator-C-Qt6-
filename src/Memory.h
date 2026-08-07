#pragma once

#include <QHash>
#include <QVector>
#include <QtGlobal>

class Memory
{
public:
    Memory();

    void setBlockSize(quint64 blockSizeBytes);
    void reset();

    QVector<quint64> readBlock(quint64 blockNumber) const;
    quint64 readWord(quint64 address) const;
    void writeWord(quint64 address, quint64 value);

private:
    quint64 m_blockSizeBytes;
    QHash<quint64, quint64> m_words;
};

