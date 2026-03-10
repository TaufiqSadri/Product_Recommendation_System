#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <ctime>

using namespace std;

struct Transaction {
    string transactionID;
    string customerID;
    string productID;
    string productName;
    string category;
    int    quantity;
    double price;
    string date;
};

string waktuSekarang() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

void tulisLog(string isi) {
    ofstream f("output_log.txt", ios::trunc);
    f << isi;
    f.close();
}

vector<Transaction> loadCSV(string filename) {
    vector<Transaction> data;
    ifstream file(filename);
    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string inv, stock, desc, qty, tgl, harga, cust, cat;

        getline(ss, inv,   ';');
        getline(ss, stock, ';');
        getline(ss, desc,  ';');
        getline(ss, qty,   ';');
        getline(ss, tgl,   ';');
        getline(ss, harga, ';');
        getline(ss, cust,  ';');
        getline(ss, cat,   ';');

        Transaction t;
        t.transactionID = inv;
        t.productID     = stock;
        t.productName   = desc;
        t.quantity      = stoi(qty);
        t.price         = stod(harga);
        t.date          = tgl;
        t.customerID    = cust;
        t.category      = cat;

        data.push_back(t);
    }

    file.close();
    return data;
}

void insertData(vector<Transaction>& data, Transaction t) {
    data.push_back(t);
    tulisLog("\n[" + waktuSekarang() + "] INSERT\n" +
             t.transactionID + " | " + t.customerID + " | " + t.productID +
             " | " + t.productName + " | " + t.category +
             " | " + to_string(t.quantity) + " | " + t.date + "\n");
}

vector<Transaction> cariInvoice(vector<Transaction> data, string id) {
    vector<Transaction> hasil;
    for (auto t : data)
        if (t.transactionID == id) hasil.push_back(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH INVOICE: " + id + "\n" +
             "Ditemukan " + to_string(hasil.size()) + " baris\n");
    return hasil;
}

vector<Transaction> cariCustomer(vector<Transaction> data, string id) {
    vector<Transaction> hasil;
    for (auto t : data)
        if (t.customerID == id) hasil.push_back(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH CUSTOMER: " + id + "\n" +
             "Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    return hasil;
}

vector<Transaction> cariProduct(vector<Transaction> data, string id) {
    vector<Transaction> hasil;
    for (auto t : data)
        if (t.productID == id) hasil.push_back(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH PRODUCT: " + id + "\n" +
             "Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    return hasil;
}

vector<Transaction> cariCategory(vector<Transaction> data, string cat) {
    vector<Transaction> hasil;
    for (auto t : data)
        if (t.category == cat) hasil.push_back(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH CATEGORY: " + cat + "\n" +
             "Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    return hasil;
}

set<string> getDaftarKategori(vector<Transaction> data) {
    set<string> cats;
    for (auto t : data) cats.insert(t.category);
    return cats;
}

#endif