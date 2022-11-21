#include "list.h"
#include "list.h"
#include "list.h"
#include "list.h"
#include "list.h"
#include "list.h"

MyTSlist::MyTSlist()
{
    end = []() {
        auto n = new Node{ "", nullptr };
        n->next = n;
        return n;
    }();
}

MyTSlist::~MyTSlist()
{
    while (end->next != end)
    {
        auto detachedNode = end->next;
        DetachAfter(end);
        delete detachedNode;
    }

    delete end;
}

void MyTSlist::PushFront(const std::string& s)
{
    Node* newNode = new Node{ s, nullptr };
    InsertAfter(end, newNode);
}

void MyTSlist::Sort()
{
    while (true)
    {
        Node* prev = end;
        Node* curr = end->next;
        Node* next = curr->next;

        int exchanges = 0;
        while (next != end)
        {
            if (curr->data > next->data)
            {
                DetachAfter(prev);
                InsertAfter(next, curr);
                std::swap(curr, next);
                ++exchanges;
            }
            prev = curr;
            curr = next;
            next = next->next;
        }

        if (exchanges == 0)
        {
            break;
        }
    }
}

void MyTSlist::InsertAfter(Node* pos, Node* newNode)
{
    newNode->next = pos->next;
    pos->next = newNode;
}

void MyTSlist::DetachAfter(Node* pos)
{
    pos->next = pos->next->next;
}

std::ostream& operator<<(std::ostream& os, const MyTSlist& l)
{
    for (auto curNode = l.end->next; curNode != l.end; curNode = curNode->next)
    {
        os << curNode->data << " ";
    }

    return os;
}