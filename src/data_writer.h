//
// Created by crax on 10/24/21.
//

#pragma once

#include "types.h"

#include <utility>
#include <vector>
#include <istream>
#include <set>
#include <string>

inline std::string replace_all(std::string& str, const std::set<std::pair<std::string, std::string>>& pairs){
    for(auto& pair : pairs) {
        auto found = str.find(pair.first);
        while(found != std::string::npos) {
            str.replace(found, pair.first.size(), pair.second);
            found = str.find(pair.first, found);
        }
    }
    return str;
}

namespace katsu {

    struct data_writer_templates {
        std::string class_template;
        std::string field_template;
        std::string header_template;
        std::string method_template;
    };

    class data_writer {
        data_writer_templates m_data_template;
    public:
        explicit data_writer(data_writer_templates data_template)
            : m_data_template(std::move(data_template)) {}

        void write_to_file(const class ast_visitor& visitor, std::ostream& output) const;
    };
}

