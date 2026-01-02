#include "Tracker.hh"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>  
#include <limits>   // numeric_limits

using std::string;
using std::vector;


// Transaction definitions

Transaction::Transaction()
    : date("0000-00-00"), description(""), category(""), type('E'), amount(0.0) {
}

Transaction::Transaction(const string& d,
    const string& desc,
    const string& cat,
    char t,
    double amt)
    : date(d), description(desc), category(cat), type(t), amount(amt) {
}

const string& Transaction::getDate() const
{
    return date;
}
const string& Transaction::getDescription() const
{
    return description;
}
const string& Transaction::getCategory() const
{
    return category;
}
char Transaction::getType() const
{
    return type;
}
double Transaction::getAmount() const
{
    return amount;
}

void Transaction::setDate(const std::string& d)
{
    date = d;
}
void Transaction::setDescription(const std::string& desc)
{
    description = desc;
}
void Transaction::setCategory(const std::string& cat)
{
    category = cat;
}
void Transaction::setType(char t)
{
    type = t;
}
void Transaction::setAmount(double amt)
{
    amount = amt;
}

std::ostream& operator<<(std::ostream& os, const Transaction& t) {
    os << t.getDate() << " | " << t.getType() << " | "
        << t.getCategory() << " | $"
        << std::fixed << std::setprecision(2) << t.getAmount()
        << " | " << t.getDescription();
    return os;
}


// Tracker 

Tracker::Tracker()
    : dynArr(nullptr), dynSize(0), dynCap(0),
    firstP(nullptr), listSize(0) {
    dynResize(4);
}

Tracker::~Tracker() {
    clearList();
    delete[] dynArr;
    dynArr = nullptr;
    dynSize = 0;
    dynCap = 0;
}


// Dynamic array resize

void Tracker::dynResize(std::size_t newCap) {
    if (newCap < 1) newCap = 1;

    Transaction* newArr = new Transaction[newCap];

    std::size_t copyCount = (dynSize < newCap) ? dynSize : newCap;
    if (dynArr != nullptr) {
        for (std::size_t i = 0; i < copyCount; ++i) {
            newArr[i] = dynArr[i];
        }
    }

    delete[] dynArr;
    dynArr = newArr;
    dynCap = newCap;

    if (dynSize > dynCap) dynSize = dynCap;
}


// Linked list clear 

void Tracker::clearList() {
    Node* p = firstP;
    while (p != nullptr) {
        Node* discard = p;
        p = p->linkP;
        delete discard;
    }
    firstP = nullptr;
    listSize = 0;
}

// Appending other's list to THIS list 
void Tracker::appendList(const Tracker& other) {

    Node* prevP = firstP;
    if (prevP != nullptr) {
        while (prevP->linkP != nullptr) prevP = prevP->linkP;
    }

    for (Node* p = other.firstP; p != nullptr; p = p->linkP) {

        Node* newNode = new Node(p->info, nullptr);

        if (firstP == nullptr) {
            firstP = newNode;
            prevP = newNode;
        }
        else {
            prevP->linkP = newNode;
            prevP = newNode;
        }

        ++listSize;
    }
}






Tracker::Tracker(const Tracker& other)
    : dynArr(nullptr), dynSize(0), dynCap(0),
    firstP(nullptr), listSize(0),
    undoLog(other.undoLog),
    lastQueryResults(other.lastQueryResults) {

    // copy dynamic array 
    dynCap = other.dynCap;
    if (dynCap < 1) dynCap = 4;

    dynSize = other.dynSize;
    if (dynSize > dynCap) dynSize = dynCap;

    dynArr = new Transaction[dynCap];
    for (std::size_t i = 0; i < dynSize; ++i) {
        dynArr[i] = other.dynArr[i];
    }

    // copy linked list 
    clearList();
    appendList(other);
}

Tracker& Tracker::operator=(const Tracker& other) {
    if (this == &other) return *this;

    // clean current
    clearList();
    delete[] dynArr;
    dynArr = nullptr;
    dynSize = 0;
    dynCap = 0;

    // copy stacks/vectors
    undoLog = other.undoLog;
    lastQueryResults = other.lastQueryResults;

    // copy dynamic array
    dynCap = other.dynCap;
    if (dynCap < 1) dynCap = 4;

    dynSize = other.dynSize;
    if (dynSize > dynCap) dynSize = dynCap;

    dynArr = new Transaction[dynCap];
    for (std::size_t i = 0; i < dynSize; ++i) {
        dynArr[i] = other.dynArr[i];
    }

    // copy linked list
    appendList(other);

    return *this;
}


// Add / Remove
void Tracker::addTransaction(const Transaction& t) {
    // Dynamic array append
    if (dynSize >= dynCap) {
        if (dynCap < 1) dynResize(4);
        else dynResize(dynCap * 2);
    }
    dynArr[dynSize++] = t;

    // Linked list add at head 
    firstP = new Node(t, firstP);
    ++listSize;

    logAction("ADD: " + t.getDate() + " " + t.getCategory() + " " + t.getDescription());
}

bool Tracker::removeByDescription(const std::string& desc) {
    bool removedNode = false;

    // Remove from dynamic array
    for (std::size_t i = 0; i < dynSize; ++i) {
        if (dynArr[i].getDescription() == desc) {
            for (std::size_t j = i + 1; j < dynSize; ++j) {
                dynArr[j - 1] = dynArr[j];
            }
            --dynSize;
            removedNode = true;
            break;
        }
    }

    // Remove from linked list 
    Node* p = firstP;
    Node* prevP = nullptr;

    while (p != nullptr) {
        if (p->info.getDescription() == desc) {
            if (prevP == nullptr) {
                firstP = p->linkP;   // removing head
            }
            else {
                prevP->linkP = p->linkP;
            }
            delete p;
            --listSize;
            removedNode = true;
            break;
        }
        prevP = p;
        p = p->linkP;
    }

    return removedNode;
}


// Searching 

int Tracker::dynFindFirstByCategory(const std::string& cat) const {
    for (std::size_t i = 0; i < dynSize; ++i) {
        if (dynArr[i].getCategory() == cat) return static_cast<int>(i);
    }
    return -1;
}

bool Tracker::listContainsCategory(const std::string& cat) const {
    Node* p = firstP;
    while (p != nullptr) {
        if (p->info.getCategory() == cat) return true;
        p = p->linkP;
    }
    return false;
}

std::vector<Transaction> Tracker::findAllByCategory(const std::string& cat) {
    lastQueryResults.clear();
    for (std::size_t i = 0; i < dynSize; ++i) {
        if (dynArr[i].getCategory() == cat) {
            lastQueryResults.push_back(dynArr[i]);
        }
    }
    return lastQueryResults;
}

std::vector<Transaction> Tracker::findAllByType(char t) {
    lastQueryResults.clear();
    for (std::size_t i = 0; i < dynSize; ++i) {
        if (dynArr[i].getType() == t) {
            lastQueryResults.push_back(dynArr[i]);
        }
    }
    return lastQueryResults;
}


// Sorting 

// Linked list merge sort helpers
static Tracker::Node* splitList(Tracker::Node* start) {
    Tracker::Node* slow = start;
    Tracker::Node* fast = start->linkP;

    while (fast != nullptr && fast->linkP != nullptr) {
        slow = slow->linkP;
        fast = fast->linkP->linkP;
    }

    Tracker::Node* second = slow->linkP;
    slow->linkP = nullptr;
    return second;
}

static Tracker::Node* mergeByAmount(Tracker::Node* a,
    Tracker::Node* b,
    bool ascending) {
    if (a == nullptr)
        return b;
    if (b == nullptr)
        return a;

    Tracker::Node* result = nullptr;

    if (ascending) {
        if (a->info.getAmount() <= b->info.getAmount()) {
            result = a;
            result->linkP = mergeByAmount(a->linkP, b, ascending);
        }
        else {
            result = b;
            result->linkP = mergeByAmount(a, b->linkP, ascending);
        }
    }
    else {
        if (a->info.getAmount() >= b->info.getAmount()) {
            result = a;
            result->linkP = mergeByAmount(a->linkP, b, ascending);
        }
        else {
            result = b;
            result->linkP = mergeByAmount(a, b->linkP, ascending);
        }
    }

    return result;
}

static Tracker::Node* mergeSortByAmount(Tracker::Node* start, bool ascending) {
    if (start == nullptr || start->linkP == nullptr) return start;

    Tracker::Node* second = splitList(start);
    start = mergeSortByAmount(start, ascending);
    second = mergeSortByAmount(second, ascending);

    return mergeByAmount(start, second, ascending);
}

void Tracker::listMergeSortByAmount(bool ascending) {
    firstP = mergeSortByAmount(firstP, ascending);
}

// STL container + STL template function 

std::vector<Transaction> Tracker::snapshotAll() const {
    std::vector<Transaction> snap;
    snap.reserve(dynSize);
    for (std::size_t i = 0; i < dynSize; ++i) snap.push_back(dynArr[i]);
    return snap;
}

double Tracker::totalIncome() const {
    auto snap = snapshotAll();
    return std::accumulate(snap.begin(), snap.end(), 0.0,
        [](double sum, const Transaction& t) {
            return (t.getType() == 'I') ? (sum + t.getAmount()) : sum;
        });
}

double Tracker::totalExpenses() const {
    auto snap = snapshotAll();
    return std::accumulate(snap.begin(), snap.end(), 0.0,
        [](double sum, const Transaction& t) {
            return (t.getType() == 'E') ? (sum + t.getAmount()) : sum;
        });
}

double Tracker::netBalance() const {
    return totalIncome() - totalExpenses();
}


// File I/O 


bool Tracker::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return false;

    for (std::size_t i = 0; i < dynSize; ++i) {
        const Transaction& t = dynArr[i];
        out << t.getDate() << " "
            << t.getType() << " "
            << t.getCategory() << " "
            << std::fixed << std::setprecision(2) << t.getAmount() << " "
            << t.getDescription() << "\n";
    }
    return true;
}

bool Tracker::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false;

    // clear current
    clearList();

    delete[] dynArr;
    dynArr = nullptr;
    dynSize = 0;
    dynCap = 0;
    dynResize(4);

    lastQueryResults.clear();
    undoLog.clear();

    std::string date, category, description;
    char type;
    double amount;

    while (in >> date >> type >> category >> amount) {

        in.ignore();                 // ignore the single space after amount
        std::getline(in, description);

        Transaction t(date, description, category, type, amount);
        addTransaction(t);
    }

    logAction("LOAD: " + filename);
    return true;
}



// Undo log (stack usage)

void Tracker::logAction(const std::string& action) {
    undoLog.push(action);
}

bool Tracker::undoLastAction(std::string& outAction) {
    return undoLog.pop(outAction);
}
