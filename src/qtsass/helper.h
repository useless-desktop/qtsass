#pragma once

#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace qtsass {

inline void encoding(QTextStream& stream) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Utf8);
#endif
}

inline QString read_file(const QString& filepath) {
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        encoding(in);
        return in.readAll();
    }
    return QString();
}

inline void write_file(const QString& filepath, const QString& content) {
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        encoding(out);
        out << content;
        out.flush();
    }
}

} // namespace qtsass
