#include "Transaction.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

using namespace std;

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
         << "Date\n";
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

void tampilHasil(vector<Transaction> hasil) {
    printHeader();
    for (auto t : hasil) printRow(t);
    cout << "Total: " << hasil.size() << " baris\n";
}

int main() {
    // reset log tiap run
    ofstream resetLog("output_log.txt", ios::trunc);
    resetLog << "=== LOG SISTEM REKOMENDASI PRODUK ===\n";
    resetLog << "[" << waktuSekarang() << "] PROGRAM DIMULAI\n";
    resetLog.close();

    auto t1 = chrono::high_resolution_clock::now();
    vector<Transaction> data = loadCSV("transactions.csv");
    auto t2 = chrono::high_resolution_clock::now();
    double loadTime = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Berhasil memuat " << data.size() << " baris (" << fixed << setprecision(2) << loadTime << " ms).\n";

    int pilih;
    do {
        cout << "\n========================================\n";
        cout << " SISTEM REKOMENDASI PRODUK - FASE 1\n";
        cout << " Struktur Data: Vector | E-Commerce\n";
        cout << "========================================\n";
        cout << " 1. Tampilkan Semua Transaksi\n";
        cout << " 2. Insert Transaksi Baru\n";
        cout << " 3. Search by Transaction ID\n";
        cout << " 4. Search by Customer ID\n";
        cout << " 5. Search by Product ID\n";
        cout << " 6. Search by Category\n";
        cout << " 7. Info Dataset\n";
        cout << "----------------------------------------\n";
        cout << " 0. Keluar\n";
        cout << "========================================\n";
        cout << "Pilih: ";

        if (!(cin >> pilih)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid.\n";
            continue;
        }

        switch (pilih) {
            case 1: {
                tampilHasil(data);
                string log = "\n[" + waktuSekarang() + "] TAMPIL SEMUA | total: " + to_string(data.size()) + " baris\n";
                log += string(110, '-') + "\n";
                log += "Invoice     | CustID     | ProductID   | Product Name                    | Category        | Qty | Price   | Date\n";
                log += string(110, '-') + "\n";
                for (auto t : data)
                    log += t.transactionID + " | " + t.customerID + " | " + t.productID + " | " +
                           t.productName + " | " + t.category + " | " +
                           to_string(t.quantity) + " | " + to_string(t.price) + " | " + t.date + "\n";
                tulisLog(log);
                break;
            }
            case 2: {
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

                insertData(data, t);
                cout << "Berhasil ditambahkan.\n";
                break;
            }
            case 3: {
                string id;
                cout << "\nInvoice: "; cin >> id;
                vector<Transaction> hasil = cariInvoice(data, id);
                if (hasil.empty()) { cout << "Tidak ditemukan.\n"; break; }
                tampilHasil(hasil);
                break;
            }
            case 4: {
                string id;
                cout << "\nCustomer ID: "; cin >> id;
                vector<Transaction> hasil = cariCustomer(data, id);
                if (hasil.empty()) { cout << "Tidak ditemukan.\n"; break; }
                tampilHasil(hasil);
                break;
            }
            case 5: {
                string id;
                cout << "\nProduct ID: "; cin >> id;
                vector<Transaction> hasil = cariProduct(data, id);
                if (hasil.empty()) { cout << "Tidak ditemukan.\n"; break; }
                tampilHasil(hasil);
                break;
            }
            case 6: {
                set<string> cats = getDaftarKategori(data);
                vector<string> catList(cats.begin(), cats.end());
                cout << "\nKategori:\n";
                for (int i = 0; i < (int)catList.size(); i++)
                    cout << "  " << i+1 << ". " << catList[i] << "\n";
                cout << "Pilih: ";
                int p; cin >> p;
                if (p < 1 || p > (int)catList.size()) { cout << "Pilihan tidak valid.\n"; break; }
                vector<Transaction> hasil = cariCategory(data, catList[p - 1]);
                if (hasil.empty()) { cout << "Tidak ditemukan.\n"; break; }
                tampilHasil(hasil);
                break;
            }
            case 7: {
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
                tulisLog("\n[" + waktuSekarang() + "] INFO DATASET | baris: " + to_string(data.size()) + "\n");
                break;
            }
            case 0: {
                tulisLog("\n[" + waktuSekarang() + "] PROGRAM SELESAI\n");
                cout << "Sampai jumpa!\n";
                break;
            }
            default:
                cout << "Pilihan tidak valid.\n";
        }

    } while (pilih != 0);

    return 0;
}