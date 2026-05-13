#include "compiler.h"
#include "helper.h"
#include "conformers.h"
#include "functions.h"
#include "importers.h"

#include <sass/context.h>

namespace qtsass {

static void set_custom_functions(struct Sass_Options* ctx_opt, const std::vector<std::shared_ptr<Function>>& functions) {
    const auto count = functions.size();
    // create list of all custom functions
    auto fn_list = sass_make_function_list(count);
    for (int i = 0; i < count; i++) {
        const auto& function = functions[i];
        // allocate a custom function caller
        const auto signature = function->signature().toUtf8();
        auto fn = sass_make_function(signature.data(), function->function(), function->cookie());
        sass_function_set_list_entry(fn_list, i, fn);
    }
    sass_option_set_c_functions(ctx_opt, fn_list);
}

static void set_custom_importers(struct Sass_Options* ctx_opt, const std::vector<std::shared_ptr<Importer>>& importers) {
    const auto count = importers.size();
    // create list for all custom importers
    auto imp_list = sass_make_importer_list(count);
    for (int i = 0; i < count; i++) {
        const auto& importer = importers[i];
        // allocate custom importer
        auto imp = sass_make_importer(importer->function(), importer->priority(), importer->cookie());
        // put only the importer on to the list
        sass_importer_set_list_entry(imp_list, i, imp);
    }
    // register list on to the context options
    sass_option_set_c_importers(ctx_opt, imp_list);
}

static void set_custom_include_paths(struct Sass_Options* ctx_opt, const QStringList& include_paths) {
    if (!include_paths.isEmpty()) {
        for (auto include_path : include_paths) {
            const auto path = include_path.toUtf8();
            sass_option_push_include_path(ctx_opt, path.data());
        }
    }
}

QString compiler::compile(const QString& content) {
    QElapsedTimer timer;
    timer.start();
    /*
    Conform and Compile QtSASS source code to CSS.
    This function conforms QtSASS to valid SCSS before passing it to
    sass.compile. Any keyword arguments you provide will be combined with
    qtsass's default keyword arguments and passed to sass.compile.
    .. code-block:: python
        >>> import qtsass
        >>> qtsass.compile("QWidget {background: rgb(0, 0, 0);}")
        QWidget {background:black;}

    :param string: QtSASS source code to conform and compile.
    :param kwargs: Keyword arguments to pass to sass.compile
    :returns: CSS string
    */
    auto content_data = scss_conform(content).toUtf8();
    // LibSass will take control of data you pass in
    // Therefore we need to make a copy of static data
    char* source_data = sass_copy_c_string(content_data.data());
    // Normally you'll load data into a buffer from i.e. the disk.
    // Use `sass_alloc_memory` to get a buffer to pass to LibSass
    // then fill it with data you load from disk or somewhere else.

    // create the data context and get all related structs
    auto* data_ctx = sass_make_data_context(source_data);
    auto* ctx = sass_data_context_get_context(data_ctx);
    auto* ctx_opt = sass_context_get_options(ctx);

    // set options
    set_custom_functions(ctx_opt, _functions);
    set_custom_importers(ctx_opt, _importers);
    set_custom_include_paths(ctx_opt, _include_paths);

#ifdef _DEBUG
    sass_option_set_output_style(ctx_opt, SASS_STYLE_NESTED);
#else
    sass_option_set_output_style(ctx_opt, SASS_STYLE_COMPRESSED);
#endif // _DEBUG
    sass_option_set_precision(ctx_opt, 10);

    // 
    int status = sass_compile_data_context(data_ctx);
    QByteArray output_data;
    if (status == 0) {
        output_data = QByteArray(sass_context_get_output_string(ctx));
    } else {
        qWarning() << "sass error" << sass_context_get_error_message(ctx);
    }

    // release allocated memory
    sass_delete_data_context(data_ctx);

    QString output_string = QString::fromUtf8(output_data);
    QString qcss = qt_conform(output_string);

#ifdef _DEBUG
    qDebug() << "qtsass compile to" << qcss;
#endif // _DEBUG
    qInfo() << "qtsass compile elapsed" << timer.elapsed();

    return qcss;
}

QString compiler::compile_filename(const QString& input_file, const QString& output_file) {
    auto input_root = QFileInfo(input_file).absolutePath();
    auto qss = read_file(input_file);
    qInfo() << "Sass Compiling" << input_file << "...";
    _include_paths.push_back(input_root);
    auto css = compile(qss);
    if (!output_file.isEmpty()) {
        auto output_root = QFileInfo(output_file).absoluteDir();
        if (!output_root.exists()) {
            output_root.mkpath(output_root.absolutePath());
        }
        qInfo() << "Created CSS file" << output_file;
        write_file(output_file, css);
    }
    return css;
}

compiler::compiler() {
    _functions = {
        std::make_shared<Function_QRgba>(),
        std::make_shared<Function_QLinearGradient>(),
        std::make_shared<Function_QRadialGradient>(),
    };

    _importers = {
        std::make_shared<Importer_QSS>(),
    };
}

compiler::~compiler() {

}

void compiler::add_include_path(const QString& path) {
    _include_paths.push_back(path);
}

void compiler::add_importer(const std::shared_ptr<Importer>& impl) {
    _importers.push_back(impl);
}

void compiler::add_function(const std::shared_ptr<Function>& impl) {
    _functions.push_back(impl);
}

} // namespace qtsass
