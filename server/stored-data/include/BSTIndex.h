#pragma once
#include <memory>
#include <string>
#include <vector>

class BSTIndex {
    struct Node { std::string key; long position; std::unique_ptr<Node> left, right; Node(std::string k,long p):key(std::move(k)),position(p){} };
    std::unique_ptr<Node> root_;
    static void insertNode(std::unique_ptr<Node>& n,const std::string& key,long pos);
    static const Node* findNode(const Node* n,const std::string& key);
    static std::unique_ptr<Node> eraseNode(std::unique_ptr<Node> n,const std::string& key);
    static void collect(const Node* n,std::vector<std::pair<std::string,long>>& out);
    static std::unique_ptr<Node> buildBalanced(const std::vector<std::pair<std::string,long>>& entries,int lo,int hi);
public:
    void clear(); void insert(const std::string& key,long position); bool contains(const std::string& key) const; long find(const std::string& key) const; void erase(const std::string& key);
    std::vector<std::pair<std::string,long>> entries() const;
    void buildFromSorted(const std::vector<std::pair<std::string,long>>& entries);
};
