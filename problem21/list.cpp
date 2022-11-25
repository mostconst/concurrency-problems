#include "list.h"

#include <cassert>

class NodeWriteAccess
{
public:
    NodeWriteAccess(Node* n)
        : node(n)
        , lock(node->rwLock)
    {}
public:
    Node* node;
    std::unique_lock<std::shared_mutex> lock;
};

MyTSlist::MyTSlist()
{
    //end = []() {
    //    auto n = new Node{ "", nullptr };
    //    n->next = n;
    //    return n;
    //}();
}

MyTSlist::~MyTSlist()
{
    {
        std::unique_lock endLock(endNode->rwLock);
        while (endNode->next != endNode)
        {
            auto detachedNode = endNode->next;
            {
                std::shared_lock l(detachedNode->rwLock);
                endNode->next = detachedNode->next;
            }
            delete detachedNode;
        }
    }

    delete endNode;
}

void MyTSlist::PushFront(const std::string& s)
{
    Node* newNode = new Node{ s, nullptr, {} };
    std::unique_lock lock(endNode->rwLock);
    InsertAfter(endNode, newNode);
}

void MyTSlist::Sort()
{
    SortAlgo(false);
}

int MyTSlist::SortStep()
{
    return SortAlgo(true);
}

int MyTSlist::SortAlgo(bool onestep)
{
    int totalExchanges = 0;
    while (true)
    {
        Node* prev = endNode;
        std::unique_lock prevLock(prev->rwLock);

        if (prev->next == endNode)
        {
            break;
        }
        Node* curr = prev->next;
        std::unique_lock currLock(curr->rwLock);


        if (curr->next == endNode)
        {
            break;
        }
        Node* next = curr->next;
        std::unique_lock nextLock(next->rwLock);

        int passExchanges = 0;
        while (true)
        {
            if (curr->data > next->data)
            {
                prev->next = next;
                InsertAfter(next, curr);
                std::swap(curr, next);
                std::swap(currLock, nextLock);
                ++passExchanges;
                ++totalExchanges;
                if (onestep)
                {
                    break;
                }
            }
            if (next->next == endNode)
            {
                break;
            }
            prev = curr;
            prevLock = std::move(currLock);
            curr = next;
            currLock = std::move(nextLock);
            next = next->next;
            nextLock = std::unique_lock(next->rwLock);
        }

        if (passExchanges == 0 || onestep)
        {
            break;
        }
    }

    return totalExchanges;
}

MyTSlist::Iterator MyTSlist::begin() const
{
    std::shared_lock lock(endNode->rwLock);
    return Iterator(endNode->next);
}

MyTSlist::Iterator MyTSlist::end() const
{
    return Iterator{ endNode };
}

void MyTSlist::InsertAfter(Node* pos, Node* newNode)
{
    newNode->next = pos->next;
    pos->next = newNode;
}

std::ostream& operator<<(std::ostream& os, const MyTSlist& l)
{
    for (auto iter = l.begin(); iter != l.end(); ++iter)
    {
        if (iter != l.begin())
        {
            os << " -> ";
        }
        os << *iter;
    }

    return os;
}


//-----------------------------------------------------------------------------
// 
// ---
MyTSlist::Iterator::Iterator(Node* _node)
    : node(_node)
{

}

const std::string& MyTSlist::Iterator::operator*() const
{
    return node->data;
}


MyTSlist::Iterator& MyTSlist::Iterator::operator++()
{
    std::shared_lock lock(node->rwLock);
    node = node->next;
    return *this;
}


bool MyTSlist::Iterator::operator!=(const Iterator& other) const
{
    return node != other.node;
}
