#include "Transaction.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
using namespace std;
using namespace std::chrono;

template<typename Func>
double ukurWaktu(Func f, int ulang = 5) {
    double total = 0;
    for (int i = 0; i < ulang; i++) {
        auto start = high_resolution_clock::now();
        f();
        auto end = high_resolution_clock::now();
        total += duration<double, milli>(end - start).count();
    }
    return total / ulang;
}

void benchmark(vector<Transaction>& data, int n, const string& label) {
    vector<Transaction> subset(data.begin(), data.begin() + min(n, (int)data.size()));

    cout << "\n============================\n";
    cout << "Ukuran data: " << label << "\n";
    cout << "============================\n";

    Transaction dummy;
    dummy.transactionID = "TEST001";
    dummy.customerID    = "C9999";
    dummy.productID     = "P9999";
    dummy.productName   = "Test Product";
    dummy.category      = "Test";
    dummy.quantity      = 1;
    dummy.price         = 9.99;
    dummy.date          = "2024-01-01";

    double t_insert = ukurWaktu([&]() {
        subset.push_back(dummy);
        subset.pop_back();
    });
    cout << "Insert (1 record)   : " << t_insert << " ms\n";

    string targetInvoice = subset[n/2].transactionID;
    double t_invoice = ukurWaktu([&]() {
        for (auto& t : subset)
            if (t.transactionID == targetInvoice) break;
    });
    cout << "Search Invoice      : " << t_invoice << " ms\n";

    string targetCust = subset[n/2].customerID;
    double t_cust = ukurWaktu([&]() {
        vector<Transaction> hasil;
        for (auto& t : subset)
            if (t.customerID == targetCust) hasil.push_back(t);
    });
    cout << "Search Customer ID  : " << t_cust << " ms\n";

    string targetProd = subset[n/2].productID;
    double t_prod = ukurWaktu([&]() {
        vector<Transaction> hasil;
        for (auto& t : subset)
            if (t.productID == targetProd) hasil.push_back(t);
    });
    cout << "Search Product ID   : " << t_prod << " ms\n";

    string targetCat = subset[n/2].category;
    double t_cat = ukurWaktu([&]() {
        vector<Transaction> hasil;
        for (auto& t : subset)
            if (t.category == targetCat) hasil.push_back(t);
    });
    cout << "Search Category     : " << t_cat << " ms\n";
}

int main() {
    cout << "Mengukur Load CSV...\n";
    double t_load = ukurWaktu([&]() {
        vector<Transaction> tmp = loadCSV("transactions.csv");
    });
    cout << "Load CSV (semua)    : " << t_load << " ms\n";

    vector<Transaction> data = loadCSV("transactions.csv");
    cout << "Data dimuat         : " << data.size() << " baris\n";

    benchmark(data, 100,    "100");
    benchmark(data, 500,    "500");
    benchmark(data, 1000,   "1.000");
    benchmark(data, 10000,  "100.000");
    benchmark(data, (int)data.size(), "semua data");

    return 0;
}
