#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

// -----------------------------------------------------------
// CLASS 1: TaxRelief  (Composition)
// -----------------------------------------------------------
class TaxRelief {
private:
    double individual, medical, lifestyle, insurance;

public:
    TaxRelief() {
        individual = 9000;
        medical = lifestyle = insurance = 0;
    }

    void inputExtraRelief() {
        cout << "Medical Expenses (RM): ";
        cin >> medical;
        if (medical > 7000) medical = 7000;

        cout << "Lifestyle (RM): ";
        cin >> lifestyle;
        if (lifestyle > 4000) lifestyle = 4000;

        cout << "Insurance (RM): ";
        cin >> insurance;
        if (insurance > 6000) insurance = 6000;
    }

    double getTotalRelief() const {
        return individual + medical + lifestyle + insurance;
    }

    friend class TaxSummary;
};


// -----------------------------------------------------------
// CLASS 2: TaxPayer (Base Class)
// -----------------------------------------------------------
class TaxPayer {
protected:
    string id;
    double income, otherIncome, mtd;

public:
    TaxPayer() {
        id = "";
        income = otherIncome = mtd = 0;
    }

    void setData(string pid, double inc, double oth, double m) {
        id = pid;
        income = inc;
        otherIncome = oth;
        mtd = m;
    }

    // ---- PUBLIC GETTERS (fixes protected-access errors) ----
    string getID() const { return id; }
    double getIncome() const { return income; }
    double getOtherIncome() const { return otherIncome; }
    double getMTD() const { return mtd; }

    double getTotalIncome() const {
        return income + otherIncome;
    }

    friend class TaxSummary;
};


// -----------------------------------------------------------
// CLASS 3: TaxSummary (Inheritance)
// -----------------------------------------------------------
class TaxSummary : public TaxPayer {
private:
    TaxRelief relief;

public:

    void inputOtherIncome() {
        cout << "Other Income (RM): ";
        cin >> otherIncome;
    }

    void inputRelief() {
        relief.inputExtraRelief();
    }

    double computeChargeableIncome() {
        double charge = getTotalIncome() - relief.getTotalRelief();
        return (charge < 0) ? 0 : charge;
    }

    double computeTax(double charge) {
        double tax = 0;

        if (charge <= 5000) tax = 0;
        else if (charge <= 20000) tax = (charge - 5000) * 0.01;
        else if (charge <= 35000) tax = 150 + (charge - 20000) * 0.03;
        else if (charge <= 50000) tax = 600 + (charge - 35000) * 0.06;
        else if (charge <= 70000) tax = 1500 + (charge - 50000) * 0.11;
        else if (charge <= 100000) tax = 3700 + (charge - 70000) * 0.19;
        else if (charge <= 400000) tax = 9400 + (charge - 100000) * 0.25;
        else if (charge <= 600000) tax = 84400 + (charge - 400000) * 0.26;
        else if (charge <= 2000000) tax = 136400 + (charge - 600000) * 0.28;
        else tax = 476400 + (charge - 2000000) * 0.30;

        return tax;
    }

    void printSummary() {
        double totalIncome = getTotalIncome();
        double totalRelief = relief.getTotalRelief();
        double charge = computeChargeableIncome();
        double taxPay = computeTax(charge);
        double balance = taxPay - mtd;

        cout << "\n=================================================\n";
        cout << "            Individual Tax Summary\n";
        cout << "=================================================\n";
        cout << fixed << setprecision(2);
        cout << "Total Income (RM): " << totalIncome << endl;
        cout << "Total Tax Relief (RM): " << totalRelief << endl;
        cout << "Chargeable Income (RM): " << charge << endl;
        cout << "Total Tax Payable (RM): " << taxPay << endl;
        cout << "Total Monthly Tax Deduction MTD (RM): " << mtd << endl;

        if (balance <= 0)
            cout << "Status: Refund\nTax Payable for the Year: RM" << balance << endl;
        else
            cout << "Status: Pay Tax\nTax Payable for the Year: RM" << balance << endl;

        cout << "=================================================\n";
    }

    void saveSummaryToFile() {
        ofstream fout("TaxSummary_Output.txt", ios::app);
        double totalIncome = getTotalIncome();
        double totalRelief = relief.getTotalRelief();
        double charge = computeChargeableIncome();
        double taxPay = computeTax(charge);
        double balance = taxPay - mtd;

        fout << fixed << setprecision(2);
        fout << "ID: " << id << "\n";
        fout << "Total Income: RM" << totalIncome << "\n";
        fout << "Relief: RM" << totalRelief << "\n";
        fout << "Chargeable Income: RM" << charge << "\n";
        fout << "Tax Payable: RM" << taxPay << "\n";
        fout << "MTD: RM" << mtd << "\n";
        fout << "Final Amount: RM" << balance << "\n";
        fout << "-------------------------------\n";

        fout.close();
        cout << "Tax Summary saved to file.\n";
    }
};


// -----------------------------------------------------------
// EXTRA FEATURE 1 (Option 4): Sort by Total Income
// -----------------------------------------------------------
void sortByIncome(TaxPayer arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i].getTotalIncome() < arr[j].getTotalIncome()) {
                TaxPayer temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    cout << "Records sorted by Total Income (Highest → Lowest).\n";
}


// -----------------------------------------------------------
// EXTRA FEATURE 2 (Option 5): Show Highest & Lowest Taxpayer
// -----------------------------------------------------------
void showHighestLowest(TaxPayer arr[], int n) {
    if (n == 0) return;

    int hi = 0, lo = 0;
    for (int i = 1; i < n; i++) {
        if (arr[i].getTotalIncome() > arr[hi].getTotalIncome())
            hi = i;
        if (arr[i].getTotalIncome() < arr[lo].getTotalIncome())
            lo = i;
    }

    cout << "\nHighest Income : " << arr[hi].getID() << " RM" << arr[hi].getTotalIncome() << endl;
    cout << "Lowest Income  : " << arr[lo].getID() << " RM" << arr[lo].getTotalIncome() << endl;
}


// -----------------------------------------------------------
// DISPLAY ALL RECORDS
// -----------------------------------------------------------
void displayAll(TaxPayer arr[], int n) {
    cout << "\nDisplay Individual Income Tax Data (" << n << ")\n";
    cout << left << setw(10) << "ID"
         << setw(12) << "Income"
         << setw(12) << "Other"
         << setw(12) << "MTD" << endl;

    for (int i = 0; i < n; i++) {
        cout << left << setw(10) << arr[i].getID()
             << setw(12) << arr[i].getIncome()
             << setw(12) << arr[i].getOtherIncome()
             << setw(12) << arr[i].getMTD() << endl;
    }
}


// -----------------------------------------------------------
// MAIN PROGRAM
// -----------------------------------------------------------
int main() {
    TaxPayer data[20];
    int count = 0;
    int choice;
    bool loaded = false;

    do {
        cout << "\n=================================================\n";
        cout << " 1. Read Tax Data File\n";
        cout << " 2. Display Tax Data Information\n";
        cout << " 3. Search and Calculate Tax\n";
        cout << " 4. Sort Records by Income\n";
        cout << " 5. Display Highest & Lowest Income\n";
        cout << " 6. Save Tax Summary to File\n";
        cout << " 7. Exit Program\n";
        cout << "=================================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // OPTION 1: LOAD FILE
        if (choice == 1) {
            ifstream fin("TaxData.txt");
            if (!fin) {
                cout << "File not found.\n";
                continue;
            }

            string id;
            double inc, oth, m;
            count = 0;

            while (fin >> id >> inc >> oth >> m) {
                data[count].setData(id, inc, oth, m);
                count++;
            }
            fin.close();
            loaded = true;
            cout << "Tax Data Loaded Successfully.\n";
        }

        // OPTION 2: DISPLAY ALL
        else if (choice == 2) {
            if (!loaded) cout << "Please load tax data first.\n";
            else displayAll(data, count);
        }

        // OPTION 3: SEARCH & CALCULATE
        else if (choice == 3) {
            if (!loaded) {
                cout << "Please load data first.\n";
                continue;
            }

            string findID;
            cout << "Input Tax Reference No.: ";
            cin >> findID;

            bool found = false;
            for (int i = 0; i < count; i++) {
                if (data[i].getID() == findID) {
                    found = true;

                    TaxSummary ts;
                    ts.setData(
                        data[i].getID(),
                        data[i].getIncome(),
                        data[i].getOtherIncome(),
                        data[i].getMTD()
                    );

                    cout << "\nPlease provide the following information...\n";
                    ts.inputOtherIncome();
                    ts.inputRelief();
                    ts.printSummary();
                }
            }

            if (!found)
                cout << "Record not found.\n";
        }

        // OPTION 4: SORT BY INCOME
        else if (choice == 4) {
            if (!loaded) cout << "Load data first.\n";
            else sortByIncome(data, count);
        }

        // OPTION 5: HIGHEST & LOWEST
        else if (choice == 5) {
            if (!loaded) cout << "Load data first.\n";
            else showHighestLowest(data, count);
        }

        // OPTION 6: SAVE SUMMARY
        else if (choice == 6) {
            if (!loaded) {
                cout << "Load data first.\n";
                continue;
            }

            string findID;
            cout << "Input Tax Reference No. to save summary: ";
            cin >> findID;

            bool found = false;
            for (int i = 0; i < count; i++) {
                if (data[i].getID() == findID) {
                    found = true;

                    TaxSummary ts;
                    ts.setData(
                        data[i].getID(),
                        data[i].getIncome(),
                        data[i].getOtherIncome(),
                        data[i].getMTD()
                    );

                    ts.inputOtherIncome();
                    ts.inputRelief();
                    ts.saveSummaryToFile();
                }
            }

            if (!found)
                cout << "Record not found.\n";
        }

    } while (choice != 7);

    return 0;
}
