#include "list.h"

#include <cassert>
#include <mutex>

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
    Clear();
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

void MyTSlist::InsertAfter(Node* pos, Node* newNode)
{
    newNode->next = pos->next;
    pos->next = newNode;
}

std::ostream& operator<<(std::ostream& os, const MyTSlist& l)
{
    auto listIter = l.GetIterator();
    bool printedOne = false;
    while (listIter.hasNext())
    {
        if (printedOne)
        {
            os << " -> ";
        }
        os << listIter.Next();
        printedOne = true;
    }

    return os;
}

MyTSlist::NewIterator MyTSlist::GetIterator() const
{
    return NewIterator(endNode);
}

void MyTSlist::Clear()
{
    std::unique_lock endLock(endNode->rwLock);
    while (endNode->next != endNode)
    {
        auto detachedNode = endNode->next;
        {
            std::unique_lock l(detachedNode->rwLock);
            endNode->next = detachedNode->next;
        }
        delete detachedNode;
    }
}

MyTSlist::NewIterator::NewIterator(Node* sentinel)
    : sentinel(sentinel)
{
    curr = sentinel;
    currLock = std::shared_lock(curr->rwLock);
    if (curr->next != sentinel)
    {
        next = curr->next;
        nextLock = std::shared_lock(next->rwLock);
    }
}

const std::string& MyTSlist::NewIterator::Next()
{
    assert(hasNext());
    std::swap(curr, next);
    std::swap(currLock, nextLock);
    next = curr->next;
    // sentinel может выступать в роли как начального, так и конечного элемента,
    // так как список циклический. Чтобы избежать дедлоков, его мьютекс захватываем
    // только когда он выступает в роли начального, а тут ситуация обратная.
    nextLock = (next != sentinel) ? std::shared_lock(next->rwLock) :
                                    std::shared_lock<std::shared_mutex>();

    return curr->data;
}

bool MyTSlist::NewIterator::hasNext() const
{
    return next && next != sentinel;
}
