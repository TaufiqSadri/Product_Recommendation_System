#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "transactionVector.h"
#include "transactionHashTable.h"
#include "transactionAVL.h"

using namespace std;

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

string formatTransaction(const Transaction& t, int no) {
    ostringstream oss;
    oss << no << ". "
        << t.invoiceId   << " | "
        << t.stockCode   << " | "
        << t.description << " | Qty: "
        << t.quantity    << " | "
        << t.invoiceDate << " | Rp "
        << t.price       << " | Cust: "
        << t.customerId  << " | "
        << t.category;
    return oss.str();
}

void printOutputStatus(string outputFile, bool saved) {
    if (saved)
        cout << "[OK] Hasil disimpan ke " << outputFile << "\n";
    else
        cerr << "[ERROR] Gagal menulis file: " << outputFile << "\n";
}

void resetAllStructures() {
    resetVectorTransactions();
    resetHashTransactions();
    resetAVLTransactions();
}

struct CSVLoadResult {
    int count;
    double vectorMs;
    double hashMs;
    double avlMs;
};

int loadCSV(string filename, int limit = 0) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Tidak bisa membuka file: " << filename << "\n";
        return 0;
    }

    string line;
    getline(file, line); // skip header

    int count = 0;
    while (getline(file, line)) {
        if (limit > 0 && count >= limit) break;
        if (trim(line).empty()) continue;

        stringstream ss(line);
        string token;
        Transaction t;
        t.recordId = 0;

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

        assignTransactionRecordId(t);
        insertTransactionVector(t);
        insertTransactionHash(t);
        insertTransactionAVL(t);
        count++;
    }

    file.close();
    return count;
}

CSVLoadResult loadCSVMeasured(string filename, int limit = 0) {
    CSVLoadResult result;
    result.count = 0;
    result.vectorMs = 0.0;
    result.hashMs = 0.0;
    result.avlMs = 0.0;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Tidak bisa membuka file: " << filename << "\n";
        return result;
    }

    vector<Transaction> data;
    if (limit > 0) data.reserve(limit);

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (limit > 0 && (int)data.size() >= limit) break;
        if (trim(line).empty()) continue;

        stringstream ss(line);
        string token;
        Transaction t;
        t.recordId = 0;

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

        assignTransactionRecordId(t);
        data.push_back(t);
    }
    file.close();

    auto startVector = chrono::high_resolution_clock::now();
    for (Transaction t : data) insertTransactionVector(t);
    auto endVector = chrono::high_resolution_clock::now();
    result.vectorMs = chrono::duration_cast<chrono::nanoseconds>(endVector - startVector).count() / 1000000.0;

    auto startHash = chrono::high_resolution_clock::now();
    for (Transaction t : data) insertTransactionHash(t);
    auto endHash = chrono::high_resolution_clock::now();
    result.hashMs = chrono::duration_cast<chrono::nanoseconds>(endHash - startHash).count() / 1000000.0;

    auto startAVL = chrono::high_resolution_clock::now();
    for (Transaction t : data) insertTransactionAVL(t);
    auto endAVL = chrono::high_resolution_clock::now();
    result.avlMs = chrono::duration_cast<chrono::nanoseconds>(endAVL - startAVL).count() / 1000000.0;

    result.count = (int)data.size();
    return result;
}

void appendOutputBlock(string title, string content, string outputFile) {
    ofstream out(outputFile);
    if (out.is_open()) {
        out << "[Query] " << title << "\n";
        out << content;
        out << "\n";
        out.close();
        printOutputStatus(outputFile, true);
    } else {
        printOutputStatus(outputFile, false);
    }
}

int resolveOutputTotal(int totalTransactions) {
    if (totalTransactions >= 0)
        return totalTransactions;
    return getVectorTransactionCount();
}

void writeOutput(string title, const vector<Transaction>& hasil, double durasi, string outputFile,
                 int totalTransactions = -1) {
    ostringstream content;

    content << "Total  : " << resolveOutputTotal(totalTransactions) << " transaksi\n";
    content << "Hasil  : " << hasil.size() << " ditemukan\n";
    content << "Waktu  : " << fixed << setprecision(5) << durasi << " ms\n\n";

    if (hasil.empty()) {
        content << "Tidak ada data ditemukan.\n";
    } else {
        int no = 1;
        vector<Transaction>::const_iterator it;
        for (it = hasil.begin(); it != hasil.end(); ++it)
            content << formatTransaction(*it, no++) << "\n";
    }

    appendOutputBlock(title, content.str(), outputFile);
}

void writeOutputRaw(string title, string body, double durasi, string outputFile,
                    int totalTransactions = -1) {
    ostringstream content;

    content << "Total  : " << resolveOutputTotal(totalTransactions) << " transaksi\n";
    content << "Waktu  : " << fixed << setprecision(5) << durasi << " ms\n\n";
    content << body;

    appendOutputBlock(title, content.str(), outputFile);
}

void writeRecommendationOutput(string title, const vector<ProductSummary>& hasil, double durasi,
                               string outputFile, string scoreLabel = "Frekuensi",
                               string extraLabel = "Total Qty",
                               int totalTransactions = -1) {
    ostringstream content;

    content << "Total  : " << resolveOutputTotal(totalTransactions) << " transaksi\n";
    content << "Hasil  : " << hasil.size() << " produk\n";
    content << "Waktu  : " << fixed << setprecision(5) << durasi << " ms\n\n";

    if (hasil.empty()) {
        content << "Tidak ada rekomendasi ditemukan.\n";
    } else {
        int no = 1;
        for (ProductSummary item : hasil) {
            content << no++ << ". "
                    << item.stockCode << " | "
                    << item.description << " | "
                    << item.category << " | "
                    << scoreLabel << ": " << item.transactionCount;
            if (!extraLabel.empty()) {
                content << " | " << extraLabel << ": " << item.totalQuantity;
            }
            content << "\n";
        }
    }

    appendOutputBlock(title, content.str(), outputFile);
}

#endif
