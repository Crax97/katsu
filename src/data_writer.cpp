//
// Created by crax on 10/24/21.
//

#include "data_writer.h"
#include "ast_visitor.h"

#include <sstream>
#include <fmt/core.h>
#include <fmt/os.h>
#include <set>

void katsu::data_writer::write_to_file(const ast_visitor& visitor, std::ostream &output) const {
    bool did_output_header = false;
    for (auto &klass: visitor.get_classes()) {
        if(!klass.is_reflecting) {
            continue;
        }
        if(!did_output_header) {
            did_output_header = true;
            output << m_data_template.header_template;
        }
        auto klass_string = std::string(m_data_template.class_template);
        std::stringstream m_fields;
        std::stringstream m_methods;
        for (auto &field: klass.fields) {

            std::string line_template = std::string(m_data_template.field_template);
            replace_all(line_template, {
                    {"%FIELDTYPE%", field.type},
                    {"%FIELDNAME%", field.name},
                    {"%CLASSNAME%", klass.class_name},
                    {"%NAMESPACE%", klass.collect_namespace()},
            });
            m_fields << line_template;
        }
        for (auto &method: klass.methods) {

            std::string line_template = std::string(m_data_template.method_template);
            replace_all(line_template, {
                    {"%METHODRETURNTYPE%", method.return_type},
                    {"%METHODNAME%", method.name},
                    {"%CLASSNAME%", klass.class_name},
                    {"%NAMESPACE%", klass.collect_namespace()},
            });
            m_methods << line_template;
        }

        replace_all(klass_string, {
                {"%FIELDS%", m_fields.str()},
                {"%METHODS%", m_methods.str()},
                {"%CLASSNAME%", klass.class_name},
                {"%NAMESPACE%", klass.collect_namespace()},
        });

        output << klass_string;
    }

}
