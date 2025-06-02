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
        unsigned int constantBufferBytes;

        Description(bool useConstantBuffer)
            : cache() // This means not using the cache.
            , cacheType(CacheType::Data) // Ignore cacheType because not using the cache.
            , constantBufferBytes(useConstantBuffer ? 128 : 0)
        {}

        Description(
            std::string cache,
            CacheType cacheType)
            : cache(cache)
            , cacheType(cacheType)
            , constantBufferBytes(0) // Ignore it because layout is built from cache.
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
