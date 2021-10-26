//
// Created by crax on 10/23/21.
//
#pragma once

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <optional>
#include <charconv>
#include <cassert>


namespace katsu::detail {
    template <typename T> std::optional<T> from_string(const std::string_view &s) {
        assert(false && "This shouldn't be reached");
        return {};
    }
}

namespace katsu {
    class option_parser {
    private:
        bool m_positional_enabled{};
        std::unordered_map<std::string_view, std::list<std::string_view>> m_arguments{};
        std::vector<std::string_view> m_positional_arguments{};

        struct key_value {
            std::string_view key;
            std::string_view value;
        };

        static key_value split_option(std::string_view str) {
            auto index_of_split = str.find('=');
            if(index_of_split == std::string_view::npos) {
                if(str.size() <= 2) {
                    return {str.substr(1), ""};
                }
                // The option is like -Ipath/to/thing
                auto key = str.substr(1, 1);
                auto value = str.substr(2, str.size());
                return {key, value};
            }
            return {str.substr(1, index_of_split - 1), str.substr(index_of_split + 1)};
        }
    private:
        int parse_all_positional(int argc, const char** argv) {
            int i = 1;
            while(i < argc && strcmp(argv[i], "--") != 0) {
                m_positional_arguments.emplace_back(argv[i]);
                i ++;
            }
            return i ++;
        }

        void parse_options(int begin, int argc, const char** argv) {
            for(int i = begin; i < argc; i++) {
                auto [key, value] = split_option(argv[i]);
                m_arguments[key].emplace_back(value);
            }
        }
    public:
        void enable_positional_arguments() {
            m_positional_enabled = true;
        }
        void parse(int argc, const char** argv) {
                int options_begin = 1;
                if(m_positional_enabled) {
                    options_begin = parse_all_positional(argc, argv);
                }
                parse_options(options_begin, argc, argv);
        }

        const std::vector<std::string_view>& get_positional_arguments() const {
            return m_positional_arguments;
        }

        bool is_set(std::string_view key) const {
            return m_arguments.find(key) != m_arguments.end();
        }

        template<typename T>
        std::optional<T> get_argument(std::string_view key) const {
            auto found = m_arguments.find(key);
            if(found != m_arguments.end()) {
                return katsu::detail::from_string<T>(found->second.front());
            }
            return {};
        }

        std::optional<std::list<std::string_view>> get_arguments(std::string_view key) const {
            auto found = m_arguments.find(key);
            if(found != m_arguments.end()) {
                return found->second;
            }
            return {};
        }
    };
}

#ifdef KATSU_DEFINE_IMPLEMENTATION
namespace katsu::detail {
        template <typename T>
        std::optional<T> safe_convert(std::string_view str) {
            T val;
            auto[ptr, err] = std::from_chars(str.data(), str.data() + str.size(), val);
            if(err == std::errc()) {
                return val;
            } else {
                return {};
            }
        }

        template <> std::optional<std::string> from_string<std::string>(const std::string_view &val) {
          return std::string(val);
        }
        template <> std::optional<char> from_string(const std::string_view &val) { return val[0]; }
        template <> std::optional<unsigned char> from_string(const std::string_view &val) { return val[0]; }
        template <> std::optional<short> from_string(const std::string_view &val) {
            return safe_convert<short>(val);
        }
        template <> std::optional<unsigned short> from_string(const std::string_view &val) {
          return safe_convert<unsigned short>(val);
        }
        template <> std::optional<int> from_string(const std::string_view &val) { return safe_convert<int>(val); }
        template <> std::optional<unsigned int> from_string(const std::string_view &val) {
          return safe_convert<unsigned int>(val);
        }

        template <> std::optional<long> from_string(const std::string_view &val) { return safe_convert<long>(val); }
        template <> std::optional<long long> from_string(const std::string_view &val) {
          return safe_convert<long long>(val);
        }
        template <> std::optional<unsigned long> from_string(const std::string_view &val) {
          return safe_convert<unsigned long>(val);
        }
        template <> std::optional<unsigned long long> from_string(const std::string_view &val) {
          return safe_convert<unsigned long long>(val);
        }
        template <> std::optional<bool> from_string(const std::string_view &val) { return val != "false" && val != "0"; }

        template <> std::optional<float> from_string(const std::string_view &val) { return std::stof(std::string(val)); }
        template <> std::optional<double> from_string(const std::string_view &val) {
          return std::stod(std::string(val));
        }
    }
#endif
