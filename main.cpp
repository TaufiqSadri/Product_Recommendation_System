#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <limits>
#include "readwrite.h"

#define CSV_FILE    "transactions.csv"
#define OUTPUT_FILE "output.txt"

using namespace std;

chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}

long long stopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

void printMenu() {
    cout << "\n=== SISTEM REKOMENDASI PRODUK - E-COMMERCE ===\n";
    cout << "[1] Tampilkan Semua Transaksi\n";
    cout << "[2] Insert Transaksi Baru\n";
    cout << "[3] Search by Invoice ID\n";
    cout << "[4] Search by Customer ID\n";
    cout << "[5] Search by Stock Code\n";
    cout << "[6] Update Transaksi\n";
    cout << "[7] Delete Transaksi\n";
    cout << "[8] Statistik Eksekusi\n";
    cout << "[0] Keluar\n";
    cout << "Pilih: ";
}

int main() {
    transactions.reserve(120000);

    cout << "[LOADING] Membaca " << CSV_FILE << " ...\n";
    auto t0 = startTimer();
    int loaded = loadCSV(CSV_FILE);
    long long durLoad = stopTimer(t0);

    if (loaded == 0) {
        cerr << "[ERROR] Gagal memuat data. Pastikan " << CSV_FILE << " ada di folder ini.\n";
        return 1;
    }

    cout << "Jumlah transaksi : " << loaded << "\n";
    cout << "Waktu load       : " << durLoad << " ms\n";

    int pilih = -1;
    do {
        printMenu();
        if (!(cin >> pilih)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Input tidak valid.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (pilih) {
        case 1: {
            auto t = startTimer();
            tampilSemuaTransaksi();
            lastShowTime = stopTimer(t);
            cout << "waktu = " << lastShowTime << " ms\n";
            break;
        }
        case 2: {
            Transaction t;
            cout << "\n--- INSERT TRANSAKSI ---\n";
            cout << "Invoice ID  : "; getline(cin, t.invoiceId);
            cout << "Stock Code  : "; getline(cin, t.stockCode);
            cout << "Description : "; getline(cin, t.description);
            cout << "Quantity    : ";
            string qty; getline(cin, qty);
            try { t.quantity = stoi(trim(qty)); } catch (...) { t.quantity = 0; }
            cout << "Date        : "; getline(cin, t.invoiceDate);
            cout << "Price       : ";
            string prc; getline(cin, prc);
            try { t.price = stod(trim(prc)); } catch (...) { t.price = 0.0; }
            cout << "Customer ID : "; getline(cin, t.customerId);
            cout << "Category    : "; getline(cin, t.category);

            t.invoiceId   = trim(t.invoiceId);
            t.stockCode   = trim(t.stockCode);
            t.description = trim(t.description);
            t.invoiceDate = trim(t.invoiceDate);
            t.customerId  = trim(t.customerId);
            t.category    = trim(t.category);

            auto ts = startTimer();
            insertTransaction(t);
            lastInsertTime = stopTimer(ts);
            cout << "[OK] Berhasil ditambahkan\n";
            cout << "waktu = " << lastInsertTime << " ms\n";
            cout << "[INFO] Total sekarang: " << transactions.size() << " transaksi (CSV tidak berubah)\n";
            break;
        }
        case 3: {
            string id;
            cout << "\nInvoice ID: "; getline(cin, id);
            auto t = startTimer();
            vector<Transaction> hasil = searchByInvoiceId(trim(id));
            lastSearchTime = stopTimer(t);
            writeOutput("Search Invoice: " + trim(id), hasil, lastSearchTime, OUTPUT_FILE);
            cout << "waktu = " << lastSearchTime << " ms\n";
            break;
        }
        case 4: {
            string id;
            cout << "\nCustomer ID: "; getline(cin, id);
            auto t = startTimer();
            vector<Transaction> hasil = searchByCustomerId(trim(id));
            lastSearchTime = stopTimer(t);
            writeOutput("Search Customer: " + trim(id), hasil, lastSearchTime, OUTPUT_FILE);
            cout << "waktu = " << lastSearchTime << " ms\n";
            break;
        }
        case 5: {
            string id;
            cout << "\nStock Code: "; getline(cin, id);
            auto t = startTimer();
            vector<Transaction> hasil = searchByStockCode(trim(id));
            lastSearchTime = stopTimer(t);
            writeOutput("Search Stock: " + trim(id), hasil, lastSearchTime, OUTPUT_FILE);
            cout << "waktu = " << lastSearchTime << " ms\n";
            break;
        }
        case 6: {
            string id;
            cout << "\n--- UPDATE TRANSAKSI ---\n";
            cout << "Invoice ID: "; getline(cin, id);
            id = trim(id);

            vector<Transaction> cek = searchByInvoiceId(id);
            if (cek.empty()) {
                cout << "[INFO] Invoice ID \"" << id << "\" tidak ditemukan.\n";
                break;
            }

            // Tampilkan data lama dulu
            cout << "\nData saat ini:\n";
            cout << "  Stock Code  : " << cek[0].stockCode   << "\n";
            cout << "  Description : " << cek[0].description << "\n";
            cout << "  Quantity    : " << cek[0].quantity    << "\n";
            cout << "  Date        : " << cek[0].invoiceDate << "\n";
            cout << "  Price       : " << cek[0].price       << "\n";
            cout << "  Customer ID : " << cek[0].customerId  << "\n";
            cout << "  Category    : " << cek[0].category    << "\n";

            cout << "\nMasukkan data baru (kosongkan jika tidak ingin diubah):\n";

            Transaction updated = cek[0]; // salin data lama dulu

            string input;
            cout << "Stock Code  : "; getline(cin, input);
            if (!trim(input).empty()) updated.stockCode = trim(input);

            cout << "Description : "; getline(cin, input);
            if (!trim(input).empty()) updated.description = trim(input);

            cout << "Quantity    : "; getline(cin, input);
            if (!trim(input).empty()) try { updated.quantity = stoi(trim(input)); } catch (...) {}

            cout << "Date        : "; getline(cin, input);
            if (!trim(input).empty()) updated.invoiceDate = trim(input);

            cout << "Price       : "; getline(cin, input);
            if (!trim(input).empty()) try { updated.price = stod(trim(input)); } catch (...) {}

            cout << "Customer ID : "; getline(cin, input);
            if (!trim(input).empty()) updated.customerId = trim(input);

            cout << "Category    : "; getline(cin, input);
            if (!trim(input).empty()) updated.category = trim(input);

            auto t = startTimer();
            bool ok = updateByInvoiceId(id, updated);
            lastUpdateTime = stopTimer(t);

            if (ok) cout << "[OK] Transaksi berhasil diupdate\n";
            else    cout << "[ERROR] Gagal mengupdate transaksi.\n";
            cout << "waktu = " << lastUpdateTime << " ms\n";
            break;
        }
        case 7: {
            string id;
            cout << "\n--- DELETE TRANSAKSI ---\n";
            cout << "Invoice ID yang ingin dihapus: "; getline(cin, id);
            id = trim(id);

            auto t = startTimer();
            bool ok = deleteByInvoiceId(id);
            lastDeleteTime = stopTimer(t);

            if (ok) {
                cout << "[OK] Transaksi \"" << id << "\" berhasil dihapus\n";
                cout << "[INFO] Total sekarang: " << transactions.size() << " transaksi\n";
            } else {
                cout << "[INFO] Invoice ID \"" << id << "\" tidak ditemukan.\n";
            }
            cout << "waktu = " << lastDeleteTime << " ms\n";
            break;
        }
        case 8: {
            cout << "\nTotal Data : " << transactions.size() << "\n\n";
            cout << "Execution Time Statistics (ms)\n";
            cout << "Insert Time : " << lastInsertTime << "\n";
            cout << "Search Time : " << lastSearchTime << "\n";
            cout << "Update Time : " << lastUpdateTime << "\n";
            cout << "Delete Time : " << lastDeleteTime << "\n";
            cout << "Show Time   : " << lastShowTime   << "\n";
            break;
        }
        case 0:
            cout << "Sampai jumpa!\n";
            break;
        default:
            cout << "[ERROR] Pilihan tidak valid. Masukkan 0-8.\n";
        }

    } while (pilih != 0);

    return 0;
}