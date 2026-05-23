#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "transaction.h"

using namespace std;

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

string formatTransaction(Transaction t, int no) {
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

void writeOutput(string title, vector<Transaction> hasil, long long durasi, string outputFile) {
    ofstream out(outputFile, ios::trunc);
    ostringstream content;

    content << "Query  : " << title << "\n";
    content << "Total  : " << transactions.size() << " transaksi\n";
    content << "Hasil  : " << hasil.size() << " ditemukan\n";
    content << "Waktu  : " << durasi << " ms\n\n";

    if (hasil.empty()) {
        content << "Tidak ada data ditemukan.\n";
    } else {
        int no = 1;
        vector<Transaction>::iterator it;
        for (it = hasil.begin(); it != hasil.end(); ++it)
            content << formatTransaction(*it, no++) << "\n";
    }

    cout << content.str();
    if (out.is_open()) {
        out << content.str();
        out.close();
    }
}

void writeOutputRaw(string title, string body, long long durasi, string outputFile) {
    ofstream out(outputFile, ios::trunc);
    ostringstream content;

    content << "Query  : " << title << "\n";
    content << "Total  : " << transactions.size() << " transaksi\n";
    content << "Waktu  : " << durasi << " ms\n\n";
    content << body;

    cout << content.str();
    if (out.is_open()) {
        out << content.str();
        out.close();
    }
}

#endif