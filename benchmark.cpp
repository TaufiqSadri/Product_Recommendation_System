#include "readwrite.h"
#include "Transaction.h"
#include <iostream>
#include <chrono>
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

void benchmark(int n, const string& label) {
    vector<Transaction> backup = transactions;
    transactions = vector<Transaction>(backup.begin(), backup.begin() + min(n, (int)backup.size()));

    cout << "\n============================\n";
    cout << "Ukuran data: " << label << "\n";
    cout << "============================\n";

    string targetInvoice = transactions[n/2].invoiceId;
    string targetCust    = transactions[n/2].customerId;
    string targetStock   = transactions[n/2].stockCode;

    // INSERT
    Transaction dummy;
    dummy.invoiceId   = "TEST999";
    dummy.stockCode   = "S9999";
    dummy.description = "Test Product";
    dummy.quantity    = 1;
    dummy.invoiceDate = "2024-01-01";
    dummy.price       = 9.99;
    dummy.customerId  = "C9999";
    dummy.category    = "Test";

    double t_insert = ukurWaktu([&]() {
        insertTransaction(dummy);
        transactions.pop_back();
    });
    cout << "Insert (1 record)    : " << t_insert << " ms\n";

    // SEARCH INVOICE ID
    double t_invoice = ukurWaktu([&]() {
        searchByInvoiceId(targetInvoice);
    });
    cout << "Search Invoice ID    : " << t_invoice << " ms\n";

    // SEARCH CUSTOMER ID
    double t_cust = ukurWaktu([&]() {
        searchByCustomerId(targetCust);
    });
    cout << "Search Customer ID   : " << t_cust << " ms\n";

    // SEARCH STOCK CODE
    double t_stock = ukurWaktu([&]() {
        searchByStockCode(targetStock);
    });
    cout << "Search Stock Code    : " << t_stock << " ms\n";

    // UPDATE
    insertTransaction(dummy);
    Transaction updated = dummy;
    updated.description = "Updated Product";
    updated.price = 19.99;
    double t_update = ukurWaktu([&]() {
        updateByInvoiceId(dummy.invoiceId, updated);
    });
    deleteByInvoiceId(dummy.invoiceId);
    cout << "Update (by InvoiceID): " << t_update << " ms\n";

    // DELETE
    double t_delete = ukurWaktu([&]() {
        insertTransaction(dummy);
        deleteByInvoiceId(dummy.invoiceId);
    });
    cout << "Delete (by InvoiceID): " << t_delete << " ms\n";

    transactions = backup;
}

int main() {
    cout << "Mengukur Load CSV...\n";
    double t_load = ukurWaktu([&]() {
        transactions.clear();
        loadCSV("transactions.csv");
    });
    cout << "Load CSV             : " << t_load << " ms\n";

    transactions.clear();
    int loaded = loadCSV("transactions.csv");
    cout << "Data dimuat          : " << loaded << " baris\n";

    benchmark(100,  "100");
    benchmark(1000,  "1.000");
    benchmark(10000, "10.000");
    benchmark(100000, "100.000");

    cout << "\n============================\n";
    cout << "SELESAI\n";
    cout << "============================\n";

    return 0;
}
