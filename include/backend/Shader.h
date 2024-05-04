#pragma once

#include <string>
#include "BasicTypes.h"

namespace au::rhi {

class Shader {
public:
    struct Description final {
        ShaderStage stage;
        std::string source;
        std::string entryName;
        enum class SourceType {
            Source,      // source code
            SourceFile,  // source file
            Bytecode,    // shader bytecode
            BytecodeFile // bytecode file
        } sourceType;

        Description(
            ShaderStage stage,
            std::string source,
            std::string entryName = "main",
            SourceType sourceType = SourceType::Source)
            : stage(stage)
            , source(source)
            , entryName(entryName)
            , sourceType(sourceType)
        {}
    };

    virtual bool IsValid() const = 0;

    struct Reflection {
        unsigned int descriptorsCount = 0;
        // TODO...
    };
    // TODO
    virtual Reflection Reflect() const = 0;

    virtual std::string DumpBytecode() const = 0;

protected:
    Shader() = default;
    virtual ~Shader() = default;
};

}
