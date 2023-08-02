#pragma once

#include <string>

namespace au::rhi {

class DescriptorGroup;

class PipelineLayout {
public:
    struct Description {
        std::string cache; // Unsupported yet
    };

    virtual void AddGroup(DescriptorGroup* group) = 0;
    virtual void BuildLayout() = 0;

protected:
    PipelineLayout() = default;
    virtual ~PipelineLayout() = default;
};

}
