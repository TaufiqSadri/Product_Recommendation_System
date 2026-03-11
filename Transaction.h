#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct Transaction {
    string invoiceId;
    string stockCode;
    string description;
    int    quantity;
    string invoiceDate;
    double price;
    string customerId;
    string category;
};

vector<Transaction> transactions;

// Menyimpan waktu eksekusi tiap operasi (ms)
long long lastInsertTime = 0;
long long lastSearchTime = 0;
long long lastUpdateTime = 0;
long long lastDeleteTime = 0;
long long lastShowTime   = 0;

void insertTransaction(Transaction t) {
    transactions.push_back(t);
}

void tampilSemuaTransaksi() {
    vector<Transaction>::iterator it;
    int no = 1;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        cout << no++ << ". "
             << it->invoiceId   << " | "
             << it->stockCode   << " | "
             << it->description << " | Qty: "
             << it->quantity    << " | "
             << it->invoiceDate << " | Rp "
             << it->price       << " | Cust: "
             << it->customerId  << " | "
             << it->category    << "\n";
    }
}

vector<Transaction> searchByInvoiceId(string id) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->invoiceId == id)
            hasil.push_back(*it);
    }
    return hasil;
}

vector<Transaction> searchByCustomerId(string id) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->customerId == id)
            hasil.push_back(*it);
    }
    return hasil;
}

vector<Transaction> searchByStockCode(string code) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->stockCode == code)
            hasil.push_back(*it);
    }
    return hasil;
}

bool updateByInvoiceId(string id, Transaction updated) {
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->invoiceId == id) {
            *it = updated;
            return true;
        }
    }
    return false;
}

bool deleteByInvoiceId(string id) {
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->invoiceId == id) {
            transactions.erase(it);
            return true;
        }
    }
    return false;
}

#endif