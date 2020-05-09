#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <memory>


namespace Json {

    class Node : std::variant<std::vector<Node>,
            std::map<std::string, Node>,
            long long,
            double,
            bool,
            std::nullptr_t,
            std::string> {
    public:

        template <typename T>
        bool holds() const {
            return std::holds_alternative<T>(*this);
        }

        Node(int num) : Node(static_cast<long long>(num)) {}

        using variant::variant;

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        double AsDouble() const {
            double result;
            try {
                result = std::get<double>(*this);
            } catch (...) {
                result =  std::get<long long>(*this);
            }
            return result;
        }
        bool AsBool() const {
            return std::get<bool>(*this);
        }

        long long AsLong() const {
            return std::get<long long>(*this);
        }

        int AsInt() const {
            return std::get<long long>(*this);
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }

        bool IsNull() const {
            return holds<std::nullptr_t>();
        }

        std::nullptr_t AsNull() const {
            return std::get<std::nullptr_t>(*this);
        }

    private:

    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& os);

}