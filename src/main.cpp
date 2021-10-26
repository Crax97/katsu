#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>

#include <clang-c/Index.h>

#include "options.h"
#include "ast_visitor.h"
#include "data_writer.h"

#define KATSU_DEFINE_IMPLEMENTATION
#include "option_parser.h"

options opts;

std::string read_file(const std::filesystem::path& file_path) {
    std::fstream file;
    file.open(file_path, std::iostream::in);
    if(!file) {
        std::cerr << "Could not open file " << file_path << "\n";
        std::exit(-1);
    }

    std::string file_content;
    std::string(line);
    while(std::getline(file, line)) {
        file_content.append(line);
        file_content.append("\n");
    }
    return file_content;
}

void write_file(const std::filesystem::path& file_path, const std::string& content) {
    std::fstream file;
    file.open(file_path, std::iostream::out | std::iostream::trunc);
    if(!file) {
        std::cerr << "Could not open file " << file_path << "\n";
        std::exit(-1);
    }

    file << content;
}

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cerr << "Wrong usage: Give me some files\n";
        return -1;
    }

    katsu::option_parser parser;
    // parser.add_option("o", "output folder", "gen");
    // parser.add_option("t", "templates folder", "templates);
    parser.enable_positional_arguments();
    parser.parse(argc, argv);

    auto output_path = parser.get_argument<std::string>("o");
    if(output_path) {
        opts.output_path = output_path.value();
    }

    auto templates_folder = parser.get_argument<std::string>("t");
    if(templates_folder) {
        opts.templates_folder = templates_folder.value();
    }
    auto abs_output_path = std::filesystem::absolute(opts.output_path);
    if(!exists(abs_output_path)) {
        std::filesystem::create_directory(abs_output_path);
    }
    
    opts.is_debug = parser.is_set("d");

    auto args = std::vector<std::string>({
            "c++",
            "-std=c++17",
            "-E"
    });

    if(parser.is_set("I")) {
        auto includes = parser.get_arguments("I").value();
        for(const auto& include_dir : includes) {
            auto include_dir_command = std::string("-I") + std::string(include_dir);
            std::cout << "Using additional include directory " << include_dir_command << "\n";
            args.emplace_back(std::move(include_dir_command));
        } 
    }
    katsu::data_writer_templates temp;
    temp.class_template = read_file(opts.templates_folder / "class.kh");
    temp.field_template = read_file(opts.templates_folder / "field.kh");
    temp.header_template = read_file(opts.templates_folder / "header.kh");

    katsu::data_writer writer(temp);

    const char** c_args = new const char*[args.size()];
    for(int i = 0; i < args.size(); i ++) {
        c_args[i] = args[i].c_str();
    }

    CXIndex index = clang_createIndex(0, 0);
    for(auto& source : parser.get_positional_arguments()) {


        // auto source_content = read_file(source);
        // replace_all(source_content, {
        //         {"REFLECT", "__attribute__((annotate(\"reflect\")))"}
        // });

        auto ext = std::filesystem::path(source).extension();
        auto filename = std::filesystem::path(source).filename().replace_extension("").string();
        auto output_filename = filename + std::string(".gen") + ext.string();
        // auto temp_filename = filename + std::string(".temp") + ext.string();

        // write_file(temp_filename, source_content);
        CXTranslationUnit unit = clang_parseTranslationUnit(
                index,
                source.data(),
                c_args, args.size(),
                nullptr, 0,
                CXTranslationUnit_None
        );

        if (unit == nullptr) {
            std::cerr << "Could not open source " << source << "\n";
            continue;
        }

        katsu::ast_visitor visitor = katsu::ast_visitor::begin_visit(unit, opts);
        clang_disposeTranslationUnit(unit);

        std::fstream output_file;
        output_file.open(abs_output_path / output_filename, std::ios_base::out | std::ios_base::trunc);
        if(!output_file) {
            std::cerr << "Could not open output file " << abs_output_path / output_filename << "\n";
            continue;
        }

        std::cout << "writing to " << output_filename << "\n";
        if(opts.is_debug) {
            writer.write_to_file(visitor, std::cout);
        } else {
            writer.write_to_file(visitor, output_file);
        }
    }
    delete c_args;
    clang_disposeIndex(index);
}
