#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>

#include <clang-c/Index.h>

#include "ast_visitor.h"
#include "data_writer.h"

#define KATSU_DEFINE_IMPLEMENTATION
#include "option_parser.h"

struct options {
    std::filesystem::path output_path = "gen";
    std::filesystem::path templates_folder = "templates";
} opts;

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
    const char* args[] = {
            "c++",
            "-std=c++17",
            "-DREFLECT=__attribute__((annotate(\"reflect\")))"
    };

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

    katsu::data_writer_templates temp;
    temp.class_template = read_file(opts.templates_folder / "class.kh");
    temp.field_template = read_file(opts.templates_folder / "field.kh");
    temp.header_template = read_file(opts.templates_folder / "header.kh");

    katsu::data_writer writer(temp);

    for(auto& source : parser.get_positional_arguments()) {


        auto source_content = read_file(source);
        replace_all(source_content, {
                {"REFLECT", "__attribute__((annotate(\"reflect\")))"}
        });
        write_file("temp.hpp", source_content);
        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit unit = clang_parseTranslationUnit(
                index,
                "temp.hpp",
                args, 2,
                nullptr, 0,
                CXTranslationUnit_None
        );

        if (unit == nullptr) {
            std::cerr << "Could not open source " << source << "\n";
            continue;
        }

        katsu::ast_visitor visitor = katsu::ast_visitor::begin_visit(unit);
        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);

        auto ext = std::filesystem::path(source).extension();
        auto filename = std::filesystem::path(source).filename().replace_extension("").string();
        auto output_filename = filename + std::string(".gen") + ext.string();

        std::fstream output_file;
        output_file.open(abs_output_path / output_filename, std::ios_base::out | std::ios_base::trunc);
        if(!output_file) {
            std::cerr << "Could not open output file " << abs_output_path / output_filename << "\n";
            continue;
        }

        std::cout << "writing to " << output_filename << "\n";

        writer.write_to_file(visitor, output_file);
    }
}
