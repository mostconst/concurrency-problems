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

    class NewIterator
    {
    public:
        NewIterator(Node* sentinel);
        const std::string& Next();
        bool hasNext() const;
    private:
        const Node* const sentinel;
        Node* curr = nullptr;
        std::shared_lock<std::shared_mutex> currLock;
        Node* next = nullptr;
        std::shared_lock<std::shared_mutex> nextLock;
    };

    void PushFront(const std::string& s);
    void Sort();
    int SortStep();
    [[nodiscard]] NewIterator GetIterator() const;

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
