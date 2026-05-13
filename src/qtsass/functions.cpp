
#include "functions.h"

namespace qtsass {

static QString get_color(const union Sass_Value* value) {
    if (sass_value_is_list(value)) {
        const auto r = sass_number_get_value(sass_list_get_value(value, 0));
        const auto g = sass_number_get_value(sass_list_get_value(value, 1));
        const auto b = sass_number_get_value(sass_list_get_value(value, 2));
        const auto a = sass_number_get_value(sass_list_get_value(value, 3));
        const auto au = sass_number_get_unit(sass_list_get_value(value, 3));
        double alpha = 255;
        if (strncmp(au, "%", 1) == 0) {
            alpha = a;
        } else if (a > 1.0) {
            // A value from 0 to 255 is coming in, convert to %
            alpha = a / 2.55;
        } else {
            alpha = a * 100;
        }
        return QString("rgba(%1, %2, %3, %4%)")
            .arg(int(r))
            .arg(int(g))
            .arg(int(b))
            .arg(int(alpha));
    } else if (sass_value_is_color(value)) {
        const auto r = sass_color_get_r(value);
        const auto g = sass_color_get_g(value);
        const auto b = sass_color_get_b(value);
        const auto a = sass_color_get_a(value);
        return QString("rgba(%1, %2, %3, %4%)")
            .arg(int(r))
            .arg(int(g))
            .arg(int(b))
            .arg(int(a * 100));
    }
    return QString::fromUtf8(sass_string_get_value(value));
}

static union Sass_Value* rgba(const union Sass_Value* s_args, Sass_Function_Entry cb, struct Sass_Compiler* comp) {
    const auto data = get_color(s_args).toUtf8();
    return sass_make_string(data.data());
}

QString Function_QRgba::signature() const {
    return QStringLiteral("rgba($red, $green, $blue, $alpha)");
}

Function::Impl Function_QRgba::function() const {
    return &rgba;
}

} // namespace qtsass

namespace qtsass {

static QString qt_stop(const union Sass_Value* value) {
    const auto result = QString("stop: %1 %2");
    const auto pos = sass_number_get_value(sass_list_get_value(value, 0));
    const auto color = get_color(sass_list_get_value(value, 1));
    return result.arg(pos).arg(color);
}

static QStringList qt_stops(const union Sass_Value* list) {
    const auto stop_size = sass_list_get_length(list);
    QStringList stops_str;
    for (size_t i = 0; i < stop_size; ++i) {
        auto value = sass_list_get_value(list, i);
        stops_str.append(qt_stop(value));
    }
    return stops_str;
}

static QString qt_qlineargradient(double x1, double y1, double x2, double y2, const QStringList& stops)
{
    const auto templateText = QString("qlineargradient(x1:%1, y1:%2, x2:%3, y2:%4, %5)");
    return templateText.arg(x1).arg(y1).arg(x2).arg(y2).arg(stops.join(", "));
}

static union Sass_Value* qlineargradient(const union Sass_Value* s_args, Sass_Function_Entry cb, struct Sass_Compiler* comp)
{
    /*
        :type x1: sass.SassNumber
        :type y1: sass.SassNumber
        :type x2: sass.SassNumber
        :type y2: sass.SassNumber
        :type stops: sass.SassList
    */
    const auto x1 = sass_number_get_value(sass_list_get_value(s_args, 0));
    const auto y1 = sass_number_get_value(sass_list_get_value(s_args, 1));
    const auto x2 = sass_number_get_value(sass_list_get_value(s_args, 2));
    const auto y2 = sass_number_get_value(sass_list_get_value(s_args, 3));
    const auto stops = qt_stops(sass_list_get_value(s_args, 4));
    const auto data = qt_qlineargradient(x1, y1, x2, y2, stops).toUtf8();
    return sass_make_string(data.data());
}

QString Function_QLinearGradient::signature() const {
    return QStringLiteral("qlineargradient($x1, $y1, $x2, $y2, $stops)");
}

Function::Impl Function_QLinearGradient::function() const {
    return &qlineargradient;
}

} // namespace qtsass


namespace qtsass {

static QString qt_qradialgradient(const QString& spread, double cx, double cy, double radius, double fx, double fy, const QStringList& stops)
{
    const auto templateText = QString("qradialgradient(spread: %0, cx: %1, cy: %2, radius: %3, fx: %4, fy: %5, %6)");
    return templateText.arg(spread).arg(cx).arg(cy).arg(radius).arg(fx).arg(fy).arg(stops.join(", "));
}

static union Sass_Value* qradialgradient(const union Sass_Value* s_args, Sass_Function_Entry cb, struct Sass_Compiler* comp)
{
    /*
    :type spread: string
    :type cx: sass.SassNumber
    :type cy: sass.SassNumber
    :type radius: sass.SassNumber
    :type fx: sass.SassNumber
    :type fy: sass.SassNumber
    :type stops: sass.SassList
    */
    const auto spread = QString::fromUtf8(sass_string_get_value(sass_list_get_value(s_args, 0)));
    const auto cx     = sass_number_get_value(sass_list_get_value(s_args, 1));
    const auto cy     = sass_number_get_value(sass_list_get_value(s_args, 2));
    const auto radius = sass_number_get_value(sass_list_get_value(s_args, 3));
    const auto fx     = sass_number_get_value(sass_list_get_value(s_args, 4));
    const auto fy     = sass_number_get_value(sass_list_get_value(s_args, 5));
    const auto stops  = qt_stops(sass_list_get_value(s_args, 6));
    const auto data = qt_qradialgradient(spread, cx, cy, radius, fx, fy, stops).toUtf8();
    return sass_make_string(data.data());
}

QString Function_QRadialGradient::signature() const {
    return QStringLiteral("qradialgradient($spread, $cx, $cy, $radius, $fx, $fy, $stops)");
}

Function::Impl Function_QRadialGradient::function() const {
    return &qradialgradient;
}

} // namespace qtsass
