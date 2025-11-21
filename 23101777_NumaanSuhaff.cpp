// 23101777_NumaanSuhaff_fixed.cpp

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>    
#include <cctype>
using namespace std;
namespace fs = std::filesystem;

string up(const string &s)
{
    string r = s;
    for(char &c : r) c = toupper((unsigned char)c);
    return r;
}

// -----------------------------------------------------------
// CLASS 1: TaxRelief  (Composition)
// -----------------------------------------------------------
class TaxRelief 
{
private:
    double individual, medical, lifestyle, insurance;

public:
    TaxRelief() 
    {
        individual = 9000;
        medical = lifestyle = insurance = 0;
    }

    void inputExtraRelief() 
    {
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

    double getTotalRelief() const 
    {
        return individual + medical + lifestyle + insurance;
    }

    // getters for saving/inspection
    double getIndividual() const { return individual; }
    double getMedical() const { return medical; }
    double getLifestyle() const { return lifestyle; }
    double getInsurance() const { return insurance; }

    // setters (used when loading previously-entered relief)
    void setMedical(double m) { medical = (m>7000?7000:m); }
    void setLifestyle(double l) { lifestyle = (l>4000?4000:l); }
    void setInsurance(double i) { insurance = (i>6000?6000:i); }
};

// -----------------------------------------------------------
// CLASS 2: TaxPayer (Base Class)
// -----------------------------------------------------------
class TaxPayer 
{
protected:
    string id;
    double income, otherIncome, mtd;

public:
    TaxPayer() 
    {
        id = "";
        income = otherIncome = mtd = 0;
    }

    void setData(const string& pid, double inc, double oth, double m) 
    {
        id = pid;
        income = inc;
        otherIncome = oth;
        mtd = m;
    }

    double getTotalIncome() const 
    {
        return income + otherIncome;
    }

    // accessors for external functions
    string getId() const { return id; }
    double getIncome() const { return income; }
    double getOtherIncome() const { return otherIncome; }
    double getMtd() const { return mtd; }

    void setIncome(double v) { income = v; }
    void setOtherIncome(double v) { otherIncome = v; }
    void setMtd(double v) { mtd = v; }
};

// -----------------------------------------------------------
// CLASS 3: TaxSummary (Inheritance + Friend Function)
// -----------------------------------------------------------
class TaxSummary : public TaxPayer 
{
private:
    TaxRelief relief;

public:
    void setRelief(const TaxRelief &r) { relief = r; }

    TaxRelief getRelief() const { return relief; }

    void inputRelief() 
    {
        relief.inputExtraRelief();
    }

    double computeChargeableIncome() const 
    {
        double charge = getTotalIncome() - relief.getTotalRelief();
        return (charge < 0) ? 0 : charge;
    }

    double computeTaxAndContribs(double charge, vector<pair<string,double>>& contribs) const 
    {
        contribs.clear();
        if (charge <= 0) 
        {
            return 0.0;
        }

        struct Br { double upper; double rate; };
        vector<Br> brackets = 
        {
            {5000, 0.00},
            {20000, 0.01},
            {35000, 0.03},
            {50000, 0.06},
            {70000, 0.11},
            {100000, 0.19},
            {400000, 0.25},
            {600000, 0.26},
            {2000000, 0.28},
            {1e18, 0.30}
        };

        double prev = 0.0;
        double totalTax = 0.0;
        for (auto &b : brackets) 
        {
            double top = b.upper;
            if (charge > prev) 
            {
                double portion = min(charge, top) - prev;
                double partTax = portion * b.rate;
                string label = to_string((long long)prev+1) + "-" + ( (top>1e17) ? "Above" : to_string((long long)top) );
                contribs.push_back({label, partTax});
                totalTax += partTax;
            } 
            else 
            {
                contribs.push_back({ "0", 0.0 });
            }
            prev = top;
            if (prev >= charge) break;
        }
        return totalTax;
    }

    double computeTax(double charge) const 
    {
        vector<pair<string,double>> contribs;
        return computeTaxAndContribs(charge, contribs);
    }

    // prints a detailed summary including first amount / remaining lines
    void printSummaryDetailed() 
    {
        double totalIncome = getTotalIncome();
        double totalRelief = relief.getTotalRelief();
        double charge = computeChargeableIncome();

        vector<pair<string,double>> contribs;
        double totalTax = computeTaxAndContribs(charge, contribs);

        // tax on first RM100,000
        double firstLimit = 100000.0;
        vector<pair<string,double>> contribs100k;
        double taxOnFirst100k = computeTaxAndContribs(min(charge, firstLimit), contribs100k);
        double taxRemaining = totalTax - taxOnFirst100k;
        double balance = totalTax - mtd;

        cout << "\n=================================================\n";
        cout << "            Individual Tax Summary\n";
        cout << "=================================================\n";
        cout << fixed << setprecision(2);
        cout << "Total Income (RM): " << totalIncome << endl;
        cout << "Total Tax Relief (RM): " << totalRelief << endl;
        cout << "Chargeable Income (RM): " << charge << endl;

        // display breakdown lines 
        cout << "Tax of the first amount (up to RM100,000) : RM" << taxOnFirst100k << endl;
        cout << "Tax on remaining amount                 : RM" << taxRemaining << endl;

        cout << "Total tax payable                        : RM" << totalTax << endl;
        cout << "Total Monthly Tax Deduction MTD (RM): " << mtd << endl;

        if (balance <= 0)
            cout << "Status: Refund\nTax Payable for the Year: RM" << balance << endl;
        else
            cout << "Status: Pay Tax\nTax Payable for the Year: RM" << balance << endl;

        cout << "=================================================\n";
    }

    void saveSummaryToFile(const string& filename) 
    {
        ofstream fout(filename, ios::app);
        double totalIncome = getTotalIncome();
        double totalRelief = relief.getTotalRelief();
        double charge = computeChargeableIncome();

        vector<pair<string,double>> contribs;
        double totalTax = computeTaxAndContribs(charge, contribs);
        vector<pair<string,double>> contribs100k;
        double taxOnFirst100k = computeTaxAndContribs(min(charge, 100000.0), contribs100k);
        double taxRemaining = totalTax - taxOnFirst100k;
        double balance = totalTax - mtd;

        fout << "ID: " << id << "\n";
        fout << fixed << setprecision(2);
        fout << "Total Income: RM" << totalIncome << "\n";
        fout << "Total Relief: RM" << totalRelief << "\n";
        fout << "Chargeable Income: RM" << charge << "\n";
        fout << "Tax of the first amount (up to RM100,000): RM" << taxOnFirst100k << "\n";
        fout << "Tax on remaining amount: RM" << taxRemaining << "\n";
        fout << "Total Tax Payable: RM" << totalTax << "\n";
        fout << "MTD: RM" << mtd << "\n";
        fout << "Final Amount (TaxPay - MTD): RM" << balance << "\n";
        fout << "-------------------------------------------\n";

        fout.close();
    }
};

// -----------------------------------------------------------
// UTILITY: displayAll (uses getters)
// -----------------------------------------------------------
void displayAll(TaxPayer arr[], int n) 
{
    cout << "\nDisplay Individual Income Tax Data (" << n << ")\n";
    cout << left << setw(10) << "ID"
         << setw(14) << "Income"
         << setw(14) << "Other"
         << setw(14) << "MTD" << endl;

    cout << fixed << setprecision(2);
    for (int i = 0; i < n; i++) 
    {
        cout << left << setw(10) << arr[i].getId()
             << setw(14) << arr[i].getIncome()
             << setw(14) << arr[i].getOtherIncome()
             << setw(14) << arr[i].getMtd() << endl;
    }
}

// -----------------------------------------------------------
// SORT: by total income (desc/asc) and by ID (use getters)
// -----------------------------------------------------------
void sortByIncome(TaxPayer arr[], int n, bool highestFirst=true) 
{
    sort(arr, arr + n, [highestFirst](const TaxPayer &a, const TaxPayer &b) 
    {
        if (highestFirst) return a.getTotalIncome() > b.getTotalIncome();
        else return a.getTotalIncome() < b.getTotalIncome();
    });
    cout << "Records sorted by Total Income (" << (highestFirst ? "Highest → Lowest" : "Lowest → Highest") << ").\n";
}

void sortById(TaxPayer arr[], int n) 
{
    sort(arr, arr + n, [](const TaxPayer &a, const TaxPayer &b) 
    {
        return a.getId() < b.getId();
    });
    cout << "Records sorted by ID (ascending).\n";
}

// -----------------------------------------------------------
// EDIT RECORD (replace highest/lowest option)
// -----------------------------------------------------------
void editRecord(TaxPayer arr[], int n, vector<TaxRelief>& reliefs, vector<bool>& hasRelief) 
{
    if (n == 0) 
    {
        cout << "No data loaded.\n";
        return;
    }
    string id;
    cout << "Enter ID to edit: ";
    cin >> id;
    string uid = up(id);
    int idx = -1;
    for (int i = 0; i < n; ++i) {
        if (up(arr[i].getId()) == uid) { idx = i; break; }
    }
    if (idx == -1) 
    {
        cout << "Record not found.\n";
        return;
    }
    cout << "Editing record for " << arr[idx].getId() << "\n";
    cout << "Current Income: " << arr[idx].getIncome() << "  Enter new income (or -1 to keep): ";
    double tmp; cin >> tmp;
    if (tmp >= 0) arr[idx].setIncome(tmp);

    cout << "Current Other Income: " << arr[idx].getOtherIncome() << "  Enter new other income (or -1 to keep): ";
    cin >> tmp;
    if (tmp >= 0) arr[idx].setOtherIncome(tmp);

    cout << "Current MTD: " << arr[idx].getMtd() << "  Enter new MTD (or -1 to keep): ";
    cin >> tmp;
    if (tmp >= 0) arr[idx].setMtd(tmp);

    if (hasRelief[idx]) 
    {
        cout << "Keep previously entered relief data for this record? (y/n): ";
        char ch; cin >> ch;
        if (ch == 'n' || ch == 'N') 
        {
            hasRelief[idx] = false;
            reliefs[idx] = TaxRelief();
        }
    } else 
    {
        cout << "No relief data exists yet for this record.\n";
    }

    cout << "Record updated.\n";
}

// -----------------------------------------------------------
// HELPER: list .txt files in current directory and choose
// -----------------------------------------------------------
string chooseFileFromFolder() 
{
    vector<fs::directory_entry> txtFiles;
    for (auto &p : fs::directory_iterator(fs::current_path())) 
    {
        if (!p.is_regular_file()) continue;
        string ext = p.path().extension().string();
        if (ext == ".txt" || ext == ".TXT") {
            txtFiles.push_back(p);
        }
    }
    if (txtFiles.empty()) 
    {
        cout << "No .txt files found in folder.\n";
        return "";
    }
    cout << "Found " << txtFiles.size() << " text file(s):\n";
    for (size_t i = 0; i < txtFiles.size(); ++i) 
    {
        cout << i+1 << ". " << txtFiles[i].path().filename().string() << "\n";
    }
    cout << "Enter number of file to load: ";
    int sel; cin >> sel;
    if (sel < 1 || sel > (int)txtFiles.size()) 
    {
        cout << "Invalid selection.\n";
        return "";
    }
    return txtFiles[sel-1].path().string();
}

// -----------------------------------------------------------
// MAIN PROGRAM
// -----------------------------------------------------------
int main() 
{
    TaxPayer data[50];   
    int count = 0;
    int choice;
    bool loaded = false;

    vector<TaxRelief> reliefs(50);
    vector<bool> hasRelief(50, false);

    do 
    {
        cout << "\n=================================================\n";
        cout << " 1. Read Tax Data File\n";
        cout << " 2. Display Tax Data Information\n";
        cout << " 3. Search and Calculate Tax\n";
        cout << " 4. Sort Records (choose method)\n";
        cout << " 5. Edit a Record by ID\n";
        cout << " 6. Save Tax Summary to File\n";
        cout << " 7. Exit Program\n";
        cout << "=================================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // OPTION 1: LOAD FILE (multiple files listing)
        if (choice == 1) 
        {
            string filepath = chooseFileFromFolder();
            if (filepath.empty()) continue;

            ifstream fin(filepath);
            if (!fin) {
                cout << "Unable to open file: " << filepath << "\n";
                continue;
            }

            string id;
            double inc, oth, m;
            count = 0;
            while (fin >> id >> inc >> oth >> m) 
            {
                data[count].setData(id, inc, oth, m);
                hasRelief[count] = false;         
                reliefs[count] = TaxRelief();
                count++;
                if (count >= 50) break;
            }
            fin.close();
            loaded = true;
            cout << "Tax Data Loaded Successfully from " << filepath << ". Records: " << count << "\n";
        }

        // OPTION 2: DISPLAY ALL
        else if (choice == 2) 
        {
            if (!loaded) cout << "Please load tax data first.\n";
            else displayAll(data, count);
        }

        // OPTION 3: SEARCH & CALCULATE
        else if (choice == 3) 
        {
            if (!loaded) {
                cout << "Please load data first.\n";
                continue;
            }
            string findID;
            cout << "Input Tax Reference No.: ";
            cin >> findID;
            string ufind = up(findID);

            bool found = false;
            for (int i = 0; i < count; i++) 
            {
                if (up(data[i].getId()) == ufind) 
                {
                    found = true;

                    // --- ASK (or update) Other Income BEFORE computing ---
                    cout << "Current Other Income for " << data[i].getId() << ": " << data[i].getOtherIncome() << endl
                         << "  Enter new Other Income to use (or -1 to keep current): ";
                    double newOther; cin >> newOther;
                    if (newOther >= 0) 
                    {
                        data[i].setOtherIncome(newOther);
                    }

                    TaxSummary ts;
                    ts.setData(data[i].getId(), data[i].getIncome(), data[i].getOtherIncome(), data[i].getMtd());

                    // if we already have relief for this record, ask whether to reuse
                    if (hasRelief[i]) 
                    {
                        cout << "Previously entered relief exists for this record. Use it? (y/n): ";
                        char c; cin >> c;
                        if (c == 'y' || c == 'Y') 
                        {
                            ts.setRelief(reliefs[i]);
                        } 
                        else 
                        {
                            cout << "\nPlease provide the following information...\n";
                            ts.inputRelief();
                            reliefs[i] = ts.getRelief();
                            hasRelief[i] = true;
                        }
                    } 
                    else 
                    {
                        cout << "\nPlease provide the following information...\n";
                        ts.inputRelief();
                        reliefs[i] = ts.getRelief();
                        hasRelief[i] = true;
                    }

                    ts.printSummaryDetailed();
                    break;
                }
            }
            if (!found) cout << "Record not found.\n";
        }

        // OPTION 4: SORT (suboptions)
        else if (choice == 4) 
        {
            if (!loaded) { cout << "Load data first.\n"; continue; }
            cout << "Sort by:\n1. Highest -> Lowest income\n2. Lowest -> Highest income\n3. By ID (ascending)\nChoose: ";
            int s; cin >> s;
            if (s == 1) sortByIncome(data, count, true);
            else if (s == 2) sortByIncome(data, count, false);
            else if (s == 3) sortById(data, count);
            else cout << "Invalid sort option.\n";
        }

        // OPTION 5: EDIT RECORD BY ID
        else if (choice == 5) 
        {
            if (!loaded) { cout << "Load data first.\n"; continue; }
            editRecord(data, count, reliefs, hasRelief);
        }

        // OPTION 6: SAVE TAX SUMMARY TO FILE
        else if (choice == 6) 
        {
            if (!loaded) { cout << "Load data first.\n"; continue; }
            string findID;
            cout << "Input Tax Reference No. to save summary: ";
            cin >> findID;
            string ufind = up(findID);

            bool found = false;
            for (int i = 0; i < count; i++) 
            {
                if (up(data[i].getId()) == ufind) 
                {
                    found = true;
                    TaxSummary ts;
                    ts.setData(data[i].getId(), data[i].getIncome(), data[i].getOtherIncome(), data[i].getMtd());

                    if (!hasRelief[i]) 
                    {
                        cout << "No relief data found for this record. Please enter relief details now.\n";
                        ts.inputRelief();
                        reliefs[i] = ts.getRelief();
                        hasRelief[i] = true;
                    } else {
                        ts.setRelief(reliefs[i]);
                    }

                    ts.saveSummaryToFile("TaxSummary_Output.txt");
                    cout << "Saved summary for " << data[i].getId() << " to TaxSummary_Output.txt\n";
                    break;
                }
            }
            if (!found) cout << "Record not found.\n";
        }
        else 
        {
            if (choice != 7) cout << "Unknown option.\n";
        }

    } while (choice != 7);

    cout << "Exiting program.\n";
    return 0;
}
