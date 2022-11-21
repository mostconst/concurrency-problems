#pragma once

#include <ostream>

class MyTSlist final
{
public:
    MyTSlist();
    ~MyTSlist();

    void PushFront(const std::string& s);
    void Sort();
    friend std::ostream& operator<<(std::ostream& os, const MyTSlist& l);

private:
    struct Node
    {
        const std::string data;
        Node* next;
    };

    void InsertAfter(Node* pos, Node* newNode);
    void DetachAfter(Node* pos);

    Node* end = nullptr;
};