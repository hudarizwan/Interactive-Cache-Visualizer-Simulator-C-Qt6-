#include "CacheEngine.h"

#include <utility>

CacheEngine::CacheEngine()
    : m_policy(ReplacementPolicyType::LRU),
      m_clock(0)
{
}

bool CacheEngine::configure(const CacheConfig &config, QString *errorText)
{
    QString validationError;
    if (!config.isValid(&validationError)) {
        if (errorText) {
            *errorText = validationError;
        }
        return false;
    }

    m_config = config;
    m_config.configured = true;
    m_policy.setType(m_config.policy);
    m_memory.setBlockSize(m_config.blockSizeBytes);
    m_sets.clear();
    m_sets.resize(m_config.setCount());
    for (CacheSet &set : m_sets) {
        set.resize(m_config.waysPerSet());
    }
    reset();
    return true;
}

void CacheEngine::reset()
{
    m_stats.reset();
    m_clock = 0;
    m_memory.reset();
    for (CacheSet &set : m_sets) {
        set.reset();
    }
}

bool CacheEngine::configured() const
{
    return m_config.configured;
}

const CacheConfig &CacheEngine::config() const
{
    return m_config;
}

const CacheStats &CacheEngine::stats() const
{
    return m_stats;
}

QVector<CacheSet> &CacheEngine::sets()
{
    return m_sets;
}

const QVector<CacheSet> &CacheEngine::sets() const
{
    return m_sets;
}

CacheAccessResult CacheEngine::access(AccessType type, quint64 address)
{
    CacheAccessResult result;
    if (!configured()) {
        return result;
    }

    const DecodedAddress decoded = AddressDecoder::decode(address, m_config);
    if (decoded.setIndex >= static_cast<quint64>(m_sets.size())) {
        return result;
    }

    CacheSet &set = m_sets[static_cast<int>(decoded.setIndex)];
    const int hitWay = set.findHit(decoded.tag);

    ++m_clock;
    ++m_stats.accesses;

    result.valid = true;
    result.address = address;
    result.type = type;
    result.blockNumber = decoded.blockNumber;
    result.tag = decoded.tag;
    result.offset = decoded.offset;
    result.blockBaseAddress = decoded.blockBaseAddress;
    result.setIndex = static_cast<int>(decoded.setIndex);

    if (hitWay >= 0) {
        ++m_stats.hits;
        CacheLine &line = set.lineAt(hitWay);
        m_policy.onHit(line, m_clock);
        if (type == AccessType::Write) {
            line.markDirty();
        }
        result.hit = true;
        result.wayIndex = hitWay;
        return result;
    }

    ++m_stats.misses;

    const int freeWay = set.findFreeWay();
    const bool hasFreeLine = freeWay >= 0;
    const int targetWay = hasFreeLine ? freeWay : set.selectVictim(m_policy);
    if (!hasFreeLine) {
        ++m_stats.evictions;
        result.eviction = true;
        result.victimWay = targetWay;
    }

    CacheLine newLine;
    QVector<quint64> words = m_memory.readBlock(decoded.blockNumber);
    const bool dirty = type == AccessType::Write;
    newLine.load(decoded.tag, decoded.blockNumber, decoded.blockBaseAddress, std::move(words), m_clock, m_clock, dirty);
    m_policy.onInsert(newLine, m_clock);
    set.lineAt(targetWay) = std::move(newLine);
    result.wayIndex = targetWay;
    return result;
}

