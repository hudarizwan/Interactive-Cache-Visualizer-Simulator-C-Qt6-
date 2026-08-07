#pragma once

#include <QString>
#include <QVector>
#include <QtGlobal>

enum class CacheMode
{
    DirectMapped,
    FullyAssociative,
    SetAssociative
};

enum class ReplacementPolicyType
{
    LRU,
    FIFO,
    Random
};

enum class AccessType
{
    Read,
    Write
};

struct TraceLine
{
    int lineNumber = 0;
    QString rawText;
    bool valid = false;
    AccessType type = AccessType::Read;
    quint64 address = 0;
    QString errorText;
};

struct CacheStats
{
    qint64 accesses = 0;
    qint64 hits = 0;
    qint64 misses = 0;
    qint64 evictions = 0;

    void reset();
    double hitRate() const;
    double missRate() const;
};

struct CacheConfig
{
    CacheMode mode = CacheMode::SetAssociative;
    ReplacementPolicyType policy = ReplacementPolicyType::LRU;
    quint64 cacheSizeBytes = 64;
    quint64 blockSizeBytes = 8;
    int associativity = 2;
    bool configured = false;

    int totalLines() const;
    int setCount() const;
    int waysPerSet() const;
    int blockOffsetBits() const;
    int setBits() const;
    bool isValid(QString *errorText = nullptr) const;
};

struct DecodedAddress
{
    quint64 address = 0;
    quint64 blockNumber = 0;
    quint64 setIndex = 0;
    quint64 tag = 0;
    quint64 offset = 0;
    quint64 blockBaseAddress = 0;
};

struct CacheAccessResult
{
    bool valid = false;
    bool hit = false;
    bool eviction = false;
    int setIndex = -1;
    int wayIndex = -1;
    int victimWay = -1;
    quint64 address = 0;
    quint64 blockNumber = 0;
    quint64 tag = 0;
    quint64 offset = 0;
    quint64 blockBaseAddress = 0;
    AccessType type = AccessType::Read;
};

QString modeName(CacheMode mode);
QString policyName(ReplacementPolicyType policy);
QString accessName(AccessType type);
QString hexString(quint64 value);
bool isPowerOfTwo(quint64 value);
int integerLog2(quint64 value);

