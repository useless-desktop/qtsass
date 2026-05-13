#pragma once

#include <qtsass/functions.h>
#include <qtsass/importers.h>

namespace qtsass {

class QTSASS_EXPORT compiler
{
public:
    compiler();
    ~compiler();
    void add_include_path(const QString& path);
    void add_importer(const std::shared_ptr<Importer>& impl);
    void add_function(const std::shared_ptr<Function>& impl);

public:
    QString compile(const QString& content);
    QString compile_filename(const QString& input_file, const QString& output_file = {});

private:
    QStringList _include_paths;
    std::vector<std::shared_ptr<Importer>> _importers;
    std::vector<std::shared_ptr<Function>> _functions;
};

} // namespace qtsass

