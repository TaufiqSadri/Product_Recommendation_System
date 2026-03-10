#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <iomanip>
#include <limits>
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

// isi log ke file
void tulisLog(string isi) {
    ofstream f("output_log.txt", ios::app);
    f << isi;
    f.close();
}

// ambil waktu sekarang
string waktuSekarang() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
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

void printHeader() {
    cout << string(110, '-') << "\n";
    cout << left
         << setw(12) << "Invoice"
         << setw(10) << "CustID"
         << setw(12) << "ProductID"
         << setw(32) << "Product Name"
         << setw(16) << "Category"
         << setw(6)  << "Qty"
         << setw(8)  << "Price"
         << "Date" << "\n";
    cout << string(110, '-') << "\n";
}

void printRow(Transaction t) {
    cout << left
         << setw(12) << t.transactionID
         << setw(10) << t.customerID
         << setw(12) << t.productID
         << setw(32) << t.productName.substr(0, 31)
         << setw(16) << t.category.substr(0, 15)
         << setw(6)  << t.quantity
         << setw(8)  << fixed << setprecision(2) << t.price
         << t.date   << "\n";
}

// baris log per transaksi
string barisLog(Transaction t) {
    return t.transactionID + " | " + t.customerID + " | " + t.productID +
           " | " + t.productName + " | " + t.category +
           " | " + to_string(t.quantity) + " | " + t.date + "\n";
}

// MENU 1 - TAMPILKAN SEMUA
void tampilSemua(vector<Transaction> data) {
    int total = data.size();
    cout << "\nTotal: " << total << " baris\n";
    printHeader();
    for (int i = 0; i < total; i++) printRow(data[i]);

    tulisLog("\n[" + waktuSekarang() + "] TAMPIL SEMUA\n");
    tulisLog("Total: " + to_string(total) + " baris\n");
}

// MENU 2 - INSERT
void insertData(vector<Transaction>& data) {
    Transaction t;
    cout << "\n--- INSERT TRANSAKSI ---\n";
    cout << "Invoice    : "; cin >> t.transactionID;
    cout << "Customer ID: "; cin >> t.customerID;
    cout << "Product ID : "; cin >> t.productID;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nama Produk: "; getline(cin, t.productName);
    cout << "Category   : "; getline(cin, t.category);
    cout << "Quantity   : "; cin >> t.quantity;
    cout << "Price      : "; cin >> t.price;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Date       : "; getline(cin, t.date);

    data.push_back(t);
    cout << "Berhasil ditambahkan.\n";

    tulisLog("\n[" + waktuSekarang() + "] INSERT\n");
    tulisLog(barisLog(t));
}

// MENU 3 - SEARCH BY INVOICE
void cariInvoice(vector<Transaction> data) {
    string id;
    cout << "\n--- SEARCH BY INVOICE ---\n";
    cout << "Invoice: "; cin >> id;

    vector<Transaction> hasil;
    for (auto t : data)
        if (t.transactionID == id) hasil.push_back(t);

    if (hasil.empty()) { cout << "Tidak ditemukan.\n"; return; }

    cout << "Ditemukan " << hasil.size() << " baris\n";
    printHeader();
    for (auto t : hasil) printRow(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH INVOICE: " + id + "\n");
    tulisLog("Ditemukan " + to_string(hasil.size()) + " baris\n");
    for (auto t : hasil) tulisLog(barisLog(t));
}

// MENU 4 - SEARCH BY CUSTOMER
void cariCustomer(vector<Transaction> data) {
    string id;
    cout << "\n--- SEARCH BY CUSTOMER ID ---\n";
    cout << "Customer ID: "; cin >> id;

    vector<Transaction> hasil;
    for (auto t : data)
        if (t.customerID == id) hasil.push_back(t);

    if (hasil.empty()) { cout << "Tidak ditemukan.\n"; return; }

    cout << "Ditemukan " << hasil.size() << " transaksi\n";
    printHeader();
    for (auto t : hasil) printRow(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH CUSTOMER: " + id + "\n");
    tulisLog("Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    for (auto t : hasil) tulisLog(barisLog(t));
}

// MENU 5 - SEARCH BY PRODUCT
void cariProduct(vector<Transaction> data) {
    string id;
    cout << "\n--- SEARCH BY PRODUCT ID ---\n";
    cout << "Product ID: "; cin >> id;

    vector<Transaction> hasil;
    for (auto t : data)
        if (t.productID == id) hasil.push_back(t);

    if (hasil.empty()) { cout << "Tidak ditemukan.\n"; return; }

    cout << "Ditemukan " << hasil.size() << " transaksi\n";
    printHeader();
    for (auto t : hasil) printRow(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH PRODUCT: " + id + "\n");
    tulisLog("Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    for (auto t : hasil) tulisLog(barisLog(t));
}

// MENU 6 - SEARCH BY CATEGORY
void cariCategory(vector<Transaction> data) {
    cout << "\n--- SEARCH BY CATEGORY ---\n";

    set<string> cats;
    for (auto t : data) cats.insert(t.category);
    vector<string> catList(cats.begin(), cats.end());

    cout << "Kategori:\n";
    for (int i = 0; i < (int)catList.size(); i++)
        cout << "  " << i+1 << ". " << catList[i] << "\n";
    cout << "Pilih: ";
    int pilih; cin >> pilih;

    if (pilih < 1 || pilih > (int)catList.size()) { cout << "Pilihan tidak valid.\n"; return; }

    string cat = catList[pilih - 1];

    vector<Transaction> hasil;
    for (auto t : data)
        if (t.category == cat) hasil.push_back(t);

    cout << "Ditemukan " << hasil.size() << " transaksi di kategori: " << cat << "\n";
    printHeader();
    for (auto t : hasil) printRow(t);

    tulisLog("\n[" + waktuSekarang() + "] SEARCH CATEGORY: " + cat + "\n");
    tulisLog("Ditemukan " + to_string(hasil.size()) + " transaksi\n");
    for (auto t : hasil) tulisLog(barisLog(t));
}

// MENU 7 - INFO DATASET
void infoDataset(vector<Transaction> data) {
    set<string> invoices, customers, products, categories;
    for (auto t : data) {
        invoices.insert(t.transactionID);
        customers.insert(t.customerID);
        products.insert(t.productID);
        categories.insert(t.category);
    }

    cout << "\n--- INFO DATASET ---\n";
    cout << "Total baris   : " << data.size()      << "\n";
    cout << "Invoice unik  : " << invoices.size()  << "\n";
    cout << "Customer unik : " << customers.size() << "\n";
    cout << "Produk unik   : " << products.size()  << "\n";
    cout << "Kategori unik : " << categories.size()<< "\n";
    cout << "\nDaftar kategori:\n";
    for (auto c : categories) cout << "  - " << c << "\n";

    tulisLog("\n[" + waktuSekarang() + "] INFO DATASET\n");
    tulisLog("Total: " + to_string(data.size()) + " | Invoice: " + to_string(invoices.size()) +
             " | Customer: " + to_string(customers.size()) + " | Produk: " + to_string(products.size()) + "\n");
}

#endif