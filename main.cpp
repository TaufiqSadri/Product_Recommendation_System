#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <limits>
#include <iomanip>
#include <fstream>
#include "readwrite.h"

#define CSV_FILE    "transaction.csv"
#define OUTPUT_FILE "output.txt"
#define BENCHMARK_FILE "benchmark.txt"

using namespace std;

// Menyimpan waktu eksekusi tiap operasi (ms)
double lastInsertTime = 0;
double lastSearchTime = 0;
double lastUpdateTime = 0;
double lastDeleteTime = 0;
double lastShowTime   = 0;
double lastHashInsertTime = 0;
double lastHashSearchTime = 0;
double lastHashUpdateTime = 0;
double lastHashDeleteTime = 0;
double lastHashShowTime = 0;
double lastRecommendationTime = 0;
double lastHashRecommendationTime = 0;
double lastAVLInsertTime = 0;
double lastAVLSearchTime = 0;
double lastAVLUpdateTime = 0;
double lastAVLDeleteTime = 0;
double lastAVLShowTime = 0;
double lastAVLRecommendationTime = 0;

chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}

double stopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    long long ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    return ns / 1000000.0;
}

void printWaktu(double ms) {
    cout << "waktu = " << fixed << setprecision(5) << ms << " ms\n";
}

double memoryKB(size_t bytes) {
    return bytes / 1024.0;
}

string methodName(int method) {
    if (method == 1) return "Vector";
    if (method == 2) return "Hash Table";
    if (method == 3) return "AVL Tree";
    return "Bandingkan Semua";
}

double memoryForMethodKB(int method) {
    if (method == 1) return memoryKB(estimateVectorMemory());
    if (method == 2) return memoryKB(estimateHashMemory());
    if (method == 3) return memoryKB(estimateAVLMemory());
    return 0.0;
}

string benchmarkOperationName(int menuNo) {
    if (menuNo == 0) return "Load Data";
    if (menuNo == 1) return "Tampilkan Transaksi";
    if (menuNo == 2) return "Insert Transaksi Baru";
    if (menuNo == 3) return "Search Transaksi";
    if (menuNo == 4) return "Update Transaksi";
    if (menuNo == 5) return "Delete Transaksi";
    if (menuNo == 6) return "Rekomendasi Top-N Produk";
    if (menuNo == 7) return "Rekomendasi Frequently Bought Together";
    if (menuNo == 8) return "Rekomendasi Berdasarkan Customer ID";
    return "Unknown";
}

string benchmarkMenuText(int menuNo) {
    if (menuNo == 0) return "[Load]";
    return "[" + to_string(menuNo) + "]";
}

string benchmarkLine(int dataSize, int menuNo, string operation,
                     int method, double timeMs, double memoryValueKB) {
    ostringstream line;
    line << dataSize << ";"
         << benchmarkMenuText(menuNo) << ";"
         << operation << ";"
         << methodName(method) << ";"
         << fixed << setprecision(5) << timeMs << ";"
         << fixed << setprecision(3) << memoryValueKB;
    return line.str();
}

void prepareBenchmarkFile() {
    const string header = "Data;Menu;Operasi;StrukturData;Waktu_ms;Memori_KB";
    ifstream in(BENCHMARK_FILE);
    string firstLine;
    getline(in, firstLine);
    in.close();

    if (firstLine == header) return;

    ofstream out(BENCHMARK_FILE, ios::trunc);
    if (out.is_open()) {
        out << header << "\n";
        out.close();
    }
}

void resetBenchmarkFile(int dataSize) {
    ofstream out(BENCHMARK_FILE, ios::trunc);
    if (!out.is_open()) {
        cerr << "[ERROR] Gagal membuat " << BENCHMARK_FILE << "\n";
        return;
    }

    out << "Data;Menu;Operasi;StrukturData;Waktu_ms;Memori_KB\n";
    for (int menuNo = 0; menuNo <= 8; menuNo++) {
        for (int method = 1; method <= 3; method++) {
            out << benchmarkLine(dataSize, menuNo, benchmarkOperationName(menuNo),
                                 method, 0.0, 0.0) << "\n";
        }
    }
    out.close();
}

void appendBenchmarkStat(int dataSize, int menuNo, string operation, int method, double timeMs) {
    prepareBenchmarkFile();

    vector<string> rows;
    ifstream in(BENCHMARK_FILE);
    string line;
    while (getline(in, line)) {
        rows.push_back(line);
    }
    in.close();

    string menuText = benchmarkMenuText(menuNo);
    string structureText = methodName(method);
    string newLine = benchmarkLine(dataSize, menuNo, operation, method, timeMs, memoryForMethodKB(method));
    bool updated = false;

    for (int i = 1; i < (int)rows.size(); i++) {
        stringstream ss(rows[i]);
        string dataCol, menuCol, operationCol, structureCol;
        getline(ss, dataCol, ';');
        getline(ss, menuCol, ';');
        getline(ss, operationCol, ';');
        getline(ss, structureCol, ';');

        if (menuCol == menuText && structureCol == structureText) {
            rows[i] = newLine;
            updated = true;
            break;
        }
    }

    if (!updated) {
        rows.push_back(newLine);
    }

    ofstream out(BENCHMARK_FILE, ios::trunc);
    if (!out.is_open()) {
        cerr << "[ERROR] Gagal menulis " << BENCHMARK_FILE << "\n";
        return;
    }

    for (string row : rows) {
        out << row << "\n";
    }
    out.close();
}

void appendBenchmarkCompare(int dataSize, int menuNo, string operation,
                            double vectorTime, double hashTime, double avlTime) {
    appendBenchmarkStat(dataSize, menuNo, operation, 1, vectorTime);
    appendBenchmarkStat(dataSize, menuNo, operation, 2, hashTime);
    appendBenchmarkStat(dataSize, menuNo, operation, 3, avlTime);
}

void showBenchmarkStats() {
    prepareBenchmarkFile();

    ifstream in(BENCHMARK_FILE);
    if (!in.is_open()) {
        cout << "[ERROR] Tidak bisa membuka " << BENCHMARK_FILE << "\n";
        return;
    }

    cout << "\nStatistik Benchmark\n";
    string line;
    bool hasRows = false;
    while (getline(in, line)) {
        cout << line << "\n";
        if (line.find(';') != string::npos && line.find("Data;") != 0) {
            hasRows = true;
        }
    }

    if (!hasRows) {
        cout << "Belum ada statistik. Jalankan menu 1-8 terlebih dahulu.\n";
    }

    in.close();
}

void printMenu() {
    cout << "\nSistem Rekomendasi Produk\n";
    cout << "[1] Tampilkan Transaksi\n";
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
    cout << "\n" << title << "\n";
    cout << "[1] Vector\n";
    cout << "[2] Hash Table\n";
    cout << "[3] AVL Tree\n";
    cout << "[4] Bandingkan Semua\n";
    return inputNumber("Pilih metode: ", 0);
}

int inputDataLimit() {
    cout << "Pilih ukuran data yang digunakan\n";
    cout << "[1] 10.000 transaksi\n";
    cout << "[2] 50.000 transaksi\n";
    cout << "[3] 100.000 transaksi\n";

    int choice = inputNumber("Pilih data: ", 3);
    if (choice == 1) return 10000;
    if (choice == 2) return 50000;
    return 100000;
}

string statusText(bool ok) {
    return ok ? "Berhasil" : "Gagal";
}

vector<Transaction> searchVectorByField(int field, string key) {
    if (field == 1) return searchByInvoiceIdVector(key);
    if (field == 2) return searchByCustomerIdVector(key);
    if (field == 3) return searchByStockCodeVector(key);
    return vector<Transaction>();
}

vector<Transaction> searchHashByField(int field, string key) {
    if (field == 1) return searchByInvoiceIdHash(key);
    if (field == 2) return searchByCustomerIdHash(key);
    if (field == 3) return searchByStockCodeHash(key);
    return vector<Transaction>();
}

vector<Transaction> searchAVLByField(int field, string key) {
    if (field == 1) return searchByInvoiceIdAVL(key);
    if (field == 2) return searchByCustomerIdAVL(key);
    if (field == 3) return searchByStockCodeAVL(key);
    return vector<Transaction>();
}

void writeMethodComparison(string title, string metricName, string vectorResult, double vectorTime,
                           string hashResult, double hashTime, string note = "") {
    ostringstream body;
    body << "Metode          | " << metricName << " | Waktu\n";
    body << "Vector          | " << vectorResult << " | " << fixed << setprecision(5) << vectorTime << " ms\n";
    body << "Hash Table      | " << hashResult   << " | " << fixed << setprecision(5) << hashTime   << " ms\n";

    if (hashTime > 0) {
        body << "\nHash Table sekitar " << fixed << setprecision(2) << (vectorTime / hashTime)
             << "x dibanding Vector pada query ini.\n";
    }
    if (!note.empty()) {
        body << "\nCatatan: " << note << "\n";
    }

    writeOutputRaw("Perbandingan " + title, body.str(), vectorTime + hashTime, OUTPUT_FILE);
}

void writeAllMethodComparison(string title, string metricName,
                              string vectorResult, double vectorTime,
                              string hashResult, double hashTime,
                              string avlResult, double avlTime,
                              string note = "") {
    ostringstream body;
    body << "Metode          | " << metricName << " | Waktu\n";
    body << "Vector          | " << vectorResult << " | " << fixed << setprecision(5) << vectorTime << " ms\n";
    body << "Hash Table      | " << hashResult   << " | " << fixed << setprecision(5) << hashTime   << " ms\n";
    body << "AVL Tree        | " << avlResult    << " | " << fixed << setprecision(5) << avlTime    << " ms\n";

    if (!note.empty()) {
        body << "\nCatatan: " << note << "\n";
    }

    writeOutputRaw("Perbandingan " + title, body.str(), vectorTime + hashTime + avlTime, OUTPUT_FILE);
}

void insertTransactionMeasured(Transaction t, double& vectorTime, double& hashTime, double& avlTime) {
    assignTransactionRecordId(t);

    auto tv = startTimer();
    insertTransactionVector(t);
    vectorTime = stopTimer(tv);

    auto th = startTimer();
    insertTransactionHash(t);
    hashTime = stopTimer(th);

    auto ta = startTimer();
    insertTransactionAVL(t);
    avlTime = stopTimer(ta);
}

void printShowOutputInfo(int totalData) {
    cout << "[INFO] Ringkasan " << totalData << " transaksi dicatat di " << OUTPUT_FILE << "\n";
}

string showSummaryBody(string method, int totalData) {
    ostringstream body;
    body << "Operasi : Tampilkan Transaksi\n";
    body << "Metode  : " << method << "\n";
    body << "Jumlah  : " << totalData << " transaksi\n";
    body << "Catatan : Data lengkap tidak ditulis ke output.txt.\n";
    return body.str();
}

int findRecordIdForInvoice(string invoiceId, int method) {
    int recordId = -1;

    if (method == 2)
        recordId = findRecordIdByInvoiceIdHash(invoiceId);
    else if (method == 3)
        recordId = findRecordIdByInvoiceIdAVL(invoiceId);
    else
        recordId = findRecordIdByInvoiceIdVector(invoiceId);

    if (recordId == -1 && method == 4)
        recordId = findRecordIdByInvoiceIdHash(invoiceId);
    if (recordId == -1 && method == 4)
        recordId = findRecordIdByInvoiceIdAVL(invoiceId);
    if (recordId == -1)
        recordId = findRecordIdByInvoiceIdVector(invoiceId);

    return recordId;
}

bool getTransactionForInvoice(string invoiceId, int method, Transaction& current) {
    int recordId = findRecordIdForInvoice(invoiceId, method);
    if (recordId == -1) return false;

    if (getTransactionByRecordIdVector(recordId, current)) return true;
    if (getTransactionByRecordIdHash(recordId, current)) return true;
    return getTransactionByRecordIdAVL(recordId, current);
}

Transaction inputNewTransaction() {
    Transaction t;
    t.recordId = 0;
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
    int method = inputMethod("TAMPILKAN TRANSAKSI");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        vector<Transaction> hasil = getAllTransactionsVector();
        lastShowTime = stopTimer(t);
        writeOutputRaw("Tampilkan Transaksi - Vector", showSummaryBody("Vector", (int)hasil.size()), lastShowTime, OUTPUT_FILE);
        printShowOutputInfo((int)hasil.size());
        appendBenchmarkStat(dataSize, 1, "Tampilkan Transaksi", 1, lastShowTime);
        printWaktu(lastShowTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<Transaction> hasil = getAllTransactionsHash();
        lastHashShowTime = stopTimer(t);
        writeOutputRaw("Tampilkan Transaksi - Hash Table", showSummaryBody("Hash Table", (int)hasil.size()), lastHashShowTime, OUTPUT_FILE);
        printShowOutputInfo((int)hasil.size());
        appendBenchmarkStat(dataSize, 1, "Tampilkan Transaksi", 2, lastHashShowTime);
        printWaktu(lastHashShowTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        vector<Transaction> hasil = getAllTransactionsAVL();
        lastAVLShowTime = stopTimer(t);
        writeOutputRaw("Tampilkan Transaksi - AVL Tree", showSummaryBody("AVL Tree", (int)hasil.size()), lastAVLShowTime, OUTPUT_FILE);
        printShowOutputInfo((int)hasil.size());
        appendBenchmarkStat(dataSize, 1, "Tampilkan Transaksi", 3, lastAVLShowTime);
        printWaktu(lastAVLShowTime);
        return;
    }

    auto tv = startTimer();
    vector<Transaction> hasilVector = getAllTransactionsVector();
    lastShowTime = stopTimer(tv);

    auto th = startTimer();
    vector<Transaction> hasilHash = getAllTransactionsHash();
    lastHashShowTime = stopTimer(th);

    auto ta = startTimer();
    vector<Transaction> hasilAVL = getAllTransactionsAVL();
    lastAVLShowTime = stopTimer(ta);

    writeAllMethodComparison("Tampilkan Semua Transaksi", "Jumlah",
                             to_string(hasilVector.size()) + " data", lastShowTime,
                             to_string(hasilHash.size()) + " data", lastHashShowTime,
                             to_string(hasilAVL.size()) + " data", lastAVLShowTime,
                             "Data transaksi tidak ditampilkan di terminal. Ringkasan perbandingan dicatat di output.txt.");
    printShowOutputInfo((int)hasilVector.size());
    appendBenchmarkCompare(dataSize, 1, "Tampilkan Transaksi", lastShowTime, lastHashShowTime, lastAVLShowTime);
    printWaktu(lastShowTime + lastHashShowTime + lastAVLShowTime);
}

void runInsertMenu() {
    int method = inputMethod("INSERT TRANSAKSI");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    Transaction t = inputNewTransaction();

    int dataSize = getVectorTransactionCount();
    double vectorTime = 0;
    double hashTime = 0;
    double avlTime = 0;
    insertTransactionMeasured(t, vectorTime, hashTime, avlTime);
    lastInsertTime = vectorTime;
    lastHashInsertTime = hashTime;
    lastAVLInsertTime = avlTime;

    ostringstream body;
    body << "Transaksi baru berhasil ditambahkan.\n";
    body << "Invoice ID  : " << t.invoiceId << "\n";
    body << "Stock Code  : " << t.stockCode << "\n";
    body << "Customer ID : " << t.customerId << "\n";
    body << "Total data  : " << getVectorTransactionCount() << " transaksi\n";

    if (method == 1) {
        body << "\nMetode: Vector push_back.\n";
        writeOutputRaw("Insert Vector", body.str(), vectorTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 2, "Insert Transaksi Baru", 1, vectorTime);
        printWaktu(vectorTime);
        return;
    }

    if (method == 2) {
        body << "\nMetode: Hash Table menambahkan index untuk data baru.\n";
        writeOutputRaw("Insert Hash Table", body.str(), hashTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 2, "Insert Transaksi Baru", 2, hashTime);
        printWaktu(hashTime);
        return;
    }

    if (method == 3) {
        body << "\nMetode: AVL Tree insert dengan balancing.\n";
        writeOutputRaw("Insert AVL Tree", body.str(), avlTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 2, "Insert Transaksi Baru", 3, avlTime);
        printWaktu(avlTime);
        return;
    }

    writeAllMethodComparison("Insert Transaksi", "Status",
                             "1 data", vectorTime,
                             "1 data", hashTime,
                             "1 data", avlTime,
                             "Data disimpan pada tiga struktur terpisah: vector, hash table, dan AVL tree.");
    appendBenchmarkCompare(dataSize, 2, "Insert Transaksi Baru", vectorTime, hashTime, avlTime);
    printWaktu(vectorTime + hashTime + avlTime);
}

void runSearchMenu() {
    int method = inputMethod("SEARCH TRANSAKSI");

    if (method < 1 || method > 4) {
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
    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        vector<Transaction> hasil = searchVectorByField(field, key);
        lastSearchTime = stopTimer(t);
        writeOutput("Search Vector - " + label, hasil, lastSearchTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 3, "Search Transaksi", 1, lastSearchTime);
        printWaktu(lastSearchTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<Transaction> hasil = searchHashByField(field, key);
        lastHashSearchTime = stopTimer(t);
        writeOutput("Search Hash Table - " + label, hasil, lastHashSearchTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 3, "Search Transaksi", 2, lastHashSearchTime);
        printWaktu(lastHashSearchTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        vector<Transaction> hasil = searchAVLByField(field, key);
        lastAVLSearchTime = stopTimer(t);
        writeOutput("Search AVL Tree - " + label, hasil, lastAVLSearchTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 3, "Search Transaksi", 3, lastAVLSearchTime);
        printWaktu(lastAVLSearchTime);
        return;
    }

    auto tv = startTimer();
    vector<Transaction> hasilVector = searchVectorByField(field, key);
    lastSearchTime = stopTimer(tv);

    auto th = startTimer();
    vector<Transaction> hasilHash = searchHashByField(field, key);
    lastHashSearchTime = stopTimer(th);

    auto ta = startTimer();
    vector<Transaction> hasilAVL = searchAVLByField(field, key);
    lastAVLSearchTime = stopTimer(ta);

    writeAllMethodComparison("Search - " + label, "Hasil",
                             to_string(hasilVector.size()) + " data", lastSearchTime,
                             to_string(hasilHash.size()) + " data", lastHashSearchTime,
                             to_string(hasilAVL.size()) + " data", lastAVLSearchTime);
    appendBenchmarkCompare(dataSize, 3, "Search Transaksi", lastSearchTime, lastHashSearchTime, lastAVLSearchTime);
    printWaktu(lastSearchTime + lastHashSearchTime + lastAVLSearchTime);
}

void runUpdateMenu() {
    int method = inputMethod("UPDATE TRANSAKSI");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string id;
    cout << "Invoice ID: ";
    getline(cin, id);
    id = trim(id);

    Transaction current;
    if (!getTransactionForInvoice(id, method, current)) {
        writeOutputRaw("Update Transaksi", "Invoice ID \"" + id + "\" tidak ditemukan.\n", 0, OUTPUT_FILE);
        return;
    }

    Transaction updated = inputUpdatedTransaction(current);
    updated.recordId = current.recordId;
    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        bool ok = updateByRecordIdVector(current.recordId, updated);
        lastUpdateTime = stopTimer(t);
        if (ok) updateByRecordIdHash(current.recordId, updated);
        if (ok) updateByRecordIdAVL(current.recordId, updated);
        writeOutputRaw("Update Vector", "Status: " + statusText(ok) + "\n", lastUpdateTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 4, "Update Transaksi", 1, lastUpdateTime);
        printWaktu(lastUpdateTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        bool ok = updateByRecordIdHash(current.recordId, updated);
        lastHashUpdateTime = stopTimer(t);
        if (ok) updateByRecordIdVector(current.recordId, updated);
        if (ok) updateByRecordIdAVL(current.recordId, updated);
        writeOutputRaw("Update Hash Table", "Status: " + statusText(ok) + "\n", lastHashUpdateTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 4, "Update Transaksi", 2, lastHashUpdateTime);
        printWaktu(lastHashUpdateTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        bool ok = updateByRecordIdAVL(current.recordId, updated);
        lastAVLUpdateTime = stopTimer(t);
        if (ok) updateByRecordIdVector(current.recordId, updated);
        if (ok) updateByRecordIdHash(current.recordId, updated);
        writeOutputRaw("Update AVL Tree", "Status: " + statusText(ok) + "\n", lastAVLUpdateTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 4, "Update Transaksi", 3, lastAVLUpdateTime);
        printWaktu(lastAVLUpdateTime);
        return;
    }

    auto tv = startTimer();
    bool vectorOk = updateByRecordIdVector(current.recordId, updated);
    lastUpdateTime = stopTimer(tv);

    auto th = startTimer();
    bool hashOk = updateByRecordIdHash(current.recordId, updated);
    lastHashUpdateTime = stopTimer(th);

    auto ta = startTimer();
    bool avlOk = updateByRecordIdAVL(current.recordId, updated);
    lastAVLUpdateTime = stopTimer(ta);

    writeAllMethodComparison("Update Invoice ID: " + id, "Status",
                             statusText(vectorOk), lastUpdateTime,
                             statusText(hashOk), lastHashUpdateTime,
                             statusText(avlOk), lastAVLUpdateTime,
                             "Update dilakukan pada recordId yang sama di tiga struktur data.");
    appendBenchmarkCompare(dataSize, 4, "Update Transaksi", lastUpdateTime, lastHashUpdateTime, lastAVLUpdateTime);
    cout << "[INFO] Update " << statusText(vectorOk && hashOk && avlOk) << ".\n";
    printWaktu(lastUpdateTime + lastHashUpdateTime + lastAVLUpdateTime);
}

void runDeleteMenu() {
    int method = inputMethod("DELETE TRANSAKSI");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string id;
    cout << "Invoice ID yang ingin dihapus: ";
    getline(cin, id);
    id = trim(id);

    int recordId = findRecordIdForInvoice(id, method);
    if (recordId == -1) {
        writeOutputRaw("Delete Transaksi", "Invoice ID \"" + id + "\" tidak ditemukan.\n", 0, OUTPUT_FILE);
        return;
    }

    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        bool ok = deleteByRecordIdVector(recordId);
        lastDeleteTime = stopTimer(t);
        if (ok) deleteByRecordIdHash(recordId);
        if (ok) deleteByRecordIdAVL(recordId);
        writeOutputRaw("Delete Vector", "Status: " + statusText(ok) + "\nTotal data: " + to_string(getVectorTransactionCount()) + "\n", lastDeleteTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 5, "Delete Transaksi", 1, lastDeleteTime);
        printWaktu(lastDeleteTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        bool ok = deleteByRecordIdHash(recordId);
        lastHashDeleteTime = stopTimer(t);
        if (ok) deleteByRecordIdVector(recordId);
        if (ok) deleteByRecordIdAVL(recordId);
        writeOutputRaw("Delete Hash Table", "Status: " + statusText(ok) + "\nTotal data: " + to_string(getHashTransactionCount()) + "\n", lastHashDeleteTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 5, "Delete Transaksi", 2, lastHashDeleteTime);
        printWaktu(lastHashDeleteTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        bool ok = deleteByRecordIdAVL(recordId);
        lastAVLDeleteTime = stopTimer(t);
        if (ok) deleteByRecordIdVector(recordId);
        if (ok) deleteByRecordIdHash(recordId);
        writeOutputRaw("Delete AVL Tree", "Status: " + statusText(ok) + "\nTotal data: " + to_string(getAVLTransactionCount()) + "\n", lastAVLDeleteTime, OUTPUT_FILE);
        appendBenchmarkStat(dataSize, 5, "Delete Transaksi", 3, lastAVLDeleteTime);
        printWaktu(lastAVLDeleteTime);
        return;
    }

    auto tv = startTimer();
    bool vectorOk = deleteByRecordIdVector(recordId);
    lastDeleteTime = stopTimer(tv);

    auto th = startTimer();
    bool hashOk = deleteByRecordIdHash(recordId);
    lastHashDeleteTime = stopTimer(th);

    auto ta = startTimer();
    bool avlOk = deleteByRecordIdAVL(recordId);
    lastAVLDeleteTime = stopTimer(ta);

    writeAllMethodComparison("Delete Invoice ID: " + id, "Status",
                             statusText(vectorOk), lastDeleteTime,
                             statusText(hashOk), lastHashDeleteTime,
                             statusText(avlOk), lastAVLDeleteTime,
                             "Delete dilakukan pada recordId yang sama. Hash table dan AVL tidak memakai posisi vector.");
    appendBenchmarkCompare(dataSize, 5, "Delete Transaksi", lastDeleteTime, lastHashDeleteTime, lastAVLDeleteTime);
    cout << "[INFO] Delete " << statusText(vectorOk && hashOk && avlOk) << ".\n";
    printWaktu(lastDeleteTime + lastHashDeleteTime + lastAVLDeleteTime);
}

void runTopNMenu() {
    int method = inputMethod("REKOMENDASI TOP-N PRODUK PALING SERING DIBELI");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;
    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getTopNProductsVector(n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Top-" + to_string(n) + " Produk Paling Sering Dibeli - Vector", hasil, lastRecommendationTime, OUTPUT_FILE, "Frekuensi Transaksi", "Total Qty");
        appendBenchmarkStat(dataSize, 6, "Rekomendasi Top-N Produk", 1, lastRecommendationTime);
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getTopNProductsHash(n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Top-" + to_string(n) + " Produk Paling Sering Dibeli - Hash Table", hasil, lastHashRecommendationTime, OUTPUT_FILE, "Frekuensi Transaksi", "Total Qty");
        appendBenchmarkStat(dataSize, 6, "Rekomendasi Top-N Produk", 2, lastHashRecommendationTime);
        printWaktu(lastHashRecommendationTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getTopNProductsAVL(n);
        lastAVLRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Top-" + to_string(n) + " Produk Paling Sering Dibeli - AVL Tree", hasil, lastAVLRecommendationTime, OUTPUT_FILE, "Frekuensi Transaksi", "Total Qty");
        appendBenchmarkStat(dataSize, 6, "Rekomendasi Top-N Produk", 3, lastAVLRecommendationTime);
        printWaktu(lastAVLRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getTopNProductsVector(n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getTopNProductsHash(n);
    lastHashRecommendationTime = stopTimer(th);

    auto ta = startTimer();
    vector<ProductSummary> hasilAVL = getTopNProductsAVL(n);
    lastAVLRecommendationTime = stopTimer(ta);

    writeAllMethodComparison("Top-" + to_string(n) + " Produk Paling Sering Dibeli", "Hasil",
                             to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                             to_string(hasilHash.size()) + " produk", lastHashRecommendationTime,
                             to_string(hasilAVL.size()) + " produk", lastAVLRecommendationTime);
    appendBenchmarkCompare(dataSize, 6, "Rekomendasi Top-N Produk", lastRecommendationTime, lastHashRecommendationTime, lastAVLRecommendationTime);
    printWaktu(lastRecommendationTime + lastHashRecommendationTime + lastAVLRecommendationTime);
}

void runBoughtTogetherMenu() {
    int method = inputMethod("REKOMENDASI FREQUENTLY BOUGHT TOGETHER");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string stockCode;
    cout << "Stock Code acuan: ";
    getline(cin, stockCode);
    stockCode = trim(stockCode);

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;
    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getFrequentlyBoughtTogetherVector(stockCode, n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Frequently Bought Together - Vector untuk " + stockCode, hasil, lastRecommendationTime, OUTPUT_FILE, "Muncul Bersama", "");
        appendBenchmarkStat(dataSize, 7, "Rekomendasi Frequently Bought Together", 1, lastRecommendationTime);
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getFrequentlyBoughtTogetherHash(stockCode, n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Frequently Bought Together - Hash Table untuk " + stockCode, hasil, lastHashRecommendationTime, OUTPUT_FILE, "Muncul Bersama", "");
        appendBenchmarkStat(dataSize, 7, "Rekomendasi Frequently Bought Together", 2, lastHashRecommendationTime);
        printWaktu(lastHashRecommendationTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getFrequentlyBoughtTogetherAVL(stockCode, n);
        lastAVLRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Frequently Bought Together - AVL Tree untuk " + stockCode, hasil, lastAVLRecommendationTime, OUTPUT_FILE, "Muncul Bersama", "");
        appendBenchmarkStat(dataSize, 7, "Rekomendasi Frequently Bought Together", 3, lastAVLRecommendationTime);
        printWaktu(lastAVLRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getFrequentlyBoughtTogetherVector(stockCode, n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getFrequentlyBoughtTogetherHash(stockCode, n);
    lastHashRecommendationTime = stopTimer(th);

    auto ta = startTimer();
    vector<ProductSummary> hasilAVL = getFrequentlyBoughtTogetherAVL(stockCode, n);
    lastAVLRecommendationTime = stopTimer(ta);

    writeAllMethodComparison("Frequently Bought Together untuk " + stockCode, "Hasil",
                             to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                             to_string(hasilHash.size()) + " produk", lastHashRecommendationTime,
                             to_string(hasilAVL.size()) + " produk", lastAVLRecommendationTime);
    appendBenchmarkCompare(dataSize, 7, "Rekomendasi Frequently Bought Together", lastRecommendationTime, lastHashRecommendationTime, lastAVLRecommendationTime);
    printWaktu(lastRecommendationTime + lastHashRecommendationTime + lastAVLRecommendationTime);
}

void runCustomerRecommendationMenu() {
    int method = inputMethod("REKOMENDASI BERDASARKAN CUSTOMER ID");

    if (method < 1 || method > 4) {
        cout << "[ERROR] Pilihan metode tidak valid.\n";
        return;
    }

    string customerId;
    cout << "Customer ID: ";
    getline(cin, customerId);
    customerId = trim(customerId);

    int n = inputNumber("Jumlah rekomendasi: ", 10);
    if (n <= 0) n = 10;
    int dataSize = getVectorTransactionCount();

    if (method == 1) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getCustomerRecommendationsVector(customerId, n);
        lastRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Rekomendasi Customer - Vector untuk " + customerId, hasil, lastRecommendationTime, OUTPUT_FILE, "Frekuensi di Transaksi Pelanggan Serupa", "");
        appendBenchmarkStat(dataSize, 8, "Rekomendasi Berdasarkan Customer ID", 1, lastRecommendationTime);
        printWaktu(lastRecommendationTime);
        return;
    }

    if (method == 2) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getCustomerRecommendationsHash(customerId, n);
        lastHashRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Rekomendasi Customer - Hash Table untuk " + customerId, hasil, lastHashRecommendationTime, OUTPUT_FILE, "Frekuensi di Transaksi Pelanggan Serupa", "");
        appendBenchmarkStat(dataSize, 8, "Rekomendasi Berdasarkan Customer ID", 2, lastHashRecommendationTime);
        printWaktu(lastHashRecommendationTime);
        return;
    }

    if (method == 3) {
        auto t = startTimer();
        vector<ProductSummary> hasil = getCustomerRecommendationsAVL(customerId, n);
        lastAVLRecommendationTime = stopTimer(t);
        writeRecommendationOutput("Rekomendasi Customer - AVL Tree untuk " + customerId, hasil, lastAVLRecommendationTime, OUTPUT_FILE, "Frekuensi di Transaksi Pelanggan Serupa", "");
        appendBenchmarkStat(dataSize, 8, "Rekomendasi Berdasarkan Customer ID", 3, lastAVLRecommendationTime);
        printWaktu(lastAVLRecommendationTime);
        return;
    }

    auto tv = startTimer();
    vector<ProductSummary> hasilVector = getCustomerRecommendationsVector(customerId, n);
    lastRecommendationTime = stopTimer(tv);

    auto th = startTimer();
    vector<ProductSummary> hasilHash = getCustomerRecommendationsHash(customerId, n);
    lastHashRecommendationTime = stopTimer(th);

    auto ta = startTimer();
    vector<ProductSummary> hasilAVL = getCustomerRecommendationsAVL(customerId, n);
    lastAVLRecommendationTime = stopTimer(ta);

    writeAllMethodComparison("Rekomendasi Customer ID: " + customerId, "Hasil",
                             to_string(hasilVector.size()) + " produk", lastRecommendationTime,
                             to_string(hasilHash.size()) + " produk", lastHashRecommendationTime,
                             to_string(hasilAVL.size()) + " produk", lastAVLRecommendationTime,
                             "Rekomendasi dibuat dari produk yang pernah dibeli customer, lalu mencari pelanggan lain yang membeli produk serupa dan menghitung produk lain yang sering muncul pada transaksi pelanggan tersebut.");
    appendBenchmarkCompare(dataSize, 8, "Rekomendasi Berdasarkan Customer ID", lastRecommendationTime, lastHashRecommendationTime, lastAVLRecommendationTime);
    printWaktu(lastRecommendationTime + lastHashRecommendationTime + lastAVLRecommendationTime);
}

int main() {
    int dataLimit = inputDataLimit();
    resetBenchmarkFile(dataLimit);

    reserveVectorTransactions(dataLimit + 10);
    reserveHashTransactions(dataLimit + 10);

    cout << "[LOADING] Membaca " << CSV_FILE << " ...\n";
    auto t0 = startTimer();
    CSVLoadResult loadResult = loadCSVMeasured(CSV_FILE, dataLimit);
    int loaded = loadResult.count;
    double durLoad = stopTimer(t0);

    if (loaded == 0) {
        cerr << "[ERROR] Gagal memuat data. Pastikan " << CSV_FILE << " ada di folder ini.\n";
        return 1;
    }

    appendBenchmarkCompare(loaded, 0, "Load Data",
                           loadResult.vectorMs, loadResult.hashMs, loadResult.avlMs);

    cout << "Jumlah transaksi : " << loaded << "\n";
    cout << "Ukuran data      : " << dataLimit << " transaksi\n";
    cout << "Vector storage   : " << getVectorTransactionCount() << " transaksi\n";
    cout << "Hash storage     : " << getHashTransactionCount() << " transaksi\n";
    cout << "AVL storage      : " << getAVLTransactionCount() << " transaksi\n";
    cout << "Waktu load total : " << fixed << setprecision(5) << durLoad << " ms\n";
    cout << "Load Vector      : " << fixed << setprecision(5) << loadResult.vectorMs << " ms\n";
    cout << "Load Hash Table  : " << fixed << setprecision(5) << loadResult.hashMs << " ms\n";
    cout << "Load AVL Tree    : " << fixed << setprecision(5) << loadResult.avlMs << " ms\n";
    cout << "Hash Table index : " << getHashInvoiceIndexCount() << " invoice, "
         << getHashCustomerIndexCount() << " customer, "
         << getHashStockIndexCount() << " stock code\n";
    cout << "AVL Tree index   : " << getAVLInvoiceIndexCount() << " invoice, "
         << getAVLCustomerIndexCount() << " customer, "
         << getAVLStockIndexCount() << " stock code\n";

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
            showBenchmarkStats();
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
