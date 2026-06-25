#pragma once
#include <memory>
#include <string>
#include <vector>
class BTreeIndex {
    static constexpr int T=3;
    struct Node { bool leaf; std::vector<std::string> keys; std::vector<long> positions; std::vector<std::unique_ptr<Node>> children; explicit Node(bool l):leaf(l){} };
    std::unique_ptr<Node> root_;
    static bool search(const Node*,const std::string&,long&);
    static void splitChild(Node*,int);
    static void insertNonFull(Node*,const std::string&,long);
    static void collect(const Node*,std::vector<std::pair<std::string,long>>&);
public:
    BTreeIndex(); void clear(); void insert(const std::string&,long); bool contains(const std::string&)const; long find(const std::string&)const; void erase(const std::string&); std::vector<std::pair<std::string,long>> entries()const;
};
