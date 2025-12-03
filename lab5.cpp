#include <iostream>
#include <cassert>
using namespace std;

struct nodeType
{
    int info;
    nodeType *link;
};

class linkedListType
{
public:

    void initializeList();
    bool isEmptyList() const;
    void print() const;
    int length() const;
    void destroyList();
    int front() const;
    int back() const;
    void insertFirst(const int& newItem);
    void insertLast(const int& newItem);
    void deleteNode(const int& deleteItem);

    linkedListType();   
    ~linkedListType();  

private:
    int count;
    nodeType *first;
    nodeType *last;
};

linkedListType::linkedListType()
{
    first = NULL;
    last = NULL;
    count = 0;
}

linkedListType::~linkedListType()
{
    destroyList();
}

bool linkedListType::isEmptyList() const
{
    return (first == NULL);
}

int linkedListType::length() const
{
    return count;
}

int linkedListType::front() const
{
    assert(first != NULL);
    return first->info;
}

int linkedListType::back() const
{
    assert(last != NULL);
    return last->info;
}

void linkedListType::destroyList()
{
    nodeType *temp;

    while (first != NULL)
    {
        temp = first;
        first = first->link;
        delete temp;
    }

    last = NULL;
    count = 0;
}

void linkedListType::initializeList()
{
    destroyList();
}

void linkedListType::insertFirst(const int& newItem)
{
    nodeType *newNode = new nodeType;
    assert(newNode != NULL);

    newNode->info = newItem;
    newNode->link = first;

    if (first == NULL)
        first = last = newNode;
    else
        first = newNode;

    count++;
}

void linkedListType::insertLast(const int& newItem)
{
    nodeType *newNode = new nodeType;
    assert(newNode != NULL);

    newNode->info = newItem;
    newNode->link = NULL;

    if (first == NULL)
        first = last = newNode;
    else
    {
        last->link = newNode;
        last = newNode;
    }

    count++;
}

void linkedListType::deleteNode(const int& deleteItem)
{
    if (first == NULL)
    {
        cout << "Cannot delete from an empty list." << endl;
        return;
    }

    nodeType *current;
    nodeType *trailCurrent;

    if (first->info == deleteItem)
    {
        current = first;
        first = first->link;

        if (first == NULL)
            last = NULL;

        delete current;
        count--;
        return;
    }

    trailCurrent = first;
    current = first->link;

    while (current != NULL)
    {
        if (current->info == deleteItem)
        {
            trailCurrent->link = current->link;

            if (current == last)
                last = trailCurrent;

            delete current;
            count--;
            return;
        }
        trailCurrent = current;
        current = current->link;
    }

    cout << "Item not found." << endl;
}

void linkedListType::print() const
{
    nodeType *current = first;

    if (current == NULL)
    {
        cout << "List is empty." << endl;
        return;
    }

    while (current != NULL)
    {
        cout << current->info << " ";
        current = current->link;
    }

    cout << endl;
}

int main()
{
    int choice = 99;
    int data;
    linkedListType ListObj;

    ListObj.initializeList();

    while (choice != 0)
    {
        cout << "\n===== Linked List Menu =====\n";
        cout << "0. Exit\n";
        cout << "1. Insert First\n";
        cout << "2. Insert Last\n";
        cout << "3. Display First\n";
        cout << "4. Display Last\n";
        cout << "5. Display All Nodes\n";
        cout << "6. Delete a Node\n";
        cout << "7. Delete All Nodes\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                cout << "Enter number to insert at front: ";
                cin >> data;
                ListObj.insertFirst(data);
                break;

            case 2:
                cout << "Enter number to insert at end: ";
                cin >> data;
                ListObj.insertLast(data);
                break;

            case 3:
                if (!ListObj.isEmptyList())
                    cout << "First element: " << ListObj.front() << endl;
                else
                    cout << "List is empty.\n";
                break;

            case 4:
                if (!ListObj.isEmptyList())
                    cout << "Last element: " << ListObj.back() << endl;
                else
                    cout << "List is empty.\n";
                break;

            case 5:
                cout << "All Nodes: ";
                ListObj.print();
                break;

            case 6:
                cout << "Enter number to delete: ";
                cin >> data;
                ListObj.deleteNode(data);
                break;

            case 7:
                ListObj.destroyList();
                cout << "All nodes deleted.\n";
                break;

            default:
                if (choice != 0)
                    cout << "Invalid choice. Try again.\n";
        }
    }

    cout << "Program terminated.\n";
    return 0;
}
