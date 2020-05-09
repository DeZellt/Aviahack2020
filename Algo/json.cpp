#include "json.h"

using namespace std;

namespace Json {

    struct Indenter;
    void PrintNode(const Node& node, std::ostream& os, Indenter& indenter);


    Document::Document(Node root) : root(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root;
    }

    Node LoadNode(istream& input);

    Node LoadArray(istream& input) {
        //std::cout << "array\n";
        vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNumber(istream& input) {
        //std::cout << "number\n";
        string number;
        bool is_double = false;
        bool error = false;
        bool first_digit = true;
        while (isdigit(input.peek()) || input.peek() == '.' || (first_digit && input.peek() == '-')) {
            first_digit = false;
            if (input.peek() == '.') {
                if (!is_double) {
                    is_double = true;
                } else {
                     error = true;
                }
            }
            number.push_back(input.get());
        }
        if (error) {
            throw std::logic_error("Cant load number, token is " + number);
        }

        if (is_double) {
            return Node(std::stod(number));
        } else {
            return Node(std::stoll(number));
        }

    }

//    Node LoadInt(istream& input) {
//        int result = 0;
//        while (isdigit(input.peek())) {
//            result *= 10;
//            result += input.get() - '0';
//        }
//        return Node(result);
//    }

    Node LoadBool(istream& input) {
        //std::cout << "bool\n";
        string bool_literal;
        while (isalpha(input.peek())) {
            bool_literal.push_back(input.get());
        }
        if (bool_literal == "true") {
            return Node(true);
        } else if (bool_literal == "false") {
            return Node(false);
        } else {
            throw std::logic_error("Cant load bool, token is " + bool_literal);
        }
    }

    Node LoadNull(istream& input) {
        string null_str;
        while (isalpha(input.peek())) {
            null_str.push_back(input.get());
        }
        if (null_str == "null") {
            return Node(nullptr);
        } else {
            throw std::logic_error("Cant load null, token is " + null_str);
        }
    }

    Node LoadString(istream& input) {
        //std::cout << "string\n";
        string line;
        getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(istream& input) {
        //std::cout << "dict\n";
        map<string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(istream& input) {
        //std::cout << "node\n";
        char c;
        input >> c;
        //std::cout << "node\n";
        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (c == 'n') {
            input.putback(c);
            return LoadNull(input);
        } else if (c == 't' || c == 'f') {
            input.putback(c);
            return LoadBool(input);
        } else {
            input.putback(c);
            return LoadNumber(input);
        }
    }

    Document Load(istream& input) {
        //std::cout << "load\n";
        return Document{LoadNode(input)};
    }

    struct Indenter {

        void Indent(ostream& os) {
            if (no_indent) {
                no_indent = false;
            } else {
                os << string(indent_size, ' ');
            }

        }

        void Increment() {
            indent_size += 2;
        }

        void Decrement() {
            indent_size -= 2;
        }

        void NoIndentNext() {
            no_indent = true;
        }

    private:
        bool no_indent = false;
        int indent_size = 0;
    };



    void PrintAsMap(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        if (node.AsMap().empty()) {
            os << "{}";
            return;
        }
        os << "{\n";
        indenter.Increment();
        for (auto it = node.AsMap().begin(); it != node.AsMap().end(); ++it) {
            indenter.Indent(os);
            indenter.NoIndentNext();
            os << "\"" << it->first << "\": ";
            PrintNode(it->second, os, indenter);
            if (it != prev(node.AsMap().end())) {
                os << ",";
            }
            os << "\n";
        }
        indenter.Decrement();
        indenter.Indent(os);
        os << "}";
    }

    void PrintAsDouble(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        os << node.AsDouble();
    }

    void PrintAsNumber(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        os << node.AsLong();
    }

    void PrintAsString(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        os << "\"" << node.AsString() << "\"";
    }

    void PrintAsVector(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        if (node.AsArray().empty()) {
            os << "[]";
            return;
        }
        os << "[\n";
        indenter.Increment();
        for (auto it = node.AsArray().begin(); it != node.AsArray().end(); ++it) {
            PrintNode(*it, os, indenter);
            if (it != prev(node.AsArray().end())) {
                os << ",";
            }
            os << "\n";

        }
        indenter.Decrement();
        indenter.Indent(os);
        os << "]";
    }

    void PrintAsNull(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        if (node.AsNull() == nullptr) {
            os << "null";
        } else {
            throw std::logic_error("PrintNull");
        }

    }

    void PrintAsBool(const Node& node, std::ostream& os, Indenter& indenter) {
        indenter.Indent(os);
        if (node.AsBool()) {
            os << "true";
        } else {
            os << "false";
        }
    }

    void PrintNode(const Node& node, std::ostream& os, Indenter& indenter) {
        if (node.holds<string>()) {
            PrintAsString(node, os, indenter);
        } else if (node.holds<vector<Node>>()) {
            PrintAsVector(node, os, indenter);
        } else if (node.holds<map<string, Node>>()) {
            PrintAsMap(node, os, indenter);
        } else if (node.holds<double>()) {
            PrintAsDouble(node, os, indenter);
        } else if (node.holds<long long>()) {
            PrintAsNumber(node, os, indenter);
        } else if (node.holds<bool>()) {
            PrintAsBool(node, os, indenter);
        } else if (node.holds<std::nullptr_t>()) {
            PrintAsNull(node, os, indenter);
        }

    }

    void Print(const Document& doc, std::ostream& os) {
        Indenter indenter;
        PrintNode(doc.GetRoot(), os, indenter);
    }

}