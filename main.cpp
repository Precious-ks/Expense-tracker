// Expense Tracker – Final Project Driver
// Course: CSI218
// Student: Precious Sanyu Kayanja
//
// My Money Tracker stores income and expense
// transactions.Data is managed using a custom dynamic array and a linked list.
//The program supports adding, removing, searching, sorting transactions, and
//calculating totals, while demonstrating core data structures and algorithms
//covered in class.

#include "Tracker.hh"

#include <limits>

#include <iostream>
using namespace std;


// Helper Functions


Transaction readTransaction() {
    string date, desc, cat;
    char type;
    double amount;

    cout << "Date (YYYY-MM-DD): ";
    cin >> date;
    cin.ignore();

    cout << "Description: ";
    getline(cin, desc);

    cout << "Category: ";
    getline(cin, cat);

    cout << "Type (I = Income, E = Expense): ";
    cin >> type;

    cout << "Amount: ";
    cin >> amount;

    return Transaction(date, desc, cat, type, amount);
}

void displayList(const vector<Transaction>& list) {
    if (list.empty()) {
        cout << "No transactions to display.\n";
        return;
    }

    
    for (const auto& t : list) {
        cout << t << endl;
    }
    
}


// Menu

void showMenu() {
 
    cout << "1. Add transaction\n";
    cout << "2. Remove transaction by description\n";
    cout << "3. Show all transactions\n";
    cout << "4. Find transactions by category\n";
    cout << "5. Find transactions by type (I/E)\n";
    cout << "6. Sort by amount (linked list)\n";
    cout << "7. Show totals\n";
    cout << "8. Undo history (stack)\n";
    cout << "9. Save to file\n";
    cout << "10. Load from file\n";
    cout << "0. Exit\n";
    cout << "Choice: ";
}


// Main

int main() {
    Tracker tracker;
    int choice;

    do {
        showMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            Transaction t = readTransaction();
            tracker.addTransaction(t);
            cout << "Transaction added.\n";
            break;
        }
        case 2: {
            string desc;
            cout << "Enter description to remove: ";
            getline(cin, desc);
            if (tracker.removeByDescription(desc))
                cout << "Transaction removed.\n";
            else
                cout << "No matching transaction found.\n";
            break;
        }
        case 3: {
            auto all = tracker.snapshotAll();
            displayList(all);
            break;
        }
        case 4: {
            string cat;
            cout << "Category: ";
            getline(cin, cat);
            displayList(tracker.findAllByCategory(cat));
            break;
        }
        case 5: {
            char type;
            cout << "Type (I/E): ";
            cin >> type;
            displayList(tracker.findAllByType(type));
            break;
        }
        

        case 6: {
            int dir;
            cout << "1 = Ascending, 2 = Descending: ";
            cin >> dir;
            tracker.listMergeSortByAmount(dir == 1);
            cout << "Sorted by amount using merge sort.\n";
            break;
        }
        case 7:
            cout << "Total Income   : $" << tracker.totalIncome() << endl;
            cout << "Total Expenses : $" << tracker.totalExpenses() << endl;
            cout << "Net Balance    : $" << tracker.netBalance() << endl;
            break;

        case 8: {
            string action;
            if (tracker.undoLastAction(action))
                cout << "Undo: " << action << endl;
            else
                cout << "Undo history empty.\n";
            break;
        }
        case 9: {
            string file;
            cout << "Filename to save: ";
            getline(cin, file);
            if (tracker.saveToFile(file))
                cout << "Saved successfully.\n";
            else
                cout << "Error saving file.\n";
            break;
        }
        case 10: {
            string file;
            cout << "Filename to load: ";
            getline(cin, file);
            if (tracker.loadFromFile(file))
                cout << "Loaded successfully.\n";
            else
                cout << "Error loading file.\n";
            break;
        }
        case 0:
            cout << "Goodbye!\n";
            break;

        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
