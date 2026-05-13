#include "conformers.h"

namespace qtsass {

class Conformer
{
public:
    explicit Conformer() = default;
    virtual ~Conformer() = default;
    virtual QString to_scss(const QString& qss) = 0;
    virtual QString to_qss(const QString& css) = 0;
};

class NotConformer : public Conformer
{   // """Conform QSS "!" in selectors."""
public:
    // """Replace "!" in selectors with "_qnot_"."""
    QString to_scss(const QString& qss) override {
        auto conformed = qss;
        return conformed.replace(":!", ":_qnot_");
    }

    // """Replace "_qnot_" in selectors with "!"."""
    QString to_qss(const QString& css) override {
        auto conformed = css;
        return conformed.replace(":_qnot_", ":!");
    }
};

/*
    Take a qss str with stops and returns the values.
      'stop: 0 red, stop: 1 blue' => '0 red, 1 blue'
*/
static QString _conform_stops_to_scss(const QString& group) {
    QStringList new_group;
    QStringList split{ "" };
    int bracket_level = 0;
    for (int i = 0; i < group.size(); i++) {
        auto _char = group[i];
        if (!bracket_level && _char == ',') {
            split.append("");
            continue;
        } else if (_char == '(') {
            bracket_level += 1;
        } else if (_char == ')') {
            bracket_level -= 1;
        }
        split[split.size() - 1] += _char;
    }

    for (auto part : split) {
        const auto& key_values = part.split(':');
        if (key_values.size() >= 2) {
            const auto key = key_values[0].trimmed();
            const auto value = key_values[1].trimmed();
            new_group.append(value);
        }
    }

    return new_group.join(", ");
}

class QLinearGradientConformer : public Conformer {
    constexpr static char _qss_pattern[] =
        R"(qlineargradient\()"
        // coords
        R"((?<coords>(?:(?:\s+)?(?:x1|y1|x2|y2):(?:\s+)?[0-9A-Za-z$_\.-]+,?)+))"
        // stops
        R"((?<stops>(?:(?:\s+)?stop:.*,?)+(?:\s+)?)?)"
        R"(\))"
        ;
    QRegularExpression qss_pattern;
    QStringList _DEFAULT_COORDS = { "x1", "y1", "x2", "y2" };

    /*
        Take a qss str with xy coords and returns the values.
          'x1: 0, y1: 0, x2: 0, y2: 0' => '0, 0, 0, 0'
          'y1: 1' => '0, 1, 0, 0'
    */
    QString _conform_coords_to_scss(const QString& group) {
        QStringList values = { "0", "0", "0", "0" };
        for (const auto& part : group.split(',')) {
            const auto& key_values = part.split(':');
            if (key_values.size() >= 2) {
                const auto key   = key_values[0].trimmed();
                const auto value = key_values[1].trimmed();
                const auto pos = _DEFAULT_COORDS.indexOf(key);
                if (pos >= 0 && pos <= 3) {
                    values[pos] = value;
                }
            }
        }
        return values.join(", ");
    }

public:
    QLinearGradientConformer() {
        qss_pattern.setPattern(_qss_pattern);
        qss_pattern.setPatternOptions(QRegularExpression::MultilineOption);
    }

    QString to_scss(const QString& qss) override {
        auto conformed = qss;
        auto it = qss_pattern.globalMatch(qss);
        while (it.hasNext()) {
            const auto match = it.next();
            if (match.hasMatch()) {
                const auto coords = match.captured("coords");
                const auto stops = match.captured("stops");
                auto new_coords = _conform_coords_to_scss(coords);
                conformed = conformed.replace(coords, new_coords);
                if (stops.isEmpty())
                    continue;
                auto new_stops = QString(", (%0)").arg(_conform_stops_to_scss(stops));
                conformed = conformed.replace(stops, new_stops);
            }
        }
        return conformed;
    }

    QString to_qss(const QString& css) override {
        return css;
    }
};

class QRadialGradientConformer : public Conformer {
    QStringList _DEFAULT_COORDS = { "cx", "cy", "radius", "fx", "fy" };
    constexpr static char _qss_pattern[] =
        R"(qradialgradient\()"
        // # spread
        R"((?<spread>(?:(?:\s+)?(?:spread):(?:\s+)?[0-9A-Za-z$_\.-]+,?)+)?)"
        // # coords
        R"((?<coords>(?:(?:\s+)?(?:cx|cy|radius|fx|fy):(?:\s+)?[0-9A-Za-z$_\.-]+,?)+))"
        // # stops
        R"((?<stops>(?:(?:\s+)?stop:.*,?)+(?:\s+)?)?)"
        R"(\))"
        ;
    QRegularExpression qss_pattern;

    /*
        Take a qss str with xy coords and returns the values.
        'spread: pad|repeat|reflect'
    */
    QString _conform_spread_to_scss(const QString& group) {
        QString spread = "pad";
        for (const auto& part : group.split(',')) {
            const auto& key_values = part.split(':');
            if (key_values.size() >= 2) {
                const auto key = key_values[0].trimmed();
                const auto value = key_values[1].trimmed();
                if (key == "spread") {
                    spread = value;
                }
            }
        }
        return spread;
    }
    /*
        Take a qss str with xy coords and returns the values.
          'cx: 0, cy: 0, radius: 0, fx: 0, fy: 0' => '0, 0, 0, 0, 0'
          'cy: 1' => '0, 1, 0, 0, 0'
    */
    QString _conform_coords_to_scss(const QString& group) {
        QStringList values = { "0", "0", "0", "0", "0" };
        for (const auto& part : group.split(',')) {
            const auto& key_values = part.split(':');
            if (key_values.size() >= 2) {
                const auto key = key_values[0].trimmed();
                const auto value = key_values[1].trimmed();
                const auto pos = _DEFAULT_COORDS.indexOf(key);
                if (pos >= 0) {
                    values[pos] = value;
                }
            }
        }
        return values.join(", ");
    }

public:
    QRadialGradientConformer() {
        qss_pattern.setPattern(_qss_pattern);
        qss_pattern.setPatternOptions(QRegularExpression::MultilineOption);
    }
    /*
        Conform qss qradialgradient to scss qradialgradient form.
        Normalize all whitespace including the removal of newline chars.
        qradialgradient(cx: 0, cy: 0, radius: 0, fx: 0, fy: 0, stop: 0 red, stop: 1 blue)
        =>
        qradialgradient(0, 0, 0, 0, 0, (0 red, 1 blue))
    */
    QString to_scss(const QString& qss) override {
        auto conformed = qss;
        auto it = qss_pattern.globalMatch(qss);
        while (it.hasNext()) {
            const auto match = it.next();
            if (match.hasMatch()) {
                const auto spread = match.captured("spread");
                const auto coords = match.captured("coords");
                const auto stops = match.captured("stops");
                auto new_spread = "'" + _conform_spread_to_scss(spread) + "', ";
                conformed = conformed.replace(spread, new_spread);
                auto new_coords = _conform_coords_to_scss(coords);
                conformed = conformed.replace(coords, new_coords);
                if (stops.isEmpty())
                    continue;
                auto new_stops = QString(", (%0)").arg(_conform_stops_to_scss(stops));
                conformed = conformed.replace(stops, new_stops);
            }
        }
        return conformed;
    }

    QString to_qss(const QString& css) override {
        return css;
    }

};

} // namespace qtsass

namespace qtsass {

using ConformerCreator = std::function<std::shared_ptr<Conformer>()>;

class ConformerFactory
{
    inline static std::vector<ConformerCreator> creator_list = {};
public:
    static void register_conformer(const ConformerCreator creator) {
        creator_list.push_back(creator);
    }

    static std::vector<std::shared_ptr<Conformer>> conformers() {
        std::vector<std::shared_ptr<Conformer>> conformer_list;
        conformer_list.reserve(creator_list.size());
        for (ConformerCreator creator : std::as_const(creator_list)) {
            conformer_list.push_back(std::invoke(creator));
        }
        return conformer_list;
    }
};

#define QTSASS_REGISTER_CONFORMER(TYPE)                           \
namespace {                                                       \
static const struct __QTSASS_CONFORMER_##TYPE{                    \
    inline static ConformerCreator creator = [] {                 \
            return std::make_shared<TYPE>();                      \
    };                                                            \
    inline __QTSASS_CONFORMER_##TYPE() {                          \
        ConformerFactory::register_conformer(creator);            \
    }                                                             \
    inline ~__QTSASS_CONFORMER_##TYPE() {                         \
                                                                  \
    }                                                             \
} __QTSASS_CONFORMER_##TYPE##_instance_; }

QTSASS_REGISTER_CONFORMER(NotConformer)
QTSASS_REGISTER_CONFORMER(QLinearGradientConformer)
QTSASS_REGISTER_CONFORMER(QRadialGradientConformer)

} // namespace qtsass

namespace qtsass {

QString scss_conform(const QString& input_str) {
    auto conformed = input_str;
    for (auto con : ConformerFactory::conformers()) {
        conformed = con->to_scss(conformed);
    }
    return conformed;
}

QString qt_conform(const QString& input_str) {
    auto conformed = input_str;
    for (auto con : ConformerFactory::conformers()) {
        conformed = con->to_qss(conformed);
    }
    return conformed;
}

} // namespace qtsass
