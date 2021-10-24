//
// Created by crax on 10/23/21.
//

#include "ast_visitor.h"
#include <clang-c/Index.h>
#include <memory>
#include <iostream>
#include <cassert>

void katsu::ast_visitor::visit_class_decl(const CXCursor &cursor) {
    auto class_name = clang_getCursorSpelling(cursor);

    std::list<std::string> namespace_names = collect_namespace_names();
    m_classes.push_back({
                              cursor, clang_getCString(class_name), {}, namespace_names});
    clang_disposeString(class_name);
}

void katsu::ast_visitor::visit_field_decl(const CXCursor &cursor) {
    auto type_spelling = clang_getTypeSpelling(clang_getCursorType(cursor));
    auto var_name = clang_getCursorSpelling(cursor);

    auto semantic_parent = clang_getCursorSemanticParent(cursor);

    if(m_classes.empty() || !clang_equalCursors(m_classes.back().cursor, semantic_parent)) {
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
        visit_class_decl(parent);
    } else if (parent_kind == CXCursor_FieldDecl) {
        visit_field_decl(parent);
    }
}


void katsu::ast_visitor::dispatch_visit(const CXCursor &current, const CXCursor &parent, CXClientData Data) {
    CXCursorKind kind = clang_getCursorKind(current);
    CXCursorKind parent_kind = clang_getCursorKind(parent);
    if (kind == CXCursor_ClassDecl) {
        CXCursor class_namespace = get_class_namespace(current);
        if(clang_getCursorKind(class_namespace) == CXCursor_TranslationUnit) {
            return;
        }
        // Roll back to the previous namespace
        while(!m_namespace_stack.empty() && !clang_equalCursors(class_namespace, m_namespace_stack.back())) {
            exit_namespace_decl(class_namespace);
        }
    }

    if (kind == CXCursor_AnnotateAttr) {
        visit_annotation_decl(current, parent);
    } else if (kind == CXCursor_Namespace) {
        visit_namespace_decl(current);
    }
}

CXCursor katsu::ast_visitor::get_class_namespace(const CXCursor &current) {
    CXCursor class_namespace = clang_getCursorSemanticParent(current);
    CXCursorKind namespace_kind = clang_getCursorKind(class_namespace);
    while (namespace_kind != CXCursor_Namespace && namespace_kind != CXCursor_TranslationUnit)  {
        class_namespace = clang_getCursorSemanticParent(class_namespace);
        namespace_kind = clang_getCursorKind(class_namespace);
    }
    return class_namespace;
}

katsu::ast_visitor katsu::ast_visitor::begin_visit(const CXTranslationUnit& translation_unit) {

    katsu::ast_visitor visitor;
    auto visit_fn = [](CXCursor c, CXCursor p, CXClientData data)
    {
        if (clang_Location_isFromMainFile (clang_getCursorLocation (c)) == 0) {
            return CXChildVisit_Continue;
        }
        katsu::ast_visitor& in_visitor = *static_cast<katsu::ast_visitor*>(data);
        in_visitor.dispatch_visit(c, p, data);
        return CXChildVisit_Recurse;
    };
    CXCursor root_cursor = clang_getTranslationUnitCursor(translation_unit);
    clang_visitChildren(root_cursor,
                        visit_fn, &visitor);
    return visitor;
}

void katsu::ast_visitor::visit_namespace_decl(const CXCursor &namespace_cursor) {
    m_namespace_stack.emplace_back(namespace_cursor);
}

void katsu::ast_visitor::exit_namespace_decl(const CXCursor &namespace_cursor) {
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
