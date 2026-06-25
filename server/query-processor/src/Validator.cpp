#include "Validator.h"
#include "TableFile.h"
#include <ctime>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
using namespace std;
static bool validIdentifier(const string&v){return regex_match(v,regex(R"([A-Za-z_][A-Za-z0-9_]*)"));}
void Validator::validateDatabaseName(const string&n){if(!validIdentifier(n))throw runtime_error("Invalid database name: "+n);}void Validator::validateTableName(const string&n){if(!validIdentifier(n))throw runtime_error("Invalid table name: "+n);}
void Validator::validateColumnDefinitions(const vector<ColumnDefinition>&cs){if(cs.empty())throw runtime_error("A table must have at least one column");unordered_set<string>names;for(auto&c:cs){if(!validIdentifier(c.name))throw runtime_error("Invalid column name: "+c.name);if(!names.insert(c.name).second)throw runtime_error("Duplicate column name: "+c.name);string t=Parser::upper(c.type);if(t!="INTEGER"&&t!="DOUBLE"&&t!="DATETIME"&&t.rfind("VARCHAR",0)!=0)throw runtime_error("Invalid column type for "+c.name+": "+c.type);if(t.rfind("VARCHAR",0)==0&&c.varcharLength<=0)throw runtime_error("VARCHAR length must be greater than zero for "+c.name);}}
static bool validDateTime(const string&v){tm tm{};istringstream ss(v);ss>>get_time(&tm,"%Y-%m-%d %H:%M:%S");if(ss.fail()||!ss.eof())return false;int y=tm.tm_year+1900,m=tm.tm_mon+1,d=tm.tm_mday,h=tm.tm_hour,mi=tm.tm_min,se=tm.tm_sec;if(m<1||m>12||h>23||mi>59||se>59)return false;int days[]={31,28,31,30,31,30,31,31,30,31,30,31};bool leap=(y%400==0)||(y%4==0&&y%100!=0);if(leap)days[1]=29;return d>=1&&d<=days[m-1];}
void Validator::validateValueForType(const string&type,const string&v){string t=Parser::upper(type);if(t=="INTEGER"){if(!regex_match(v,regex(R"([-+]?\d+)")))throw runtime_error("Expected INTEGER, received: "+v);}else if(t=="DOUBLE"){if(!regex_match(v,regex(R"([-+]?(?:\d+\.?\d*|\.\d+))")))throw runtime_error("Expected DOUBLE, received: "+v);}else if(t=="DATETIME"){if(!validDateTime(v))throw runtime_error("Invalid DATETIME '"+v+"'. Expected a real date in YYYY-MM-DD HH:MM:SS format");}else if(t.rfind("VARCHAR",0)==0){smatch m;if(regex_match(t,m,regex(R"(VARCHAR\((\d+)\))",regex_constants::icase))){int n=stoi(m[1]);if((int)v.size()>n)throw runtime_error("VARCHAR("+to_string(n)+") value is too long ("+to_string(v.size())+" characters): "+v);}}}
void Validator::validateInsertValues(const TableSchema&s,const vector<string>&v){if(s.columns.size()!=v.size())throw runtime_error("INSERT value count does not match column count. Expected "+to_string(s.columns.size())+", received "+to_string(v.size()));for(size_t i=0;i<v.size();++i){try{validateValueForType(s.columns[i].type,v[i]);}catch(const exception&e){throw runtime_error("Invalid value for column '"+s.columns[i].name+"': "+e.what());}}}
void Validator::validateColumnExists(const TableSchema&s,const string&c){for(auto&x:s.columns)if(x.name==c)return;throw runtime_error("Column does not exist: "+c);}
