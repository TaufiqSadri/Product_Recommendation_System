#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <limits>
#include <iomanip>
#include "readwrite.h"

#define CSV_FILE    "transactions.csv"
#define OUTPUT_FILE "output.txt"

using namespace std;

chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}

double stopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    long long us = chrono::duration_cast<chrono::microseconds>(end - start).count();
    return us / 1000.0;
}

void printWaktu(double ms) {
    cout << "waktu = " << fixed << setprecision(3) << ms << " ms\n";
}

void printMenu() {
    cout << "\n=== SISTEM REKOMENDASI PRODUK - E-COMMERCE ===\n";
    cout << "[1] Simpan Semua Transaksi ke output.txt\n";
    cout << "[2] Insert Transaksi Baru\n";
    cout << "[3] Search Transaksi\n";
    cout << "[4] Update Transaksi\n";
    cout << "[5] Delete Transaksi\n";
    cout << "[6] Rekomendasi Top-N Produk Paling Sering Dibeli\n";
    cout << "[7] Rekomendasi Frequently Bought Together\n";
    cout << "[8] Rekomendasi Berdasarkan Customer ID\n";
    cout << "[9] Statistik Eksekusi\n";
    cout << "[0] Keluar\n";
    cout << "Pilih: ";
}

int inputNumber(string label, int defaultValue) {
    string input;
    cout << label;
    getline(cin, input);
    if (trim(input).empty()) return defaultValue;

    try { return stoi(trim(input)); }
    catch (...) { return defaultValue; }
}

string searchFieldName(int field) {
    if (field == 1) return "Invoice ID";
    if (field == 2) return "Customer ID";
    if (field == 3) return "Stock Code";
    return "Unknown";
}

int inputMethod(string title) {
    cout << "\n--- " << title << " ---\n";
    cout << "[1] Vector\n";
    cout << "[2] Hash Table\n";
    cout << "[3] Bandingkan Vector vs Hash Table\n";
    return inputNumber("Pilih metode: ", 0);
}

string statusText(bool ok) {
    return ok ? "Berhasil" : "Gagal";
}

vector<Transaction> searchVectorByField(int field, string key) {
    if (field == 1) return searchByInvoiceId(key);
    if (field == 2) return searchByCustomerId(key);
    if (field == 3) return searchByStockCode(key);
    return vector<Transaction>();
}

vector<Transaction> searchHashByField(int field, string key) {
    if (field == 1) return searchByInvoiceIdHash(key);
    if (field == 2) return searchByCustomerIdHash(key);
    if (field == 3) return searchByStockCodeHash(key);
    return vector<Transaction>();
}

void writeMethodComparison(string title, string metricName, string vectorResult, double vectorTime,
                           string hashResult, double hashTime, string note = "") {
    ostringstream body;
    body << "Metode          | " << metricName << " | Waktu\n";
    body << "Vector          | " << vectorResult << " | " << fixed << setprecision(3) << vectorTime << " ms\n";
    body << "Hash Table      | " << hashResult   << " | " << fixed << setprecision(3) << hashTime   << " ms\n";

    if (hashTime > 0) {
        body << "\nHash Table sekitar " << fixed << setprecision(2) << (vectorTime / hashTime)
             << "x dibanding Vector pada query ini.\n";
    }
    if (!note.empty()) {
        body << "\nCatatan: " << note << "\n";
    }

    writeOutputRaw("Perbandingan " + title, body.str(), vectorTime + hashTime, OUTPUT_FILE);
}

Transaction inputNewTransaction() {
    Transaction t;
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

    return t;
}

Transaction inputUpdatedTransaction(Transaction current) {
    cout << "\nData saat ini:\n";
    cout << "  Stock Code  : " << current.stockCode   << "\n";
    cout << "  Description : " << current.description << "\n";
    cout << "  Quantity    : " << current.quantity    << "\n";
    cout << "  Date        : " << current.invoiceDate << "\n";
    cout << "  Price       : " << current.price       << "\n";
    cout << "  Customer ID : " << current.customerId  << "\n";
    cout << "  Category    : " << current.category    << "\n";

    cout << "\nMasukkan data baru (kosongkan jika tidak ingin diubah):\n";

    Transaction updated = current;
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

    return updated;
}

void runShowMenu() {
    int method = inputMethod("SIMPAN SEMUA TRANSAKSI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    if (method == 1) {
        auto t = startTimer();
        vector<Transaction> hasil = getAllTransactionsVector();
        lastShowTime = stopTimer(t);
        writeOutput("Semua Transaksi - Vector", hasil, lastShowTime, OUTPUT_FILE);
        printWaktu(lastShowTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<Transaction> hasil = getAllTransactionsHash();
        lastHashShowTime = stopTimer(t);
        writeOutput("Semua Transaksi - Hash Table", hasil, lastHashShowTime, OUTPUT_FILE);
        printWaktu(lastHashShowTime);
        return;
    }

    auto tv = startTimer();
    vector<Transaction> hasilVector = getAllTransactionsVector();
    lastShowTime = stopTimer(tv);

    auto th = startTimer();
    vector<Transaction> hasilHash = getAllTransactionsHash();
    lastHashShowTime = stopTimer(th);

    writeMethodComparison("Simpan Semua Transaksi", "Jumlah",
                          to_string(hasilVector.size()) + " data", lastShowTime,
                          to_string(hasilHash.size()) + " data", lastHashShowTime,
                          "Mode banding hanya menyimpan ringkasan performa ke output.txt.");
    printWaktu(lastShowTime + lastHashShowTime);
}

void runInsertMenu() {
    int method = inputMethod("INSERT TRANSAKSI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    Transaction t = inputNewTransaction();

    double vectorTime = 0;
    double hashTime = 0;
    insertTransactionMeasured(t, vectorTime, hashTime);
    lastInsertTime = vectorTime;
    lastHashInsertTime = hashTime;

    ostringstream body;
    body << "Transaksi baru berhasil ditambahkan.\n";
    body << "Invoice ID  : " << t.invoiceId << "\n";
    body << "Stock Code  : " << t.stockCode << "\n";
    body << "Customer ID : " << t.customerId << "\n";
    body << "Total data  : " << transactions.size() << " transaksi\n";

    if (method == 1) {
        body << "\nMetode: Vector push_back.\n";
        writeOutputRaw("Insert Vector", body.str(), vectorTime, OUTPUT_FILE);
        printWaktu(vectorTime);
        return;
    }

    if (method == 2) {
        body << "\nMetode: Hash Table menambahkan index untuk data baru.\n";
        writeOutputRaw("Insert Hash Table", body.str(), hashTime, OUTPUT_FILE);
        printWaktu(hashTime);
        return;
    }

    writeMethodComparison("Insert Transaksi", "Status",
                          "1 data", vectorTime,
                          "1 index", hashTime,
                          "Data dimasukkan satu kali. Vector menyimpan data utama, Hash Table menyimpan index.");
    printWaktu(vectorTime + hashTime);
}

void runSearchMenu() {
    int method = inputMethod("SEARCH TRANSAKSI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    cout << "\nCari berdasarkan:\n";
    cout << "[1] Invoice ID\n";
    cout << "[2] Customer ID\n";
    cout << "[3] Stock Code\n";
    int field = inputNumber("Pilih field: ", 0);

    if (field < 1 || field > 3) {
        cout << "[ERROR] Pilihan field tidak valid.\n";
        return;
    }

    string key;
    cout << "Keyword: ";
    getline(cin, key);
    key = trim(key);

    if (key.empty()) {
        cout << "[ERROR] Keyword tidak boleh kosong.\n";
        return;
    }

    string label = searchFieldName(field) + ": " + key;

    if (method == 1) {
        auto t = startTimer();
        vector<Transaction> hasil = searchVectorByField(field, key);
        lastSearchTime = stopTimer(t);
        writeOutput("Search Vector - " + label, hasil, lastSearchTime, OUTPUT_FILE);
        printWaktu(lastSearchTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<Transaction> hasil = searchHashByField(field, key);
        lastHashSearchTime = stopTimer(t);
        writeOutput("Search Hash Table - " + label, hasil, lastHashSearchTime, OUTPUT_FILE);
        printWaktu(lastHashSearchTime);
        return;
    }

    auto tv = startTimer();
    vector<Transaction> hasilVector = searchVectorByField(field, key);
    lastSearchTime = stopTimer(tv);

    auto th = startTimer();
    vector<Transaction> hasilHash = searchHashByField(field, key);
    lastHashSearchTime = stopTimer(th);

    writeMethodComparison("Search - " + label, "Hasil",
                          to_string(hasilVector.size()) + " data", lastSearchTime,
                          to_string(hasilHash.size()) + " data", lastHashSearchTime);
    printWaktu(lastSearchTime + lastHashSearchTime);
}

void runUpdateMenu() {
    int method = inputMethod("UPDATE TRANSAKSI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string id;
    cout << "Invoice ID: ";
    getline(cin, id);
    id = trim(id);

    int currentPos = (method == 1) ? findIndexByInvoiceIdVector(id) : findIndexByInvoiceIdHash(id);
    if (currentPos == -1 && method == 3) currentPos = findIndexByInvoiceIdHash(id);
    if (currentPos == -1) currentPos = findIndexByInvoiceIdVector(id);

    if (currentPos == -1) {
        writeOutputRaw("Update Transaksi", "Invoice ID \"" + id + "\" tidak ditemukan.\n", 0, OUTPUT_FILE);
        return;
    }

    Transaction updated = inputUpdatedTransaction(transactions[currentPos]);

    if (method == 1) {
        auto t = startTimer();
        bool ok = updateByInvoiceIdVector(id, updated);
        lastUpdateTime = stopTimer(t);
        writeOutputRaw("Update Vector", "Status: " + statusText(ok) + "\n", lastUpdateTime, OUTPUT_FILE);
        printWaktu(lastUpdateTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        bool ok = updateByInvoiceIdHash(id, updated);
        lastHashUpdateTime = stopTimer(t);
        writeOutputRaw("Update Hash Table", "Status: " + statusText(ok) + "\n", lastHashUpdateTime, OUTPUT_FILE);
        printWaktu(lastHashUpdateTime);
        return;
    }

    auto tv = startTimer();
    int vectorPos = findIndexByInvoiceIdVector(id);
    lastUpdateTime = stopTimer(tv);

    auto th = startTimer();
    int hashPos = findIndexByInvoiceIdHash(id);
    lastHashUpdateTime = stopTimer(th);

    bool ok = updateAtIndex(hashPos != -1 ? hashPos : vectorPos, updated);
    writeMethodComparison("Update Invoice ID: " + id, "Status",
                          statusText(vectorPos != -1), lastUpdateTime,
                          statusText(hashPos != -1), lastHashUpdateTime,
                          "Update dilakukan satu kali setelah posisi data ditemukan.");
    cout << "[INFO] Update " << statusText(ok) << ".\n";
    printWaktu(lastUpdateTime + lastHashUpdateTime);
}

void runDeleteMenu() {
    int method = inputMethod("DELETE TRANSAKSI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string id;
    cout << "Invoice ID yang ingin dihapus: ";
    getline(cin, id);
    id = trim(id);

    if (method == 1) {
        auto t = startTimer();
        bool ok = deleteByInvoiceIdVector(id);
        lastDeleteTime = stopTimer(t);
        writeOutputRaw("Delete Vector", "Status: " + statusText(ok) + "\nTotal data: " + to_string(transactions.size()) + "\n", lastDeleteTime, OUTPUT_FILE);
        printWaktu(lastDeleteTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        bool ok = deleteByInvoiceIdHash(id);
        lastHashDeleteTime = stopTimer(t);
        writeOutputRaw("Delete Hash Table", "Status: " + statusText(ok) + "\nTotal data: " + to_string(transactions.size()) + "\n", lastHashDeleteTime, OUTPUT_FILE);
        printWaktu(lastHashDeleteTime);
        return;
    }

    auto tv = startTimer();
    int vectorPos = findIndexByInvoiceIdVector(id);
    lastDeleteTime = stopTimer(tv);

    auto th = startTimer();
    int hashPos = findIndexByInvoiceIdHash(id);
    lastHashDeleteTime = stopTimer(th);

    bool ok = deleteAtIndex(hashPos != -1 ? hashPos : vectorPos);
    writeMethodComparison("Delete Invoice ID: " + id, "Status",
                          statusText(vectorPos != -1), lastDeleteTime,
                          statusText(hashPos != -1), lastHashDeleteTime,
                          "Delete dilakukan satu kali. Setelah erase dari vector, Hash Table di-reindex karena posisi data bergeser.");
    cout << "[INFO] Delete " << statusText(ok) << ".\n";
    printWaktu(lastDeleteTime + lastHashDeleteTime);
}

void runTopNMenu() {
    int method = inputMethod("REKOMENDASI TOP-N PRODUK PALING SERING DIBELI");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getTopNProductsVector(n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Top-" + to_string(n) + " Produk Paling Sering Dibeli - Vector", hasil, lastRecommendationTime, OUTPUT_FILE, "Frekuensi Transaksi", "Total Qty");
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getTopNProductsHash(n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Top-" + to_string(n) + " Produk Paling Sering Dibeli - Hash Table", hasil, lastHashRecommendationTime, OUTPUT_FILE, "Frekuensi Transaksi", "Total Qty");
        printWaktu(lastHashRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getTopNProductsVector(n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getTopNProductsHash(n);
    lastHashRecommendationTime = stopTimer(th);

    writeMethodComparison("Top-" + to_string(n) + " Produk Paling Sering Dibeli", "Hasil",
                          to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                          to_string(hasilHash.size()) + " produk", lastHashRecommendationTime);
    printWaktu(lastRecommendationTime + lastHashRecommendationTime);
}

void runBoughtTogetherMenu() {
    int method = inputMethod("REKOMENDASI FREQUENTLY BOUGHT TOGETHER");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string stockCode;
    cout << "Stock Code acuan: ";
    getline(cin, stockCode);
    stockCode = trim(stockCode);

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getFrequentlyBoughtTogetherVector(stockCode, n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Frequently Bought Together - Vector untuk " + stockCode, hasil, lastRecommendationTime, OUTPUT_FILE, "Muncul Bersama", "");
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getFrequentlyBoughtTogetherHash(stockCode, n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Frequently Bought Together - Hash Table untuk " + stockCode, hasil, lastHashRecommendationTime, OUTPUT_FILE, "Muncul Bersama", "");
        printWaktu(lastHashRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getFrequentlyBoughtTogetherVector(stockCode, n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getFrequentlyBoughtTogetherHash(stockCode, n);
    lastHashRecommendationTime = stopTimer(th);

    writeMethodComparison("Frequently Bought Together untuk " + stockCode, "Hasil",
                          to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                          to_string(hasilHash.size()) + " produk", lastHashRecommendationTime);
    printWaktu(lastRecommendationTime + lastHashRecommendationTime);
}

void runCustomerRecommendationMenu() {
    int method = inputMethod("REKOMENDASI BERDASARKAN CUSTOMER ID");

    if (method < 1 || method > 3) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string customerId;
    cout << "Customer ID: ";
    getline(cin, customerId);
    customerId = trim(customerId);

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getCustomerRecommendationsVector(customerId, n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Rekomendasi Customer - Vector untuk " + customerId, hasil, lastRecommendationTime, OUTPUT_FILE, "Skor Rekomendasi", "");
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getCustomerRecommendationsHash(customerId, n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Rekomendasi Customer - Hash Table untuk " + customerId, hasil, lastHashRecommendationTime, OUTPUT_FILE, "Skor Rekomendasi", "");
        printWaktu(lastHashRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getCustomerRecommendationsVector(customerId, n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getCustomerRecommendationsHash(customerId, n);
    lastHashRecommendationTime = stopTimer(th);

    writeMethodComparison("Rekomendasi Customer ID: " + customerId, "Hasil",
                          to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                          to_string(hasilHash.size()) + " produk", lastHashRecommendationTime,
                          "Rekomendasi dibuat dari produk yang pernah dibeli customer, lalu mencari produk lain yang sering muncul bersama produk tersebut.");
    printWaktu(lastRecommendationTime + lastHashRecommendationTime);
}

int main() {
    transactions.reserve(120000);

    cout << "[LOADING] Membaca " << CSV_FILE << " ...\n";
    auto t0 = startTimer();
    int loaded = loadCSV(CSV_FILE);
    double durLoad = stopTimer(t0);

    if (loaded == 0) {
        cerr << "[ERROR] Gagal memuat data. Pastikan " << CSV_FILE << " ada di folder ini.\n";
        return 1;
    }

    cout << "Jumlah transaksi : " << loaded << "\n";
    cout << "Waktu load       : " << fixed << setprecision(3) << durLoad << " ms\n";
    cout << "Hash Table index : " << invoiceIndex.size() << " invoice, "
         << customerIndex.size() << " customer, "
         << stockIndex.size() << " stock code\n";

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
            runShowMenu();
            break;
        }
        case 2: {
            runInsertMenu();
            break;
        }
        case 3: {
            runSearchMenu();
            break;
        }
        case 4: {
            runUpdateMenu();
            break;
        }
        case 5: {
            runDeleteMenu();
            break;
        }
        case 6: {
            runTopNMenu();
            break;
        }
        case 7: {
            runBoughtTogetherMenu();
            break;
        }
        case 8: {
            runCustomerRecommendationMenu();
            break;
        }
        case 9: {
            cout << "\nTotal Data : " << transactions.size() << "\n\n";
            cout << "Execution Time Statistics (ms)\n";
            cout << "Insert Vector Time      : " << fixed << setprecision(3) << lastInsertTime << "\n";
            cout << "Insert Hash Time        : " << fixed << setprecision(3) << lastHashInsertTime << "\n";
            cout << "Search Vector Time      : " << fixed << setprecision(3) << lastSearchTime << "\n";
            cout << "Search Hash Time        : " << fixed << setprecision(3) << lastHashSearchTime << "\n";
            cout << "Update Vector Time      : " << fixed << setprecision(3) << lastUpdateTime << "\n";
            cout << "Update Hash Time        : " << fixed << setprecision(3) << lastHashUpdateTime << "\n";
            cout << "Delete Vector Time      : " << fixed << setprecision(3) << lastDeleteTime << "\n";
            cout << "Delete Hash Time        : " << fixed << setprecision(3) << lastHashDeleteTime << "\n";
            cout << "Show Vector Time        : " << fixed << setprecision(3) << lastShowTime << "\n";
            cout << "Show Hash Time          : " << fixed << setprecision(3) << lastHashShowTime << "\n";
            cout << "Recommendation Vector   : " << fixed << setprecision(3) << lastRecommendationTime << "\n";
            cout << "Recommendation Hash     : " << fixed << setprecision(3) << lastHashRecommendationTime << "\n";
            break;
        }
        case 0:
            cout << "Sampai jumpa!\n";
            break;
        default:
            cout << "[ERROR] Pilihan tidak valid. Masukkan 0-9.\n";
        }

    } while (pilih != 0);

    return 0;
}
