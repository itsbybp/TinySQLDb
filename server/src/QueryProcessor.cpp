#include "QueryProcessor.h"
#include "Parser.h"
#include "StoredDataManager.h"
#include <chrono>
#include <stdexcept>
using namespace std;
QueryProcessor::QueryProcessor(StoredDataManager&s):storage_(s){}
nlohmann::json QueryProcessor::executeScript(const string&script,QueryContext&context){
    auto allStart=chrono::steady_clock::now();
    nlohmann::json response; response["ok"]=true; response["results"]=nlohmann::json::array();
    auto statements=Parser::splitStatements(script);
    for(size_t i=0;i<statements.size();){
        string normalized=Parser::upper(Parser::trim(statements[i]));
        if(normalized.rfind("INSERT INTO",0)==0){
            string table;
            try{table=Parser::parseInsertTableName(statements[i]);}catch(...){table="";}
            size_t j=i;
            vector<vector<string>> values;
            while(j<statements.size() && Parser::upper(Parser::trim(statements[j])).rfind("INSERT INTO",0)==0){
                try{ if(Parser::parseInsertTableName(statements[j])!=table) break; values.push_back(Parser::parseInsertValues(statements[j])); }
                catch(...){break;}
                ++j;
            }
            if(values.size()>=100){
                nlohmann::json result; result["statement"]="BATCH INSERT INTO "+table+" ("+to_string(values.size())+" statements)";
                auto st=chrono::steady_clock::now();
                try{ if(context.database.empty()) throw runtime_error("No database context. Use SET DATABASE first"); storage_.insertRows(context.database,table,values); result["success"]=true; result["message"]=to_string(values.size())+" rows inserted into "+table; }
                catch(const exception&ex){result["success"]=false;result["error"]=ex.what();response["ok"]=false;}
                result["elapsedMs"]=chrono::duration<double,milli>(chrono::steady_clock::now()-st).count(); response["results"].push_back(result); i=j; continue;
            }
        }
        nlohmann::json result; result["statement"]=Parser::trim(statements[i]); auto st=chrono::steady_clock::now();
        try{result.update(executeStatement(statements[i],context));result["success"]=true;}catch(const exception&ex){result["success"]=false;result["error"]=ex.what();response["ok"]=false;}
        result["elapsedMs"]=chrono::duration<double,milli>(chrono::steady_clock::now()-st).count(); response["results"].push_back(result); ++i;
    }
    response["database"]=context.database; response["elapsedMs"]=chrono::duration<double,milli>(chrono::steady_clock::now()-allStart).count(); return response;
}
static void requireDb(const QueryContext&c){if(c.database.empty())throw runtime_error("No database context. Use SET DATABASE <name> first or enter a database in the client context field");}
nlohmann::json QueryProcessor::executeStatement(const string&st,QueryContext&ctx){
    string n=Parser::upper(Parser::trim(st));
    nlohmann::json r;
    if(n.rfind("CREATE DATABASE",0)==0){
        string d=Parser::parseNameAfter(st,"CREATE\\s+DATABASE");
        storage_.createDatabase(d);
        r["message"]="Database created: "+d;
        return r;}
    if(n.rfind("SET DATABASE",0)==0){
        string d=Parser::parseNameAfter(st,"SET\\s+DATABASE");
        if(!storage_.databaseExists(d))throw runtime_error("Database does not exist: "+d);
        ctx.database=d;
        r["message"]="Database context set to: "+d;
        return r;}
    if(n.rfind("SELECT",0)==0){
        auto q=Parser::parseSelect(st);
        if(Parser::upper(q.tableName).rfind("SYSTEM",0)!=0)requireDb(ctx);
        return storage_.selectRows(ctx.database,q);}
    if(n.rfind("CREATE TABLE",0)==0){
        requireDb(ctx);string t=Parser::parseCreateTableName(st);
        storage_.createTable(ctx.database,t,Parser::parseCreateTableColumns(st));
        r["message"]="Table created: "+t;
        return r;}
    if(n.rfind("DROP TABLE",0)==0){
        requireDb(ctx);
        string t=Parser::parseDropTableName(st);
        storage_.dropTable(ctx.database,t);
        r["message"]="Table dropped: "+t;
        return r;}
    if(n.rfind("INSERT INTO",0)==0){
        requireDb(ctx);
        string t=Parser::parseInsertTableName(st);
        storage_.insertRow(ctx.database,t,Parser::parseInsertValues(st));
        r["message"]="1 row inserted into "+t;
        return r;}
    if(n.rfind("CREATE INDEX",0)==0){
        requireDb(ctx);
        storage_.createIndex(ctx.database,Parser::parseIndexName(st),
            Parser::parseIndexTable(st),
            Parser::parseIndexColumn(st),
            Parser::parseIndexType(st));
        r["message"]="Index created";
        return r;}
    if(n.rfind("DELETE",0)==0){
        requireDb(ctx);
        string t=Parser::parseNameAfter(st,"FROM");
        int x=storage_.deleteRows(ctx.database,t,Parser::parseWhere(st));
        r["message"]=to_string(x)+" rows deleted";
        return r;}
    if(n.rfind("UPDATE",0)==0){
        requireDb(ctx);
        int x=storage_.updateRows(ctx.database,Parser::parseUpdate(st));
        r["message"]=to_string(x)+" rows updated";
        return r;}
    throw runtime_error("Unsupported SQL statement: "+Parser::trim(st));}
