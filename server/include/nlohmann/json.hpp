#pragma once
#include <cctype>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace nlohmann {
class json {
public:
    using object_t = std::map<std::string,json>;
    using array_t = std::vector<json>;
    using value_t = std::variant<std::nullptr_t,bool,double,std::string,object_t,array_t>;
private:
    value_t v_ = nullptr;
    struct Parser {
        const std::string& s; size_t i=0;
        void ws(){ while(i<s.size() && std::isspace((unsigned char)s[i])) ++i; }
        [[noreturn]] void err(const std::string&m){ throw std::runtime_error("Invalid JSON: "+m+" at position "+std::to_string(i)); }
        std::string str(){ if(i>=s.size()||s[i]!='\"') err("expected string"); ++i; std::string out; while(i<s.size()){ char c=s[i++]; if(c=='\"') return out; if(c=='\\'){ if(i>=s.size()) err("unfinished escape"); char e=s[i++]; switch(e){case '\"':out+='\"';break;case '\\':out+='\\';break;case '/':out+='/';break;case 'b':out+='\b';break;case 'f':out+='\f';break;case 'n':out+='\n';break;case 'r':out+='\r';break;case 't':out+='\t';break;default:err("unsupported escape");}} else out+=c;} err("unterminated string"); }
        json val(){ ws(); if(i>=s.size()) err("unexpected end"); char c=s[i]; if(c=='{') return obj(); if(c=='[') return arr(); if(c=='\"') return json(str()); if(s.compare(i,4,"true")==0){i+=4;return json(true);} if(s.compare(i,5,"false")==0){i+=5;return json(false);} if(s.compare(i,4,"null")==0){i+=4;return json(nullptr);} size_t st=i; if(c=='-'||std::isdigit((unsigned char)c)){++i; while(i<s.size()&&(std::isdigit((unsigned char)s[i])||s[i]=='.'||s[i]=='e'||s[i]=='E'||s[i]=='+'||s[i]=='-'))++i; try{return json(std::stod(s.substr(st,i-st)));}catch(...){err("invalid number");}} err("unexpected token"); }
        json obj(){ ++i; object_t o; ws(); if(i<s.size()&&s[i]=='}'){++i;return json(o);} while(true){ws(); std::string k=str(); ws(); if(i>=s.size()||s[i++]!=':') err("expected ':'"); o[k]=val(); ws(); if(i>=s.size())err("unexpected end"); if(s[i]=='}'){++i;break;} if(s[i++]!=',')err("expected ','"); } return json(o); }
        json arr(){ ++i; array_t a; ws(); if(i<s.size()&&s[i]==']'){++i;return json(a);} while(true){a.push_back(val()); ws(); if(i>=s.size())err("unexpected end"); if(s[i]==']'){++i;break;} if(s[i++]!=',')err("expected ','"); } return json(a); }
    };
    static std::string esc(const std::string&s){ std::ostringstream o; for(char c:s){ switch(c){case '\"':o<<"\\\"";break;case '\\':o<<"\\\\";break;case '\n':o<<"\\n";break;case '\r':o<<"\\r";break;case '\t':o<<"\\t";break;default:o<<c;}} return o.str(); }
    void dumpTo(std::ostringstream&o,int indent,int depth) const { auto pad=[&](int d){ if(indent>0)o<<std::string(d*indent,' ');}; if(std::holds_alternative<std::nullptr_t>(v_))o<<"null"; else if(auto p=std::get_if<bool>(&v_))o<<(*p?"true":"false"); else if(auto p=std::get_if<double>(&v_)){ if(*p==(long long)*p)o<<(long long)*p; else o<<std::setprecision(15)<<*p;} else if(auto p=std::get_if<std::string>(&v_))o<<'\"'<<esc(*p)<<'\"'; else if(auto p=std::get_if<object_t>(&v_)){o<<'{'; bool first=true; for(auto&[k,val]:*p){if(!first)o<<','; if(indent>0)o<<'\n'; pad(depth+1); o<<'\"'<<esc(k)<<"\":"<<(indent>0?" ":""); val.dumpTo(o,indent,depth+1); first=false;} if(!p->empty()&&indent>0){o<<'\n';pad(depth);}o<<'}';} else {auto& arr=std::get<array_t>(v_);o<<'['; for(size_t i=0;i<arr.size();++i){if(i)o<<',';if(indent>0){o<<'\n';pad(depth+1);}arr[i].dumpTo(o,indent,depth+1);}if(!arr.empty()&&indent>0){o<<'\n';pad(depth);}o<<']';} }
public:
    json()=default; json(std::nullptr_t):v_(nullptr){} json(bool b):v_(b){} json(int n):v_((double)n){} json(long n):v_((double)n){} json(double d):v_(d){} json(const char*s):v_(std::string(s)){} json(std::string s):v_(std::move(s)){} json(object_t o):v_(std::move(o)){} json(array_t a):v_(std::move(a)){}
    template<class T> json(const std::vector<T>& xs):v_(array_t{}){auto& a=std::get<array_t>(v_); for(const auto& x:xs)a.emplace_back(json(x));}
    json(std::initializer_list<std::pair<const std::string,json>> init):v_(object_t{}){auto&o=std::get<object_t>(v_);for(auto&p:init)o[p.first]=p.second;}
    static json array(){return json(array_t{});} static json parse(const std::string&s){Parser p{s};json j=p.val();p.ws();if(p.i!=s.size())throw std::runtime_error("Invalid JSON: trailing data");return j;}
    bool is_string()const{return std::holds_alternative<std::string>(v_);} bool is_object()const{return std::holds_alternative<object_t>(v_);} bool is_array()const{return std::holds_alternative<array_t>(v_);} bool contains(const std::string&k)const{auto p=std::get_if<object_t>(&v_);return p&&p->count(k);} 
    json& operator[](const std::string&k){if(!is_object())v_=object_t{};return std::get<object_t>(v_)[k];} const json& operator[](const std::string&k)const{static json nil;auto p=std::get_if<object_t>(&v_);if(!p)return nil;auto it=p->find(k);return it==p->end()?nil:it->second;}
    void push_back(const json&j){if(!is_array())v_=array_t{};std::get<array_t>(v_).push_back(j);} void update(const json&j){if(!j.is_object())return;if(!is_object())v_=object_t{};for(auto&[k,v]:std::get<object_t>(j.v_))std::get<object_t>(v_)[k]=v;}
    template<class T>T get()const; template<class T>T value(const std::string&k,const T&def)const{if(!contains(k))return def;try{return (*this)[k].get<T>();}catch(...){return def;}}
    std::string value(const std::string& k, const char* def) const { return value<std::string>(k, std::string(def)); }
    std::string dump(int indent=-1)const{std::ostringstream o;dumpTo(o,indent,0);return o.str();}
};
template<> inline std::string json::get<std::string>()const{if(auto p=std::get_if<std::string>(&v_))return *p;throw std::runtime_error("JSON value is not a string");}
template<> inline bool json::get<bool>()const{if(auto p=std::get_if<bool>(&v_))return *p;throw std::runtime_error("JSON value is not boolean");}
template<> inline double json::get<double>()const{if(auto p=std::get_if<double>(&v_))return *p;throw std::runtime_error("JSON value is not number");}
}
