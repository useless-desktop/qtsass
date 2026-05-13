#include "importers.h"
#include "conformers.h"
#include "helper.h"

#include <sass/context.h>

namespace qtsass {

Sass_Import_List qss_importer(const char* path, Sass_Importer_Entry cb, struct Sass_Compiler* compiler)
{
    auto opt = sass_compiler_get_options(compiler);
    auto importer_file = sass_find_include(path, opt);
    if (importer_file == nullptr)
        return {};

    auto content = scss_conform(read_file(QString::fromUtf8(importer_file)));
    auto import_str = content.toUtf8();
    auto list = sass_make_import_list(1);
    char* importer_conent = sass_copy_c_string(import_str.data());
    list[0] = sass_make_import_entry(importer_file, importer_conent, 0);
    sass_free_memory(importer_file);
    return list;
}

Importer::Impl Importer_QSS::function() const {
    return &qss_importer;
}

} // namespace qtsass
