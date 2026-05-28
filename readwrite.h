#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "Transaction.h"

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

int loadCSV(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Tidak bisa membuka file: " << filename << "\n";
        return 0;
    }

    string line;
    getline(file, line); // skip header

    int count = 0;
    while (getline(file, line)) {
        //  if (count >= 10000) break;
        if (trim(line).empty()) continue;

        stringstream ss(line);
        string token;
        Transaction t;

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

        insertTransaction(t);
        count++;
    }

    file.close();
    return count;
}

double writeAllTransactionsOutput(string outputFile) {
    auto start = chrono::high_resolution_clock::now();

    ostringstream body;
    int no = 1;
    for (vector<Transaction>::iterator it = transactions.begin(); it != transactions.end(); ++it) {
        body << formatTransaction(*it, no++) << "\n";
    }

    auto end = chrono::high_resolution_clock::now();
    long long us = chrono::duration_cast<chrono::microseconds>(end - start).count();
    double durasi = us / 1000.0;

    ofstream out(outputFile, ios::trunc);
    if (out.is_open()) {
        out << "Query  : Semua Transaksi\n";
        out << "Total  : " << transactions.size() << " transaksi\n";
        out << "Waktu  : " << fixed << setprecision(3) << durasi << " ms\n\n";
        out << body.str();
        out.close();
        printOutputStatus(outputFile, true);
    } else {
        printOutputStatus(outputFile, false);
    }

    return durasi;
}

void writeOutput(string title, const vector<Transaction>& hasil, double durasi, string outputFile) {
    ofstream out(outputFile, ios::trunc);
    ostringstream content;

    content << "Query  : " << title << "\n";
    content << "Total  : " << transactions.size() << " transaksi\n";
    content << "Hasil  : " << hasil.size() << " ditemukan\n";
    content << "Waktu  : " << fixed << setprecision(3) << durasi << " ms\n\n";

    if (hasil.empty()) {
        content << "Tidak ada data ditemukan.\n";
    } else {
        int no = 1;
        vector<Transaction>::const_iterator it;
        for (it = hasil.begin(); it != hasil.end(); ++it)
            content << formatTransaction(*it, no++) << "\n";
    }

    if (out.is_open()) {
        out << content.str();
        out.close();
        printOutputStatus(outputFile, true);
    } else {
        printOutputStatus(outputFile, false);
    }
}

void writeOutputRaw(string title, string body, double durasi, string outputFile) {
    ofstream out(outputFile, ios::trunc);
    ostringstream content;

    content << "Query  : " << title << "\n";
    content << "Total  : " << transactions.size() << " transaksi\n";
    content << "Waktu  : " << fixed << setprecision(3) << durasi << " ms\n\n";
    content << body;

    if (out.is_open()) {
        out << content.str();
        out.close();
        printOutputStatus(outputFile, true);
    } else {
        printOutputStatus(outputFile, false);
    }
}

void writeRecommendationOutput(string title, const vector<ProductSummary>& hasil, double durasi,
                               string outputFile, string scoreLabel = "Frekuensi",
                               string extraLabel = "Total Qty") {
    ofstream out(outputFile, ios::trunc);
    ostringstream content;

    content << "Query  : " << title << "\n";
    content << "Total  : " << transactions.size() << " transaksi\n";
    content << "Hasil  : " << hasil.size() << " produk\n";
    content << "Waktu  : " << fixed << setprecision(3) << durasi << " ms\n\n";

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

    if (out.is_open()) {
        out << content.str();
        out.close();
        printOutputStatus(outputFile, true);
    } else {
        printOutputStatus(outputFile, false);
    }
}

#endif
