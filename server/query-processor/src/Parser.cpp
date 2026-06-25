#include "Parser.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

using namespace std;

string Parser::trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

string Parser::upper(string text) {
    transform(text.begin(), text.end(), text.begin(), [](unsigned char c){ return static_cast<char>(toupper(c)); });
    return text;
}

static string normalizeSql(const string& text) {
    string out;
    bool inQuotes = false;
    char quote = 0;
    bool pendingSpace = false;
    for (char c : text) {
        if ((c == '"' || c == '\'') && (!inQuotes || c == quote)) {
            if (pendingSpace && !out.empty()) out += ' ';
            pendingSpace = false;
            inQuotes = !inQuotes;
            quote = inQuotes ? c : 0;
            out += c;
        } else if (!inQuotes && isspace(static_cast<unsigned char>(c))) {
            pendingSpace = true;
        } else {
            if (pendingSpace && !out.empty()) out += ' ';
            pendingSpace = false;
            out += c;
        }
    }
    return Parser::trim(out);
}

vector<string> Parser::splitStatements(const string& script) {
    vector<string> statements;
    string current;
    bool inQuotes = false;
    char quote = 0;
    for (size_t i = 0; i < script.size(); ++i) {
        char c = script[i];
        if ((c == '"' || c == '\'') && (!inQuotes || c == quote)) {
            inQuotes = !inQuotes;
            quote = inQuotes ? c : 0;
        }
        if (!inQuotes && c == '/' && i + 1 < script.size() && script[i + 1] == '/') {
            while (i < script.size() && script[i] != '\n') ++i;
            current += ' ';
            continue;
        }
        if (c == ';' && !inQuotes) {
            string s = trim(current);
            if (!s.empty()) statements.push_back(s);
            current.clear();
        } else current += c;
    }
    string s = trim(current);
    if (!s.empty()) statements.push_back(s);
    return statements;
}

vector<string> Parser::splitCsv(const string& text) {
    vector<string> items;
    string current;
    bool inQuotes = false;
    char quote = 0;
    int parentheses = 0;
    for (char c : text) {
        if ((c == '"' || c == '\'') && (!inQuotes || c == quote)) {
            inQuotes = !inQuotes;
            quote = inQuotes ? c : 0;
        }
        if (!inQuotes && c == '(') ++parentheses;
        if (!inQuotes && c == ')') --parentheses;
        if (c == ',' && !inQuotes && parentheses == 0) {
            items.push_back(trim(current)); current.clear();
        } else current += c;
    }
    if (!trim(current).empty()) items.push_back(trim(current));
    return items;
}

string Parser::unquote(const string& text) {
    string t = trim(text);
    if (t.size() >= 2 && ((t.front() == '"' && t.back() == '"') || (t.front() == '\'' && t.back() == '\'')))
        return t.substr(1, t.size() - 2);
    return t;
}

string Parser::parseNameAfter(const string& statement, const string& keyword) {
    regex rgx(keyword + R"(\s+([A-Za-z_][A-Za-z0-9_]*))", regex_constants::icase);
    smatch match;
    string sql = normalizeSql(statement);
    if (!regex_search(sql, match, rgx)) throw runtime_error("Expected name after " + keyword);
    return match[1];
}

string Parser::parseCreateTableName(const string& statement) { return parseNameAfter(statement, "CREATE\\s+TABLE"); }
string Parser::parseDropTableName(const string& statement) { return parseNameAfter(statement, "DROP\\s+TABLE"); }

vector<ColumnDefinition> Parser::parseCreateTableColumns(const string& statement) {
    string sql = normalizeSql(statement);
    size_t open = sql.find('('), close = sql.rfind(')');
    if (open == string::npos || close == string::npos || close <= open)
        throw runtime_error("CREATE TABLE must contain column definitions in parentheses");
    vector<string> rawColumns = splitCsv(sql.substr(open + 1, close - open - 1));
    vector<ColumnDefinition> columns;
    regex colRgx(R"(^\s*([A-Za-z_][A-Za-z0-9_]*)\s+(INTEGER|DOUBLE|DATETIME|VARCHAR\s*\(\s*(\d+)\s*\))(.*)$)", regex_constants::icase);
    for (const auto& raw : rawColumns) {
        smatch match;
        if (!regex_match(raw, match, colRgx)) throw runtime_error("Invalid column definition: " + raw);
        ColumnDefinition col;
        col.name = match[1];
        col.type = upper(regex_replace(string(match[2]), regex(R"(\s+)"), ""));
        if (match[3].matched) col.varcharLength = stoi(match[3]);
        col.nullable = upper(match[4]).find("NOT NULL") == string::npos;
        columns.push_back(col);
    }
    return columns;
}

string Parser::parseInsertTableName(const string& statement) { return parseNameAfter(statement, "INSERT\\s+INTO"); }

vector<string> Parser::parseInsertValues(const string& statement) {
    string sql = normalizeSql(statement);
    smatch match;
    regex valuesRgx(R"(\bVALUES\s*\((.*)\)\s*$)", regex_constants::icase);
    string body;
    if (regex_search(sql, match, valuesRgx)) body = match[1];
    else {
        size_t open = sql.find('('), close = sql.rfind(')');
        if (open == string::npos || close == string::npos || close <= open)
            throw runtime_error("INSERT must contain values in parentheses");
        body = sql.substr(open + 1, close - open - 1);
    }
    auto values = splitCsv(body);
    for (auto& value : values) value = unquote(value);
    return values;
}

WhereClause Parser::parseWhere(const string& text) {
    WhereClause where;
    string sql = normalizeSql(text);
    smatch match;
    regex whereRgx(R"(\bWHERE\s+([A-Za-z_][A-Za-z0-9_]*)\s*(==|=|>|<|LIKE|NOT)\s*(.*?)(?:\s+ORDER\s+BY\b|$))", regex_constants::icase);
    if (regex_search(sql, match, whereRgx)) {
        where.enabled = true;
        where.column = match[1];
        where.op = upper(match[2]);
        if (where.op == "==") where.op = "=";
        where.value = unquote(trim(match[3]));
        if (where.value.empty()) throw runtime_error("WHERE condition requires a value");
    } else if (upper(sql).find("WHERE") != string::npos) {
        throw runtime_error("Invalid WHERE syntax. Use WHERE <column> <operator> <value>");
    }
    return where;
}

ParsedSelect Parser::parseSelect(const string& statement) {
    string sql = normalizeSql(statement);
    smatch match;
    regex selectRgx(R"(^SELECT\s+(.+?)\s+FROM\s+([A-Za-z_][A-Za-z0-9_]*)(.*)$)", regex_constants::icase);
    if (!regex_match(sql, match, selectRgx))
        throw runtime_error("Invalid SELECT syntax. Use SELECT <columns> FROM <table> [WHERE ...] [ORDER BY ...]");
    ParsedSelect result;
    result.columns = splitCsv(trim(match[1]));
    result.tableName = match[2];
    string tail = match[3];
    result.where = parseWhere(tail);
    smatch orderMatch;
    regex orderRgx(R"(\bORDER\s+BY\s+([A-Za-z_][A-Za-z0-9_]*)(?:\s+(ASC|DESC))?\s*$)", regex_constants::icase);
    if (regex_search(tail, orderMatch, orderRgx)) {
        result.orderByColumn = orderMatch[1];
        result.orderDescending = orderMatch[2].matched && upper(orderMatch[2]) == "DESC";
    } else if (upper(tail).find("ORDER") != string::npos) {
        throw runtime_error("Invalid ORDER BY syntax. Use ORDER BY <column> ASC|DESC");
    }
    return result;
}

ParsedUpdate Parser::parseUpdate(const string& statement) {
    string sql = normalizeSql(statement);
    smatch match;
    regex updateRgx(R"(^UPDATE\s+([A-Za-z_][A-Za-z0-9_]*)\s+SET\s+([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.*?)(?:\s+WHERE\s+.*|$))", regex_constants::icase);
    if (!regex_match(sql, match, updateRgx))
        throw runtime_error("Invalid UPDATE syntax. Use UPDATE <table> SET <column> = <value> [WHERE ...]");
    ParsedUpdate update;
    update.tableName = match[1]; update.column = match[2]; update.value = unquote(trim(match[3]));
    update.where = parseWhere(sql);
    return update;
}

string Parser::parseIndexName(const string& statement) { return parseNameAfter(statement, "CREATE\\s+INDEX"); }
string Parser::parseIndexTable(const string& statement) {
    smatch m; string sql = normalizeSql(statement);
    if (!regex_search(sql, m, regex(R"(\bON\s+([A-Za-z_][A-Za-z0-9_]*)\s*\()", regex_constants::icase)))
        throw runtime_error("Invalid CREATE INDEX table syntax");
    return m[1];
}
string Parser::parseIndexColumn(const string& statement) {
    smatch m; string sql = normalizeSql(statement);
    if (!regex_search(sql, m, regex(R"(\bON\s+[A-Za-z_][A-Za-z0-9_]*\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\))", regex_constants::icase)))
        throw runtime_error("Invalid CREATE INDEX column syntax");
    return m[1];
}
string Parser::parseIndexType(const string& statement) {
    smatch m; string sql = normalizeSql(statement);
    if (!regex_search(sql, m, regex(R"(\bOF\s+TYPE\s+(BTREE|BST)\s*$)", regex_constants::icase)))
        throw runtime_error("Invalid CREATE INDEX type. Use BTREE or BST");
    return upper(m[1]);
}
