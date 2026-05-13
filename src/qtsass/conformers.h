#pragma once

#include <qtsass/global.h>

#include <sass/values.h>
#include <sass/context.h>

namespace qtsass {

QString scss_conform(const QString& input_str);
QString qt_conform(const QString& input_str);

} // namespace qtsass
