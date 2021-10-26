//
// Created by crax on 10/23/21.
//

#pragma once

#include <clang-c/Index.h>
#include <string>
#include <list>

struct field_descriptor {
    CXCursor cursor;
    std::string type;
    std::string name;
};

struct class_descriptor {
    CXCursor cursor;
    std::string class_name;
    bool is_reflecting = false;
    std::list<field_descriptor> fields;
    std::list<std::string> namespaces;

    [[nodiscard]] std::string collect_namespace() const {
        std::string out;
        for(auto namesp = namespaces.begin(); namesp != namespaces.end(); namesp) {
            out.append(*namesp);
            if(++namesp != namespaces.end()) {
                out.append("::");
            }
        }
        return out;
    }
};
