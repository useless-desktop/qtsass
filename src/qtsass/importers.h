#pragma once

#include <qtsass/global.h>

#include <sass/values.h>
#include <sass/context.h>

namespace qtsass {

struct QTSASS_EXPORT Importer {
    using Impl = Sass_Import_List(*)(const char* path, Sass_Importer_Entry cb, struct Sass_Compiler* comp);
    explicit Importer() = default;
    virtual ~Importer() = default;

    virtual Impl function() const = 0;
    virtual double priority() const { return 0; }
    virtual void* cookie() const { return nullptr; }
};

} // namespace qtsass

namespace qtsass {

struct Importer_QSS : public Importer {
    Impl function() const override;
};

} // namespace qtsass
