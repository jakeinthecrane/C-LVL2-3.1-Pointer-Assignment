#include <iostream>
#include <stdexcept>
#include <string>  
#include <vector>
#include <sstream>
#include <fstream>
#include <memory> // For std::unique_ptr directive 

using namespace std;

void displayInstructions()
{
    cout << "Personal Expense Tracker to keep you organized and well-balanced!" << endl;
    cout << "Log your expenses by categorizing them and adding the amount. Type 'done' when ready for a summary.\n" << endl;
}

struct Expense
{
    string category;
    double amount = 0.0;
};

class ExpenseTracker
{
private:
    vector<Expense> expenses;
    void loadExpensesFromFile(const string& filename)
    {
        ifstream file(filename);
        if (file.is_open())
        {
            Expense expense;
            while (file >> expense.category >> expense.amount)
            {
                expenses.push_back(expense);
            }
            cout << "Welcome back! Your list of expenses are stored in expenses.txt for whenever you may want to see your progress. Continue to add below:\n";
        }
        else
        {
            cout << "No existing expense file found. Starting fresh.\n";
        }
    }

    void saveExpensesToFile(const string& filename) const
    {
        ofstream file(filename);
        for (const auto& expense : expenses)
        {
            file << expense.category << " " << expense.amount << "\n";
        }
        cout << "Expenses saved to file.\n";
    }

public:
    ExpenseTracker(const string& filename)
    {
        loadExpensesFromFile(filename);
    }

    void addExpense(const string& category, const string& inputAmount)
    {
        double amount;
        stringstream ss(inputAmount);
        if (!(ss >> amount))
        {
            throw invalid_argument("Error: Invalid input. Please enter a numeric value for the amount.");
        }
        if (amount < 0)
        {
            throw out_of_range("Error: Expense amount cannot be negative.");
        }

        Expense expense{ category, amount };
        expenses.push_back(expense);
        cout << "Added expense: " << expense.category << " - $" << expense.amount << endl;
    }

    void searchExpense(const string& searchCategory) const
    {
        bool found = false;
        for (const Expense& expense : expenses)
        {
            if (expense.category == searchCategory)
            {
                cout << "- " << expense.category << ": $" << expense.amount << endl;
                found = true;
            }
        }
        if (!found)
        {
            cout << "No expenses found in category: " << searchCategory << endl;
        }
    }

    void calculateTotal() const
    {
        if (expenses.empty())
        {
            throw runtime_error("Error: No expenses recorded. Please add expenses before calculating the total.");
        }

        double total = 0;
        for (const auto& expense : expenses)
        {
            total += expense.amount;
        }
        cout << "\nTotal spending: $" << total << endl;
    }

    void displayExpenses() const
    {
        if (expenses.empty())
        {
            cout << "No expenses recorded yet." << endl;
            return;
        }
        cout << "\nRecorded Expenses:" << endl;
        for (const auto& expense : expenses)
        {
            cout << "- " << expense.category << ": $" << expense.amount << endl;
        }
    }

    void saveToFile(const string& filename) const
    {
        saveExpensesToFile(filename);
    }
};

int main()
{
    displayInstructions();

    string filename = "expenses.txt";

    /* unique_ptr created named tracker which will manage the ExpenseTracker object.Tracker is initialized with make_unique pointer which dynamically
       allocates the ExpenseTracker on the heap and not stack memory. filename is passed to the ExpenseTracker constructor.  Tracker now holds the
       pointer to this new object created.. */
    std::unique_ptr<ExpenseTracker> tracker = std::make_unique<ExpenseTracker>(filename);
    //  I'm using this pointer here becauase it is well suited for managing the lifetime of the single ExpenseTracker object automatically. 

    bool success = true;

    try
    {
        string category, amount;
        while (true)
        {
            cout << "\nEnter an expense category (or type 'SEARCH' to look up a category. Otherwise 'DONE' to finish): ";
            getline(cin, category);
            if (category == "DONE") break;
            if (category == "SEARCH")
            {
                cout << "Enter category to search: ";
                string searchCategory;
                getline(cin, searchCategory);
                tracker->searchExpense(searchCategory);
                continue;
            }

            cout << "Enter the amount spent on " << category << ": $";
            cin >> amount;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            tracker->addExpense(category, amount);
        }

        tracker->displayExpenses();
        tracker->calculateTotal();
    }
    catch (const invalid_argument& e)
    {
        cout << e.what() << endl;
        success = false;
    }
    catch (const out_of_range& e)
    {
        cout << e.what() << endl;
        success = false;
    }
    catch (const runtime_error& e)
    {
        cout << e.what() << endl;
        success = false;
    }

    if (success)
    {
        tracker->saveToFile(filename);
        cout << "\nGreat job on staying on top of your finances!" << endl;
    }

    // `tracker` goes out of scope here, and the ExpenseTracker object it points to is automatically deleted.
    return 0;
}
