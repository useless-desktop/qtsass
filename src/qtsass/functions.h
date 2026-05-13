#pragma once

#include <qtsass/global.h>

#include <sass/values.h>
#include <sass/context.h>
#include <sass/functions.h>

namespace qtsass {

struct QTSASS_EXPORT Function {
    using Impl = union Sass_Value* (*)(const union Sass_Value* s_args, Sass_Function_Entry cb, struct Sass_Compiler* comp);
    explicit Function() = default;
    virtual ~Function() = default;
    virtual QString signature() const = 0;
    virtual Impl function() const = 0;
    virtual void* cookie() const { return nullptr; }
};

} // namespace qtsass

namespace qtsass {

struct Function_QRgba : public Function
{
public:
    QString signature() const override;
    Impl function() const override;
};

} // namespace qtsass

namespace qtsass {

struct Function_QLinearGradient : public Function
{
public:
    QString signature() const override;
    Impl function() const override;
};

} // namespace qtsass

namespace qtsass {

struct Function_QRadialGradient : public Function
{
public:
    QString signature() const override;
    Impl function() const override;
};

} // namespace qtsass

