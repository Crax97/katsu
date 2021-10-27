//
// Created by crax on 10/23/21.
//

#include "ast_visitor.h"

#include <clang-c/Index.h>
#include <memory>
#include <iostream>
#include <cassert>
#include <cstring>


struct visitor_fn_data {
    katsu::ast_visitor& visitor;
    std::string main_file_name;
    CXFile main_file;
    CXTranslationUnit main_unit;
}; 

void katsu::ast_visitor::reflect_class_begin(const CXCursor &cursor) {
    if(clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0) return;
    assert(!m_classes.empty());
    m_classes.back().is_reflecting = true;
}

void katsu::ast_visitor::visit_class_decl(const CXCursor& cursor) {
    if(clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0) return;
    auto class_name = clang_getCursorSpelling(cursor);
    const char* class_name_c = clang_getCString(class_name);
    std::list<std::string> namespace_names = collect_namespace_names();
    m_classes.push_back({cursor, class_name_c, false, {}, namespace_names});
    clang_disposeString(class_name);
    
}

void katsu::ast_visitor::visit_field_decl(const CXCursor &cursor) {
    if(clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0) return;
    auto type_spelling = clang_getTypeSpelling(clang_getCursorType(cursor));
    auto var_name = clang_getCursorSpelling(cursor);

    if(m_opts.is_debug) {
        std::cout << "Class: " << m_classes.back().class_name << "\n";
        std::cout << "\tIs reflecting? " << m_classes.back().is_reflecting << "\n";
        std::cout << "\tField " << clang_getCString(type_spelling) << " " << clang_getCString(var_name) << "\n";
    }

    if(!m_classes.back().is_reflecting) {
        auto semantic_parent = clang_getCursorSemanticParent(cursor);
        auto parent_name = clang_getCString(clang_getCursorSpelling(semantic_parent));
        std::cerr << "FAILURE while walking through " << clang_getCString(var_name) << " belonging to " << parent_name << "\n";
        std::cerr << "Maybe you forgot to REFLECT " << parent_name <<"?\n";
        std::exit(-1);
    }

    assert(!m_classes.empty());
    m_classes.back().fields.push_back({
                          cursor,
                          clang_getCString(type_spelling),
                          clang_getCString(var_name)
    });
    clang_disposeString(type_spelling);
    clang_disposeString(var_name);
}

void katsu::ast_visitor::visit_method_decl(const CXCursor &cursor) {

}

void katsu::ast_visitor::visit_annotation_decl(const CXCursor &cursor, const CXCursor& parent) {
    CXCursorKind parent_kind = clang_getCursorKind(parent);
    if(parent_kind == CXCursor_ClassDecl) {
        reflect_class_begin(parent);
    } else if (parent_kind == CXCursor_FieldDecl) {
        visit_field_decl(parent);
    }
}


void katsu::ast_visitor::dispatch_visit(const CXCursor &current, const CXCursor &parent, CXClientData Data) {
    CXCursorKind kind = clang_getCursorKind(current);
    CXCursorKind parent_kind = clang_getCursorKind(parent);

    if (kind == CXCursor_ClassDecl) {
        visit_class_decl(current);
    } else if (kind == CXCursor_Namespace) {
        visit_namespace_decl(current);
    } else if (kind == CXCursor_AnnotateAttr) {
        visit_annotation_decl(current, parent);
    } else if (kind == CXCursor_MacroExpansion) {
        // auto macro_text = clang_tokenize()
    }
}

CXCursor katsu::ast_visitor::get_class_namespace(const CXCursor &current) {
    CXCursor class_namespace = clang_getCursorSemanticParent(current);
    CXCursorKind namespace_kind = clang_getCursorKind(class_namespace);
    while (namespace_kind != CXCursor_Namespace && namespace_kind != CXCursor_TranslationUnit && namespace_kind != CXCursor_InvalidFile)  {
        class_namespace = clang_getCursorSemanticParent(class_namespace);
        namespace_kind = clang_getCursorKind(class_namespace);
    }
    return class_namespace;
}

katsu::ast_visitor katsu::ast_visitor::begin_visit(const CXTranslationUnit& translation_unit, const options& opts) {
 

    katsu::ast_visitor visitor(opts);
    auto visit_fn = [](CXCursor c, CXCursor p, CXClientData client_data)
    {
        auto visitor_data = *static_cast<visitor_fn_data*>(client_data);
        katsu::ast_visitor &in_visitor = visitor_data.visitor;
        CXFile cursor_file;
        clang_getFileLocation(clang_getCursorLocation(c), &cursor_file, nullptr, nullptr, nullptr);

        // Avoid reflecting classes that don't belong to the main file
        if(cursor_file == visitor_data.main_file) {
            in_visitor.dispatch_visit(c, p, client_data);
        }
        return CXChildVisit_Recurse;
    };
    CXCursor root_cursor = clang_getTranslationUnitCursor(translation_unit);
    auto tu_name = clang_getTranslationUnitSpelling(translation_unit);
    auto data = visitor_fn_data {
        visitor,
        clang_getCString(tu_name)
    };
    clang_disposeString(tu_name);

    data.main_file = clang_getFile(translation_unit, data.main_file_name.c_str());
    std::cout << "Begin from " << data.main_file_name << "\n";


    clang_visitChildren(root_cursor,
                        visit_fn, &data);
    return visitor;
}

void katsu::ast_visitor::visit_namespace_decl(const CXCursor &namespace_cursor) {
    auto namespace_parent = clang_getCursorSemanticParent(namespace_cursor);
    auto namespace_parent_kind = clang_getCursorKind(namespace_parent);
    if(namespace_parent_kind == CXCursor_TranslationUnit) {
        m_namespace_stack.clear();
    } else if (!m_namespace_stack.empty()) {
        while(!m_namespace_stack.empty() && (clang_equalCursors(namespace_parent, m_namespace_stack.back()) == 0)) {
            exit_namespace_decl(m_namespace_stack.back());
        }
    }
    m_namespace_stack.emplace_back(namespace_cursor);
}

void katsu::ast_visitor::exit_namespace_decl(CXCursor namespace_cursor) {
    m_namespace_stack.pop_back();
}

std::list<std::string> katsu::ast_visitor::collect_namespace_names() {
    std::list<std::string> names;
    for(auto& namesp_cursor : m_namespace_stack) {
        auto namesp_name = clang_getCursorSpelling(namesp_cursor);
        names.emplace_back(clang_getCString(namesp_name));
        clang_disposeString(namesp_name);
    }
    return names;
}
