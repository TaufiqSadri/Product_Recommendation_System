#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

struct Transaction {
    string invoiceId;
    string stockCode;
    string description;
    int    quantity;
    string invoiceDate;
    double price;
    string customerId;
    string category;
};

vector<Transaction> transactions;
unordered_map<string, vector<int> > invoiceIndex;
unordered_map<string, vector<int> > customerIndex;
unordered_map<string, vector<int> > stockIndex;

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

struct ProductSummary {
    string stockCode;
    string description;
    string category;
    int totalQuantity;
    int transactionCount;
};

void addToHashIndex(int pos) {
    if (pos < 0 || pos >= (int)transactions.size()) return;

    invoiceIndex[transactions[pos].invoiceId].push_back(pos);
    customerIndex[transactions[pos].customerId].push_back(pos);
    stockIndex[transactions[pos].stockCode].push_back(pos);
}

void removePositionFromIndex(unordered_map<string, vector<int> >& index, string key, int pos) {
    unordered_map<string, vector<int> >::iterator found = index.find(key);
    if (found == index.end()) return;

    vector<int>& positions = found->second;
    positions.erase(remove(positions.begin(), positions.end(), pos), positions.end());

    if (positions.empty()) {
        index.erase(found);
    }
}

void removeFromHashIndex(Transaction t, int pos) {
    removePositionFromIndex(invoiceIndex, t.invoiceId, pos);
    removePositionFromIndex(customerIndex, t.customerId, pos);
    removePositionFromIndex(stockIndex, t.stockCode, pos);
}

void buildHashIndexes() {
    invoiceIndex.clear();
    customerIndex.clear();
    stockIndex.clear();

    for (int i = 0; i < (int)transactions.size(); i++) {
        addToHashIndex(i);
    }
}

void insertTransaction(Transaction t) {
    transactions.push_back(t);
    addToHashIndex((int)transactions.size() - 1);
}

void insertTransactionMeasured(Transaction t, double& vectorTime, double& hashTime) {
    auto startVector = chrono::high_resolution_clock::now();
    transactions.push_back(t);
    auto endVector = chrono::high_resolution_clock::now();

    int pos = (int)transactions.size() - 1;
    auto startHash = chrono::high_resolution_clock::now();
    addToHashIndex(pos);
    auto endHash = chrono::high_resolution_clock::now();

    vectorTime = chrono::duration_cast<chrono::microseconds>(endVector - startVector).count() / 1000.0;
    hashTime = chrono::duration_cast<chrono::microseconds>(endHash - startHash).count() / 1000.0;
}

void tampilSemuaTransaksi() {
    vector<Transaction>::iterator it;
    int no = 1;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        cout << no++ << ". "
             << it->invoiceId   << " | "
             << it->stockCode   << " | "
             << it->description << " | Qty: "
             << it->quantity    << " | "
             << it->invoiceDate << " | Rp "
             << it->price       << " | Cust: "
             << it->customerId  << " | "
             << it->category    << "\n";
    }
}

vector<Transaction> searchByInvoiceId(string id) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->invoiceId == id)
            hasil.push_back(*it);
    }
    return hasil;
}

int findIndexByInvoiceIdVector(string id) {
    for (int i = 0; i < (int)transactions.size(); i++) {
        if (transactions[i].invoiceId == id)
            return i;
    }
    return -1;
}

vector<Transaction> searchByInvoiceIdHash(string id) {
    vector<Transaction> hasil;
    unordered_map<string, vector<int> >::iterator found = invoiceIndex.find(id);
    if (found == invoiceIndex.end()) return hasil;

    for (int pos : found->second) {
        if (pos >= 0 && pos < (int)transactions.size())
            hasil.push_back(transactions[pos]);
    }
    return hasil;
}

int findIndexByInvoiceIdHash(string id) {
    unordered_map<string, vector<int> >::iterator found = invoiceIndex.find(id);
    if (found == invoiceIndex.end() || found->second.empty()) return -1;

    int pos = found->second[0];
    if (pos < 0 || pos >= (int)transactions.size()) return -1;
    return pos;
}

vector<Transaction> searchByCustomerId(string id) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->customerId == id)
            hasil.push_back(*it);
    }
    return hasil;
}

vector<Transaction> searchByCustomerIdHash(string id) {
    vector<Transaction> hasil;
    unordered_map<string, vector<int> >::iterator found = customerIndex.find(id);
    if (found == customerIndex.end()) return hasil;

    for (int pos : found->second) {
        if (pos >= 0 && pos < (int)transactions.size())
            hasil.push_back(transactions[pos]);
    }
    return hasil;
}

vector<Transaction> searchByStockCode(string code) {
    vector<Transaction> hasil;
    vector<Transaction>::iterator it;
    for (it = transactions.begin(); it != transactions.end(); ++it) {
        if (it->stockCode == code)
            hasil.push_back(*it);
    }
    return hasil;
}

vector<Transaction> searchByStockCodeHash(string code) {
    vector<Transaction> hasil;
    unordered_map<string, vector<int> >::iterator found = stockIndex.find(code);
    if (found == stockIndex.end()) return hasil;

    for (int pos : found->second) {
        if (pos >= 0 && pos < (int)transactions.size())
            hasil.push_back(transactions[pos]);
    }
    return hasil;
}

bool updateAtIndex(int pos, Transaction updated) {
    if (pos < 0 || pos >= (int)transactions.size()) return false;

    Transaction old = transactions[pos];
    removeFromHashIndex(old, pos);
    transactions[pos] = updated;
    addToHashIndex(pos);
    return true;
}

bool updateByInvoiceIdVector(string id, Transaction updated) {
    int pos = findIndexByInvoiceIdVector(id);
    return updateAtIndex(pos, updated);
}

bool updateByInvoiceIdHash(string id, Transaction updated) {
    int pos = findIndexByInvoiceIdHash(id);
    return updateAtIndex(pos, updated);
}

bool updateByInvoiceId(string id, Transaction updated) {
    return updateByInvoiceIdVector(id, updated);
}

bool deleteAtIndex(int pos) {
    if (pos < 0 || pos >= (int)transactions.size()) return false;

    transactions.erase(transactions.begin() + pos);
    buildHashIndexes();
    return true;
}

bool deleteByInvoiceIdVector(string id) {
    int pos = findIndexByInvoiceIdVector(id);
    return deleteAtIndex(pos);
}

bool deleteByInvoiceIdHash(string id) {
    int pos = findIndexByInvoiceIdHash(id);
    return deleteAtIndex(pos);
}

bool deleteByInvoiceId(string id) {
    return deleteByInvoiceIdVector(id);
}

vector<Transaction> getAllTransactionsVector() {
    return transactions;
}

vector<Transaction> getAllTransactionsHash() {
    vector<Transaction> result;

    for (unordered_map<string, vector<int> >::iterator it = invoiceIndex.begin(); it != invoiceIndex.end(); ++it) {
        for (int pos : it->second) {
            if (pos >= 0 && pos < (int)transactions.size())
                result.push_back(transactions[pos]);
        }
    }

    return result;
}

int findProductSummaryIndex(vector<ProductSummary>& items, string stockCode) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].stockCode == stockCode)
            return i;
    }
    return -1;
}

bool containsString(vector<string>& items, string value) {
    for (string item : items) {
        if (item == value)
            return true;
    }
    return false;
}

vector<ProductSummary> getTopNProductsVector(int n) {
    vector<ProductSummary> summary;

    for (int i = 0; i < (int)transactions.size(); i++) {
        Transaction t = transactions[i];
        if (t.stockCode.empty()) continue;

        int pos = findProductSummaryIndex(summary, t.stockCode);
        if (pos == -1) {
            ProductSummary item;
            item.stockCode = t.stockCode;
            item.description = t.description;
            item.category = t.category;
            item.totalQuantity = t.quantity;
            item.transactionCount = 1;
            summary.push_back(item);
        } else {
            summary[pos].totalQuantity += t.quantity;
            summary[pos].transactionCount++;
        }
    }

    sort(summary.begin(), summary.end(), [](ProductSummary a, ProductSummary b) {
        if (a.totalQuantity == b.totalQuantity)
            return a.transactionCount > b.transactionCount;
        return a.totalQuantity > b.totalQuantity;
    });

    if (n > 0 && n < (int)summary.size())
        summary.resize(n);

    return summary;
}

vector<ProductSummary> getTopNProductsHash(int n) {
    unordered_map<string, ProductSummary> summary;

    for (int i = 0; i < (int)transactions.size(); i++) {
        Transaction t = transactions[i];
        if (t.stockCode.empty()) continue;

        if (summary.find(t.stockCode) == summary.end()) {
            ProductSummary item;
            item.stockCode = t.stockCode;
            item.description = t.description;
            item.category = t.category;
            item.totalQuantity = 0;
            item.transactionCount = 0;
            summary[t.stockCode] = item;
        }

        summary[t.stockCode].totalQuantity += t.quantity;
        summary[t.stockCode].transactionCount++;
    }

    vector<ProductSummary> result;
    for (unordered_map<string, ProductSummary>::iterator it = summary.begin(); it != summary.end(); ++it) {
        result.push_back(it->second);
    }

    sort(result.begin(), result.end(), [](ProductSummary a, ProductSummary b) {
        if (a.totalQuantity == b.totalQuantity)
            return a.transactionCount > b.transactionCount;
        return a.totalQuantity > b.totalQuantity;
    });

    if (n > 0 && n < (int)result.size())
        result.resize(n);

    return result;
}

vector<ProductSummary> getTopNProducts(int n) {
    return getTopNProductsHash(n);
}

vector<ProductSummary> getFrequentlyBoughtTogetherVector(string targetStockCode, int n) {
    vector<string> relatedInvoices;
    vector<ProductSummary> productInfo;

    for (int i = 0; i < (int)transactions.size(); i++) {
        if (transactions[i].stockCode == targetStockCode &&
            !containsString(relatedInvoices, transactions[i].invoiceId)) {
            relatedInvoices.push_back(transactions[i].invoiceId);
        }
    }

    for (string invoiceId : relatedInvoices) {
        vector<string> countedInInvoice;

        for (int i = 0; i < (int)transactions.size(); i++) {
            Transaction t = transactions[i];
            if (t.invoiceId != invoiceId ||
                t.stockCode == targetStockCode ||
                containsString(countedInInvoice, t.stockCode)) {
                continue;
            }

            int pos = findProductSummaryIndex(productInfo, t.stockCode);
            if (pos == -1) {
                ProductSummary item;
                item.stockCode = t.stockCode;
                item.description = t.description;
                item.category = t.category;
                item.totalQuantity = 1;
                item.transactionCount = 1;
                productInfo.push_back(item);
            } else {
                productInfo[pos].totalQuantity++;
                productInfo[pos].transactionCount++;
            }

            countedInInvoice.push_back(t.stockCode);
        }
    }

    sort(productInfo.begin(), productInfo.end(), [](ProductSummary a, ProductSummary b) {
        if (a.transactionCount == b.transactionCount)
            return a.stockCode < b.stockCode;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)productInfo.size())
        productInfo.resize(n);

    return productInfo;
}

vector<ProductSummary> getFrequentlyBoughtTogetherHash(string targetStockCode, int n) {
    unordered_map<string, int> togetherCount;
    unordered_map<string, ProductSummary> productInfo;

    unordered_map<string, vector<int> >::iterator target = stockIndex.find(targetStockCode);
    if (target == stockIndex.end()) return vector<ProductSummary>();

    unordered_set<string> relatedInvoices;
    for (int pos : target->second) {
        if (pos >= 0 && pos < (int)transactions.size())
            relatedInvoices.insert(transactions[pos].invoiceId);
    }

    for (string invoiceId : relatedInvoices) {
        unordered_map<string, vector<int> >::iterator invoice = invoiceIndex.find(invoiceId);
        if (invoice == invoiceIndex.end()) continue;

        unordered_set<string> countedInInvoice;
        for (int pos : invoice->second) {
            if (pos < 0 || pos >= (int)transactions.size()) continue;

            Transaction t = transactions[pos];
            if (t.stockCode == targetStockCode || countedInInvoice.count(t.stockCode)) continue;

            togetherCount[t.stockCode]++;
            countedInInvoice.insert(t.stockCode);

            if (productInfo.find(t.stockCode) == productInfo.end()) {
                ProductSummary item;
                item.stockCode = t.stockCode;
                item.description = t.description;
                item.category = t.category;
                item.totalQuantity = togetherCount[t.stockCode];
                item.transactionCount = togetherCount[t.stockCode];
                productInfo[t.stockCode] = item;
            } else {
                productInfo[t.stockCode].totalQuantity = togetherCount[t.stockCode];
                productInfo[t.stockCode].transactionCount = togetherCount[t.stockCode];
            }
        }
    }

    vector<ProductSummary> result;
    for (unordered_map<string, ProductSummary>::iterator it = productInfo.begin(); it != productInfo.end(); ++it) {
        result.push_back(it->second);
    }

    sort(result.begin(), result.end(), [](ProductSummary a, ProductSummary b) {
        if (a.transactionCount == b.transactionCount)
            return a.stockCode < b.stockCode;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)result.size())
        result.resize(n);

    return result;
}

vector<ProductSummary> getFrequentlyBoughtTogether(string targetStockCode, int n) {
    return getFrequentlyBoughtTogetherHash(targetStockCode, n);
}

#endif
