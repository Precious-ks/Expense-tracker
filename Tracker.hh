

#ifndef TRACKER_HH
#define TRACKER_HH

/*
* NAME:PRECIOUS KAYANJA
 * Expense Tracker (Final Project)
 */

#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <cstddef>

 
 // 1) Transaction Class 

class Transaction {
private:
    std::string date;        // YYYY-MM-DD
    std::string description;
    std::string category;
    char type;               // 'I' income, 'E' expense
    double amount;

public:
    Transaction();       // default 
    Transaction(const std::string& d,
        const std::string& desc,
        const std::string& cat,
        char t,
        double amt);

    // Accessors
    const std::string& getDate() const;
    const std::string& getDescription() const;
    const std::string& getCategory() const;
    char getType() const;
    double getAmount() const;

    // Mutators
    void setDate(const std::string& d);
    void setDescription(const std::string& desc);
    void setCategory(const std::string& cat);
    void setType(char t);
    void setAmount(double amt);

    // Output helper
    friend std::ostream& operator<<(std::ostream& os, const Transaction& t);
};


// 2) Template Class: SimpleStack<T>

template<class T>
class SimpleStack {
private:
    struct SNode {
        T info;
        SNode* linkP;
        SNode(const T& val, SNode* link = nullptr) : info(val), linkP(link) {}
    };

    SNode* topP;
    std::size_t size;

public:
    SimpleStack() : topP(nullptr), size(0) {}//default
    ~SimpleStack() 
    { 
        clear(); 
    }
    //copy constructor
    SimpleStack(const SimpleStack& otherStack) : topP(nullptr), size(0) 
    {
        std::vector<T> temp;// temporay stored values
        
        for (SNode* traverseP = otherStack.topP; traverseP != nullptr; traverseP = traverseP->linkP) {
            temp.push_back(traverseP->info);
        }
        for (std::size_t i = temp.size(); i > 0; --i) {
            push(temp[i - 1]);
        }
    }

    SimpleStack<T>& operator=(const SimpleStack& otherStack) {
        if (this == &otherStack) 
            
         return *this;

        clear();

        std::vector<T> temp;
        
        for (SNode* traverseP = otherStack.topP; traverseP != nullptr; traverseP = traverseP->linkP) {
            temp.push_back(traverseP->info);
        }
        for (std::size_t i = temp.size(); i > 0; --i) {
            push(temp[i - 1]);
        }
        return *this;
    }
    //adds new node which becomes the top of the stack
    void push(const T& val) {
        topP = new SNode(val, topP);
        ++size;
    }

    //removing node and returning with bool and letting the main print
    bool pop(T& out)
    {
        if (!topP) return false;
        SNode* deleteP = topP;
        out = topP->info;
        topP = topP->linkP;
        delete deleteP;
        --size;
        return true;
    }

    //

    bool isEmpty() const 
    { 
        return topP == nullptr; 
    }
    
    void clear() {
        while (topP != nullptr) 
        {
            SNode* deleteP = topP;
            topP = topP->linkP;
            delete deleteP;
        }
        //After loop topP is nullptr
        size = 0;
    }
};


// Tracker Class

class Tracker {
public:
    // Linked List Node 
    struct Node {
        Transaction info;
        Node* linkP;
        Node(const Transaction& t, Node* link = nullptr) : info(t), linkP(link)
        { }
        
        
       
    };

    // Constructors / destructor
    Tracker();
    Tracker(const Tracker& other);
    Tracker& operator=(const Tracker& other);
    ~Tracker();

    // Mutators
    void addTransaction(const Transaction& t);
    bool removeByDescription(const std::string& desc);

    // Searching
    int  dynFindFirstByCategory(const std::string& cat) const; // -1 if not found
    bool listContainsCategory(const std::string& cat) const;

    std::vector<Transaction> findAllByCategory(const std::string& cat);
    std::vector<Transaction> findAllByType(char type); // 'I' or 'E'

    // Sorting 
    
    void listMergeSortByAmount(bool ascending = true);

    // Totals 
    double totalIncome() const;
    double totalExpenses() const;
    double netBalance() const;

    // Snapshot helper
    std::vector<Transaction> snapshotAll() const;

    // File I/O
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    // Stack demo (undo log)
    void logAction(const std::string& action);
    bool undoLastAction(std::string& outAction);

    // Basic sizes
    std::size_t getDynSize() const { return dynSize; }
    std::size_t getListSize() const { return listSize; }

private:
    
    // Dynamic Array 
    
    Transaction* dynArr;
    std::size_t dynSize;
    std::size_t dynCap;

    void dynResize(std::size_t newCap);

    // Linked List
    
    Node* firstP;          // head pointer 
    std::size_t listSize;

    void clearList();
    void appendList(const Tracker& other); // copy helper

    
    // Stack requirement
    
    SimpleStack<std::string> undoLog;

    
    // STL container
    
    std::vector<Transaction> lastQueryResults;
};

#endif // TRACKER_HH
