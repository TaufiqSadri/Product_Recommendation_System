#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "readwrite.h"

#define CSV_FILE "transaction.csv"
#define BENCHMARK_FILE "benchmark.txt"

using namespace std;

struct BenchmarkRow {
    int dataSize;
    int menuNo;
    string operation;
    string structureName;
    double timeMs;
    double memoryKB;
};

chrono::high_resolution_clock::time_point benchmarkStartTimer() {
    return chrono::high_resolution_clock::now();
}

double benchmarkStopTimer(chrono::high_resolution_clock::time_point start) {
    auto end = chrono::high_resolution_clock::now();
    long long us = chrono::duration_cast<chrono::microseconds>(end - start).count();
    return us / 1000.0;
}

double benchmarkMemoryKB(size_t bytes) {
    return bytes / 1024.0;
}

vector<Transaction> readCSVSample(string filename, int limit) {
    vector<Transaction> data;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Tidak bisa membuka file: " << filename << "\n";
        return data;
    }

    string line;
    getline(file, line);

    int recordId = 1;
    while (getline(file, line)) {
        if ((int)data.size() >= limit) break;
        if (trim(line).empty()) continue;

        stringstream ss(line);
        string token;
        Transaction t;
        t.recordId = recordId++;

        if (!getline(ss, token, ';')) continue;
        t.invoiceId = trim(token);

        if (!getline(ss, token, ';')) continue;
        t.stockCode = trim(token);

        if (!getline(ss, token, ';')) continue;
        t.description = trim(token);

        if (!getline(ss, token, ';')) continue;
        try { t.quantity = stoi(trim(token)); } catch (...) { t.quantity = 0; }

        if (!getline(ss, token, ';')) continue;
        t.invoiceDate = trim(token);

        if (!getline(ss, token, ';')) continue;
        try { t.price = stod(trim(token)); } catch (...) { t.price = 0.0; }

        if (!getline(ss, token, ';')) continue;
        t.customerId = trim(token);

        if (!getline(ss, token, ';')) continue;
        t.category = trim(token);

        data.push_back(t);
    }

    file.close();
    return data;
}

void buildAllStructures(const vector<Transaction>& data) {
    resetAllStructures();
    reserveVectorTransactions((int)data.size() + 10);
    reserveHashTransactions((int)data.size() + 10);

    for (Transaction t : data) insertTransactionVector(t);
    for (Transaction t : data) insertTransactionHash(t);
    for (Transaction t : data) insertTransactionAVL(t);
}

Transaction makeBenchmarkTransaction(int dataSize) {
    Transaction t;
    t.recordId = dataSize + 1;
    t.invoiceId = "BENCH" + to_string(dataSize);
    t.stockCode = "BENCHSKU" + to_string(dataSize);
    t.description = "BENCHMARK TRANSACTION";
    t.quantity = 1;
    t.invoiceDate = "01/01/26 00.00";
    t.price = 1.0;
    t.customerId = "BENCHCUST" + to_string(dataSize);
    t.category = "Benchmark";
    return t;
}

void writeBenchmarkRow(ofstream& out, BenchmarkRow row) {
    out << row.dataSize << ";"
        << "[" << row.menuNo << "];"
        << row.operation << ";"
        << row.structureName << ";"
        << fixed << setprecision(3) << row.timeMs << ";"
        << fixed << setprecision(3) << row.memoryKB << "\n";
}

void writeVectorRow(ofstream& out, int dataSize, int menuNo, string operation, double timeMs) {
    writeBenchmarkRow(out, {dataSize, menuNo, operation, "Vector", timeMs, benchmarkMemoryKB(estimateVectorMemory())});
}

void writeHashRow(ofstream& out, int dataSize, int menuNo, string operation, double timeMs) {
    writeBenchmarkRow(out, {dataSize, menuNo, operation, "Hash Table", timeMs, benchmarkMemoryKB(estimateHashMemory())});
}

void writeAVLRow(ofstream& out, int dataSize, int menuNo, string operation, double timeMs) {
    writeBenchmarkRow(out, {dataSize, menuNo, operation, "AVL Tree", timeMs, benchmarkMemoryKB(estimateAVLMemory())});
}

void runBenchmarkForSize(int dataSize, ofstream& out) {
    vector<Transaction> data = readCSVSample(CSV_FILE, dataSize);
    if (data.empty()) {
        cout << "[ERROR] Data " << dataSize << " gagal dibaca.\n";
        return;
    }

    buildAllStructures(data);

    Transaction sample = data[data.size() / 2];
    Transaction insertData = makeBenchmarkTransaction(dataSize);

    auto start = benchmarkStartTimer();
    getAllTransactionsVector();
    double vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 1, "Tampilkan Transaksi", vectorTime);

    start = benchmarkStartTimer();
    getAllTransactionsHash();
    double hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 1, "Tampilkan Transaksi", hashTime);

    start = benchmarkStartTimer();
    getAllTransactionsAVL();
    double avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 1, "Tampilkan Transaksi", avlTime);

    start = benchmarkStartTimer();
    insertTransactionVector(insertData);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 2, "Insert Transaksi Baru", vectorTime);

    start = benchmarkStartTimer();
    insertTransactionHash(insertData);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 2, "Insert Transaksi Baru", hashTime);

    start = benchmarkStartTimer();
    insertTransactionAVL(insertData);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 2, "Insert Transaksi Baru", avlTime);

    start = benchmarkStartTimer();
    searchByCustomerIdVector(sample.customerId);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 3, "Search Transaksi", vectorTime);

    start = benchmarkStartTimer();
    searchByCustomerIdHash(sample.customerId);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 3, "Search Transaksi", hashTime);

    start = benchmarkStartTimer();
    searchByCustomerIdAVL(sample.customerId);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 3, "Search Transaksi", avlTime);

    Transaction updated = sample;
    updated.description = "UPDATED BY BENCHMARK";

    start = benchmarkStartTimer();
    updateByRecordIdVector(sample.recordId, updated);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 4, "Update Transaksi", vectorTime);

    start = benchmarkStartTimer();
    updateByRecordIdHash(sample.recordId, updated);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 4, "Update Transaksi", hashTime);

    start = benchmarkStartTimer();
    updateByRecordIdAVL(sample.recordId, updated);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 4, "Update Transaksi", avlTime);

    int deleteRecordId = data.back().recordId;

    start = benchmarkStartTimer();
    deleteByRecordIdVector(deleteRecordId);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 5, "Delete Transaksi", vectorTime);

    start = benchmarkStartTimer();
    deleteByRecordIdHash(deleteRecordId);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 5, "Delete Transaksi", hashTime);

    start = benchmarkStartTimer();
    deleteByRecordIdAVL(deleteRecordId);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 5, "Delete Transaksi", avlTime);

    start = benchmarkStartTimer();
    getTopNProductsVector(10);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 6, "Rekomendasi Top-N Produk", vectorTime);

    start = benchmarkStartTimer();
    getTopNProductsHash(10);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 6, "Rekomendasi Top-N Produk", hashTime);

    start = benchmarkStartTimer();
    getTopNProductsAVL(10);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 6, "Rekomendasi Top-N Produk", avlTime);

    start = benchmarkStartTimer();
    getFrequentlyBoughtTogetherVector(sample.stockCode, 10);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 7, "Rekomendasi Frequently Bought Together", vectorTime);

    start = benchmarkStartTimer();
    getFrequentlyBoughtTogetherHash(sample.stockCode, 10);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 7, "Rekomendasi Frequently Bought Together", hashTime);

    start = benchmarkStartTimer();
    getFrequentlyBoughtTogetherAVL(sample.stockCode, 10);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 7, "Rekomendasi Frequently Bought Together", avlTime);

    start = benchmarkStartTimer();
    getCustomerRecommendationsVector(sample.customerId, 10);
    vectorTime = benchmarkStopTimer(start);
    writeVectorRow(out, dataSize, 8, "Rekomendasi Berdasarkan Customer ID", vectorTime);

    start = benchmarkStartTimer();
    getCustomerRecommendationsHash(sample.customerId, 10);
    hashTime = benchmarkStopTimer(start);
    writeHashRow(out, dataSize, 8, "Rekomendasi Berdasarkan Customer ID", hashTime);

    start = benchmarkStartTimer();
    getCustomerRecommendationsAVL(sample.customerId, 10);
    avlTime = benchmarkStopTimer(start);
    writeAVLRow(out, dataSize, 8, "Rekomendasi Berdasarkan Customer ID", avlTime);
}

int main() {
    ofstream out(BENCHMARK_FILE, ios::trunc);
    if (!out.is_open()) {
        cerr << "[ERROR] Gagal membuat " << BENCHMARK_FILE << "\n";
        return 1;
    }

    out << "Data;Menu;Operasi;StrukturData;Waktu_ms;Memori_KB\n";

    int sizes[3] = {10000, 50000, 100000};
    for (int dataSize : sizes) {
        cout << "Menjalankan benchmark " << dataSize << " data...\n";
        runBenchmarkForSize(dataSize, out);
    }

    out.close();
    cout << "Benchmark selesai. Hasil tersimpan di " << BENCHMARK_FILE << "\n";
    return 0;
}
