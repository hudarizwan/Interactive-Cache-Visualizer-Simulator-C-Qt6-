#include "Memory.h"

Memory::Memory()
    : m_blockSizeBytes(1)
{
}

void Memory::setBlockSize(quint64 blockSizeBytes)
{
    m_blockSizeBytes = blockSizeBytes == 0 ? 1 : blockSizeBytes;
}

void Memory::reset()
{
    m_words.clear();
}

QVector<quint64> Memory::readBlock(quint64 blockNumber) const
{
    QVector<quint64> words;
    words.reserve(static_cast<int>(m_blockSizeBytes));
    const quint64 baseAddress = blockNumber * m_blockSizeBytes;
    for (quint64 i = 0; i < m_blockSizeBytes; ++i) {
        words.push_back(readWord(baseAddress + i));
    }
    return words;
}

quint64 Memory::readWord(quint64 address) const
{
    return m_words.value(address, address);
}

void Memory::writeWord(quint64 address, quint64 value)
{
    m_words.insert(address, value);
}

