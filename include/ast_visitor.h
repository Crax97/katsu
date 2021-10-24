//
// Created by crax on 10/23/21.
//

#pragma once
#include "types.h"
#include <clang-c/Index.h>
#include <list>

namespace katsu {
    class ast_visitor {
    private:
        std::list<class_descriptor> m_classes;
        std::list<CXCursor> m_namespace_stack;

    protected:
        void visit_class_decl(const CXCursor& cursor);
        void visit_field_decl(const CXCursor& cursor);
        void visit_method_decl(const CXCursor& cursor);
        void visit_namespace_decl(const CXCursor& namespace_cursor);
        void exit_namespace_decl(const CXCursor& namespace_cursor);
        void visit_annotation_decl(const CXCursor& cursor, const CXCursor& parent);
    public:
        void dispatch_visit(const CXCursor& current, const CXCursor& parent, CXClientData Data);
        static katsu::ast_visitor begin_visit(const CXTranslationUnit& translation_unit);
        [[nodiscard]] const std::list<class_descriptor>& get_classes() const {
            return m_classes;
        }

        static CXCursor get_class_namespace(const CXCursor &current) ;

        std::list<std::string> collect_namespace_names();
    };
} // namespace katsu
