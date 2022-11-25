#pragma once

#include <ostream>
#include <vector>
#include <shared_mutex>

struct Node
{
    const std::string data;
    Node* next;
    std::shared_mutex rwLock;
};

class MyTSlist final
{
public:
    MyTSlist();
    ~MyTSlist();

    MyTSlist(const MyTSlist& other) = delete;
    MyTSlist(MyTSlist&& other) = delete;
    MyTSlist& operator=(const MyTSlist& other) = delete;
    MyTSlist& operator=(MyTSlist&& other) = delete;

    class Iterator
    {
    public:
        Iterator(Node* _node);
        const std::string& operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    private:
        Node* node;
    };

    void PushFront(const std::string& s);
    void Sort();
    int SortStep();
    Iterator begin() const;
    Iterator end() const;

private:
    int SortAlgo(bool onestep);
    void InsertAfter(Node* pos, Node* newNode);

    Node* const endNode = []() {
        auto n = new Node{ "", nullptr, {} };
        n->next = n;
        return n;
    }();
};

std::ostream& operator<<(std::ostream& os, const MyTSlist& l);
