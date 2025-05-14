#pragma once

#include <string>

namespace au::rhi {

class DescriptorGroup;

class PipelineLayout {
public:
    struct Description final {
        std::string cache;
        enum class CacheType {
            Data,
            File
        } cacheType;

        Description()
            : cache() // This means not using the cache.
            , cacheType(CacheType::Data) // Ignore cacheType if not using the cache.
        {}

        Description(
            std::string cache,
            CacheType cacheType)
            : cache(cache)
            , cacheType(cacheType)
        {}
    };

    virtual bool AddGroup(DescriptorGroup* group) = 0;
    virtual bool BuildLayout() = 0;

    virtual bool IsValid() const = 0;

    virtual std::string DumpCache() const = 0;

protected:
    PipelineLayout() = default;
    virtual ~PipelineLayout() = default;
};

}
