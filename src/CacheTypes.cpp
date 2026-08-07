#include "CacheTypes.h"

void CacheStats::reset()
{
    accesses = 0;
    hits = 0;
    misses = 0;
    evictions = 0;
}

double CacheStats::hitRate() const
{
    if (accesses == 0) {
        return 0.0;
    }
    return static_cast<double>(hits) / static_cast<double>(accesses);
}

double CacheStats::missRate() const
{
    if (accesses == 0) {
        return 0.0;
    }
    return static_cast<double>(misses) / static_cast<double>(accesses);
}

int CacheConfig::totalLines() const
{
    if (blockSizeBytes == 0) {
        return 0;
    }
    return static_cast<int>(cacheSizeBytes / blockSizeBytes);
}

int CacheConfig::setCount() const
{
    const int lines = totalLines();
    if (lines <= 0) {
        return 0;
    }
    if (mode == CacheMode::DirectMapped) {
        return lines;
    }
    if (mode == CacheMode::FullyAssociative) {
        return 1;
    }
    if (associativity <= 0) {
        return 0;
    }
    return lines / associativity;
}

int CacheConfig::waysPerSet() const
{
    const int lines = totalLines();
    if (lines <= 0) {
        return 0;
    }
    if (mode == CacheMode::DirectMapped) {
        return 1;
    }
    if (mode == CacheMode::FullyAssociative) {
        return lines;
    }
    return associativity;
}

int CacheConfig::blockOffsetBits() const
{
    return integerLog2(blockSizeBytes);
}

int CacheConfig::setBits() const
{
    return integerLog2(static_cast<quint64>(setCount()));
}

bool CacheConfig::isValid(QString *errorText) const
{
    if (cacheSizeBytes == 0 || blockSizeBytes == 0 || associativity <= 0) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Please enter valid cache parameters.");
        }
        return false;
    }

    if (!isPowerOfTwo(cacheSizeBytes) || !isPowerOfTwo(blockSizeBytes)) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Cache size and block size must be powers of 2.");
        }
        return false;
    }

    const int lines = totalLines();
    if (lines <= 0) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Cache size must be divisible by block size.");
        }
        return false;
    }

    if (cacheSizeBytes % blockSizeBytes != 0) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Cache size must be divisible by block size.");
        }
        return false;
    }

    if (mode == CacheMode::DirectMapped) {
        if (associativity != 1) {
            if (errorText) {
                *errorText = QStringLiteral("Error: Direct-mapped cache must use associativity 1.");
            }
            return false;
        }
        return true;
    }

    if (mode == CacheMode::FullyAssociative) {
        if (associativity != lines) {
            if (errorText) {
                *errorText = QStringLiteral("Error: Fully associative cache must use one set with all lines.");
            }
            return false;
        }
        return true;
    }

    if (!isPowerOfTwo(static_cast<quint64>(associativity))) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Associativity must be a power of 2.");
        }
        return false;
    }

    if (associativity > lines) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Associativity cannot exceed total cache lines.");
        }
        return false;
    }

    if (lines % associativity != 0) {
        if (errorText) {
            *errorText = QStringLiteral("Error: Number of cache lines must be divisible by associativity.");
        }
        return false;
    }

    return true;
}

QString modeName(CacheMode mode)
{
    switch (mode) {
    case CacheMode::DirectMapped:
        return QStringLiteral("Direct Mapped");
    case CacheMode::FullyAssociative:
        return QStringLiteral("Fully Associative");
    case CacheMode::SetAssociative:
        return QStringLiteral("K-Way Set Associative");
    }
    return QStringLiteral("Unknown");
}

QString policyName(ReplacementPolicyType policy)
{
    switch (policy) {
    case ReplacementPolicyType::LRU:
        return QStringLiteral("LRU");
    case ReplacementPolicyType::FIFO:
        return QStringLiteral("FIFO");
    case ReplacementPolicyType::Random:
        return QStringLiteral("Random");
    }
    return QStringLiteral("Unknown");
}

QString accessName(AccessType type)
{
    return type == AccessType::Read ? QStringLiteral("R") : QStringLiteral("W");
}

QString hexString(quint64 value)
{
    return QStringLiteral("0x%1").arg(value, 0, 16).toUpper();
}

bool isPowerOfTwo(quint64 value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

int integerLog2(quint64 value)
{
    if (value == 0) {
        return 0;
    }
    int bits = 0;
    while (value > 1) {
        value >>= 1;
        ++bits;
    }
    return bits;
}

