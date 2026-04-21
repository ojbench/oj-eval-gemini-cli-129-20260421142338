#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Variable {
    int type; // 0 for int, 1 for string
    long long int_val;
    string str_val;
};

unordered_map<string, vector<pair<int, Variable>>> all_vars;
vector<string> scope_vars[110];
int current_depth = 0;

void print_invalid() {
    cout << "Invalid operation" << "\n";
}

bool parse_int(const string& s, long long& val) {
    if (s.empty()) return false;
    int start = 0;
    if (s[0] == '-') {
        start = 1;
        if (s.length() == 1) return false;
    } else if (s[0] == '+') {
        start = 1;
        if (s.length() == 1) return false;
    }
    val = 0;
    for (int i = start; i < (int)s.length(); ++i) {
        if (s[i] < '0' || s[i] > '9') return false;
        val = val * 10 + (s[i] - '0');
    }
    if (s[0] == '-') val = -val;
    return true;
}

string trim(const string& s) {
    size_t first = s.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(' ');
    return s.substr(first, last - first + 1);
}

size_t skip_spaces(const string& s, size_t pos) {
    if (pos == string::npos) return string::npos;
    return s.find_first_not_of(' ', pos);
}

size_t find_space(const string& s, size_t pos) {
    if (pos == string::npos) return string::npos;
    return s.find(' ', pos);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;
    string line;
    getline(cin, line); // consume newline

    all_vars.reserve(1000000);

    for (int i = 0; i < n; ++i) {
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        size_t p_start = skip_spaces(line, 0);
        if (p_start == string::npos) continue;
        size_t p_end = find_space(line, p_start);
        string cmd = line.substr(p_start, p_end - p_start);

        if (cmd == "Indent") {
            current_depth++;
        } else if (cmd == "Dedent") {
            if (current_depth > 0) {
                for (const string& name : scope_vars[current_depth]) {
                    all_vars[name].pop_back();
                }
                scope_vars[current_depth].clear();
                current_depth--;
            } else {
                print_invalid();
            }
        } else if (cmd == "Declare") {
            size_t p1_start = skip_spaces(line, p_end);
            if (p1_start == string::npos) { print_invalid(); continue; }
            size_t p1_end = find_space(line, p1_start);
            string type_str = line.substr(p1_start, p1_end - p1_start);

            size_t p2_start = skip_spaces(line, p1_end);
            if (p2_start == string::npos) { print_invalid(); continue; }
            size_t p2_end = find_space(line, p2_start);
            string name = line.substr(p2_start, p2_end - p2_start);

            size_t p3_start = skip_spaces(line, p2_end);
            if (p3_start == string::npos) { print_invalid(); continue; }
            string value_str = trim(line.substr(p3_start));

            auto& vec = all_vars[name];
            if (!vec.empty() && vec.back().first == current_depth) {
                print_invalid();
                continue;
            }

            if (type_str == "int") {
                long long val;
                if (parse_int(value_str, val)) {
                    vec.push_back({current_depth, {0, val, ""}});
                    scope_vars[current_depth].push_back(name);
                } else {
                    print_invalid();
                }
            } else if (type_str == "string") {
                if (value_str.length() >= 2 && value_str.front() == '"' && value_str.back() == '"') {
                    string val = value_str.substr(1, value_str.length() - 2);
                    vec.push_back({current_depth, {1, 0, val}});
                    scope_vars[current_depth].push_back(name);
                } else {
                    print_invalid();
                }
            } else {
                print_invalid();
            }
        } else if (cmd == "Add") {
            size_t p1_start = skip_spaces(line, p_end);
            if (p1_start == string::npos) { print_invalid(); continue; }
            size_t p1_end = find_space(line, p1_start);
            string res_name = line.substr(p1_start, p1_end - p1_start);

            size_t p2_start = skip_spaces(line, p1_end);
            if (p2_start == string::npos) { print_invalid(); continue; }
            size_t p2_end = find_space(line, p2_start);
            string v1_name = line.substr(p2_start, p2_end - p2_start);

            size_t p3_start = skip_spaces(line, p2_end);
            if (p3_start == string::npos) { print_invalid(); continue; }
            string v2_name = trim(line.substr(p3_start));

            auto it_res = all_vars.find(res_name);
            auto it_v1 = all_vars.find(v1_name);
            auto it_v2 = all_vars.find(v2_name);

            if (it_res == all_vars.end() || it_res->second.empty() ||
                it_v1 == all_vars.end() || it_v1->second.empty() ||
                it_v2 == all_vars.end() || it_v2->second.empty()) {
                print_invalid();
                continue;
            }

            Variable& res = it_res->second.back().second;
            Variable& v1 = it_v1->second.back().second;
            Variable& v2 = it_v2->second.back().second;

            if (res.type == v1.type && res.type == v2.type) {
                if (res.type == 0) {
                    res.int_val = v1.int_val + v2.int_val;
                } else {
                    res.str_val = v1.str_val + v2.str_val;
                }
            } else {
                print_invalid();
            }
        } else if (cmd == "SelfAdd") {
            size_t p1_start = skip_spaces(line, p_end);
            if (p1_start == string::npos) { print_invalid(); continue; }
            size_t p1_end = find_space(line, p1_start);
            string name = line.substr(p1_start, p1_end - p1_start);

            size_t p2_start = skip_spaces(line, p1_end);
            if (p2_start == string::npos) { print_invalid(); continue; }
            string value_str = trim(line.substr(p2_start));

            auto it = all_vars.find(name);
            if (it == all_vars.end() || it->second.empty()) {
                print_invalid();
                continue;
            }

            Variable& var = it->second.back().second;
            if (var.type == 0) {
                long long val;
                if (parse_int(value_str, val)) {
                    var.int_val += val;
                } else {
                    print_invalid();
                }
            } else {
                if (value_str.length() >= 2 && value_str.front() == '"' && value_str.back() == '"') {
                    string val = value_str.substr(1, value_str.length() - 2);
                    var.str_val += val;
                } else {
                    print_invalid();
                }
            }
        } else if (cmd == "Print") {
            size_t p1_start = skip_spaces(line, p_end);
            if (p1_start == string::npos) { print_invalid(); continue; }
            string name = trim(line.substr(p1_start));

            auto it = all_vars.find(name);
            if (it == all_vars.end() || it->second.empty()) {
                print_invalid();
            } else {
                Variable& var = it->second.back().second;
                cout << name << ":";
                if (var.type == 0) cout << var.int_val;
                else cout << var.str_val;
                cout << "\n";
            }
        }
    }

    return 0;
}
