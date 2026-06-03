#ifndef TRANSACTION_VECTOR_H
#define TRANSACTION_VECTOR_H

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

struct Transaction {
    int recordId;
    string invoiceId;
    string stockCode;
    string description;
    int    quantity;
    string invoiceDate;
    double price;
    string customerId;
    string category;
};

struct ProductSummary {
    string stockCode;
    string description;
    string category;
    int totalQuantity;
    int transactionCount;
};

vector<Transaction> vectorTransactions;
int nextTransactionRecordId = 1;

int assignTransactionRecordId(Transaction& t) {
    if (t.recordId <= 0) {
        t.recordId = nextTransactionRecordId++;
    } else if (t.recordId >= nextTransactionRecordId) {
        nextTransactionRecordId = t.recordId + 1;
    }
    return t.recordId;
}

void reserveVectorTransactions(int capacity) {
    vectorTransactions.reserve(capacity);
}

int getVectorTransactionCount() {
    return (int)vectorTransactions.size();
}

size_t estimateTransactionMemory(const Transaction& t) {
    size_t memory = sizeof(Transaction);
    memory += t.invoiceId.capacity() + 1;
    memory += t.stockCode.capacity() + 1;
    memory += t.description.capacity() + 1;
    memory += t.invoiceDate.capacity() + 1;
    memory += t.customerId.capacity() + 1;
    memory += t.category.capacity() + 1;
    return memory;
}

void resetVectorTransactions() {
    vectorTransactions.clear();
    vectorTransactions.shrink_to_fit();
    nextTransactionRecordId = 1;
}

size_t estimateVectorMemory() {
    size_t memory = sizeof(vectorTransactions);
    memory += vectorTransactions.capacity() * sizeof(Transaction);

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        memory += estimateTransactionMemory(vectorTransactions[i]) - sizeof(Transaction);
    }

    return memory;
}

void insertTransactionVector(Transaction t) {
    assignTransactionRecordId(t);
    vectorTransactions.push_back(t);
}

vector<Transaction> getAllTransactionsVector() {
    return vectorTransactions;
}

int findIndexByRecordIdVector(int recordId) {
    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].recordId == recordId)
            return i;
    }
    return -1;
}

bool getTransactionByRecordIdVector(int recordId, Transaction& result) {
    int pos = findIndexByRecordIdVector(recordId);
    if (pos == -1) return false;

    result = vectorTransactions[pos];
    return true;
}

int findIndexByInvoiceIdVector(string id) {
    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].invoiceId == id)
            return i;
    }
    return -1;
}

int findRecordIdByInvoiceIdVector(string id) {
    int pos = findIndexByInvoiceIdVector(id);
    if (pos == -1) return -1;
    return vectorTransactions[pos].recordId;
}

vector<Transaction> searchByInvoiceIdVector(string id) {
    vector<Transaction> hasil;
    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].invoiceId == id)
            hasil.push_back(vectorTransactions[i]);
    }
    return hasil;
}

vector<Transaction> searchByCustomerIdVector(string id) {
    vector<Transaction> hasil;
    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].customerId == id)
            hasil.push_back(vectorTransactions[i]);
    }
    return hasil;
}

vector<Transaction> searchByStockCodeVector(string code) {
    vector<Transaction> hasil;
    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].stockCode == code)
            hasil.push_back(vectorTransactions[i]);
    }
    return hasil;
}

bool updateByRecordIdVector(int recordId, Transaction updated) {
    int pos = findIndexByRecordIdVector(recordId);
    if (pos == -1) return false;

    updated.recordId = recordId;
    vectorTransactions[pos] = updated;
    return true;
}

bool deleteByRecordIdVector(int recordId) {
    int pos = findIndexByRecordIdVector(recordId);
    if (pos == -1) return false;

    vectorTransactions.erase(vectorTransactions.begin() + pos);
    return true;
}

int findProductSummaryIndexVector(vector<ProductSummary>& items, string stockCode) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].stockCode == stockCode)
            return i;
    }
    return -1;
}

bool containsStringVector(const vector<string>& items, string value) {
    for (string item : items) {
        if (item == value)
            return true;
    }
    return false;
}

vector<ProductSummary> getTopNProductsVector(int n) {
    vector<ProductSummary> summary;

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        Transaction t = vectorTransactions[i];
        if (t.stockCode.empty()) continue;

        int pos = findProductSummaryIndexVector(summary, t.stockCode);
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
        if (a.transactionCount == b.transactionCount)
            return a.totalQuantity > b.totalQuantity;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)summary.size())
        summary.resize(n);

    return summary;
}

vector<ProductSummary> getFrequentlyBoughtTogetherVector(string targetStockCode, int n) {
    vector<string> relatedInvoices;
    vector<ProductSummary> productInfo;

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        if (vectorTransactions[i].stockCode == targetStockCode &&
            !containsStringVector(relatedInvoices, vectorTransactions[i].invoiceId)) {
            relatedInvoices.push_back(vectorTransactions[i].invoiceId);
        }
    }

    for (string invoiceId : relatedInvoices) {
        vector<string> countedInInvoice;

        for (int i = 0; i < (int)vectorTransactions.size(); i++) {
            Transaction t = vectorTransactions[i];
            if (t.invoiceId != invoiceId ||
                t.stockCode == targetStockCode ||
                containsStringVector(countedInInvoice, t.stockCode)) {
                continue;
            }

            int pos = findProductSummaryIndexVector(productInfo, t.stockCode);
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

vector<ProductSummary> getCustomerRecommendationsVector(string customerId, int n) {
    vector<string> purchasedProducts;
    vector<string> relatedInvoices;
    vector<Transaction> candidateTransactions;
    vector<ProductSummary> productInfo;

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        Transaction t = vectorTransactions[i];
        if (t.customerId == customerId) {
            purchasedProducts.push_back(t.stockCode);
        }
    }

    if (purchasedProducts.empty()) return productInfo;

    sort(purchasedProducts.begin(), purchasedProducts.end());
    purchasedProducts.erase(unique(purchasedProducts.begin(), purchasedProducts.end()), purchasedProducts.end());

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        Transaction t = vectorTransactions[i];
        if (binary_search(purchasedProducts.begin(), purchasedProducts.end(), t.stockCode)) {
            relatedInvoices.push_back(t.invoiceId);
        }
    }

    sort(relatedInvoices.begin(), relatedInvoices.end());
    relatedInvoices.erase(unique(relatedInvoices.begin(), relatedInvoices.end()), relatedInvoices.end());

    for (int i = 0; i < (int)vectorTransactions.size(); i++) {
        Transaction t = vectorTransactions[i];
        if (binary_search(relatedInvoices.begin(), relatedInvoices.end(), t.invoiceId) &&
            !binary_search(purchasedProducts.begin(), purchasedProducts.end(), t.stockCode)) {
            candidateTransactions.push_back(t);
        }
    }

    sort(candidateTransactions.begin(), candidateTransactions.end(), [](Transaction a, Transaction b) {
        if (a.invoiceId == b.invoiceId)
            return a.stockCode < b.stockCode;
        return a.invoiceId < b.invoiceId;
    });

    string lastInvoice = "";
    string lastStock = "";
    for (int i = 0; i < (int)candidateTransactions.size(); i++) {
        Transaction t = candidateTransactions[i];
        if (t.invoiceId == lastInvoice && t.stockCode == lastStock) continue;

        int pos = findProductSummaryIndexVector(productInfo, t.stockCode);
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

        lastInvoice = t.invoiceId;
        lastStock = t.stockCode;
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

#endif
