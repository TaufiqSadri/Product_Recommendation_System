#ifndef TRANSACTION_HASH_TABLE_H
#define TRANSACTION_HASH_TABLE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "transactionVector.h"

using namespace std;

unordered_map<int, Transaction> hashTransactions;
unordered_map<string, unordered_set<int> > hashInvoiceIndex;
unordered_map<string, unordered_set<int> > hashCustomerIndex;
unordered_map<string, unordered_set<int> > hashStockIndex;

void reserveHashTransactions(int capacity) {
    hashTransactions.reserve(capacity);
    hashInvoiceIndex.reserve(capacity / 4);
    hashCustomerIndex.reserve(capacity / 8);
    hashStockIndex.reserve(capacity / 8);
}

int getHashTransactionCount() {
    return (int)hashTransactions.size();
}

int getHashInvoiceIndexCount() {
    return (int)hashInvoiceIndex.size();
}

int getHashCustomerIndexCount() {
    return (int)hashCustomerIndex.size();
}

int getHashStockIndexCount() {
    return (int)hashStockIndex.size();
}

void resetHashTransactions() {
    hashTransactions.clear();
    hashInvoiceIndex.clear();
    hashCustomerIndex.clear();
    hashStockIndex.clear();

    hashTransactions.rehash(0);
    hashInvoiceIndex.rehash(0);
    hashCustomerIndex.rehash(0);
    hashStockIndex.rehash(0);
}

size_t estimateHashIndexMemory(unordered_map<string, unordered_set<int> >& index) {
    size_t memory = sizeof(index);
    memory += index.bucket_count() * sizeof(void*);

    for (unordered_map<string, unordered_set<int> >::iterator it = index.begin(); it != index.end(); ++it) {
        memory += sizeof(pair<const string, unordered_set<int> >);
        memory += it->first.capacity() + 1;
        memory += it->second.bucket_count() * sizeof(void*);
        memory += it->second.size() * (sizeof(int) + sizeof(void*));
    }

    return memory;
}

size_t estimateHashMemory() {
    size_t memory = sizeof(hashTransactions);
    memory += hashTransactions.bucket_count() * sizeof(void*);

    for (unordered_map<int, Transaction>::iterator it = hashTransactions.begin(); it != hashTransactions.end(); ++it) {
        memory += sizeof(pair<const int, Transaction>);
        memory += estimateTransactionMemory(it->second) - sizeof(Transaction);
    }

    memory += estimateHashIndexMemory(hashInvoiceIndex);
    memory += estimateHashIndexMemory(hashCustomerIndex);
    memory += estimateHashIndexMemory(hashStockIndex);
    return memory;
}

void addToHashIndexes(const Transaction& t) {
    hashInvoiceIndex[t.invoiceId].insert(t.recordId);
    hashCustomerIndex[t.customerId].insert(t.recordId);
    hashStockIndex[t.stockCode].insert(t.recordId);
}

void removeFromHashIndex(unordered_map<string, unordered_set<int> >& index, string key, int recordId) {
    unordered_map<string, unordered_set<int> >::iterator found = index.find(key);
    if (found == index.end()) return;

    found->second.erase(recordId);
    if (found->second.empty())
        index.erase(found);
}

void removeFromHashIndexes(const Transaction& t) {
    removeFromHashIndex(hashInvoiceIndex, t.invoiceId, t.recordId);
    removeFromHashIndex(hashCustomerIndex, t.customerId, t.recordId);
    removeFromHashIndex(hashStockIndex, t.stockCode, t.recordId);
}

void insertTransactionHash(Transaction t) {
    assignTransactionRecordId(t);
    hashTransactions[t.recordId] = t;
    addToHashIndexes(t);
}

bool getTransactionByRecordIdHash(int recordId, Transaction& result) {
    unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
    if (found == hashTransactions.end()) return false;

    result = found->second;
    return true;
}

vector<int> getSortedRecordIds(const unordered_set<int>& ids) {
    vector<int> sortedIds;
    for (int recordId : ids) {
        if (hashTransactions.find(recordId) != hashTransactions.end())
            sortedIds.push_back(recordId);
    }
    sort(sortedIds.begin(), sortedIds.end());
    return sortedIds;
}

int findSmallestRecordId(const unordered_set<int>& ids) {
    int result = -1;
    for (int recordId : ids) {
        if (hashTransactions.find(recordId) == hashTransactions.end()) continue;
        if (result == -1 || recordId < result)
            result = recordId;
    }
    return result;
}

int findRecordIdByInvoiceIdHash(string id) {
    unordered_map<string, unordered_set<int> >::iterator found = hashInvoiceIndex.find(id);
    if (found == hashInvoiceIndex.end()) return -1;

    return findSmallestRecordId(found->second);
}

vector<Transaction> searchByInvoiceIdHash(string id) {
    vector<Transaction> hasil;
    unordered_map<string, unordered_set<int> >::iterator found = hashInvoiceIndex.find(id);
    if (found == hashInvoiceIndex.end()) return hasil;

    vector<int> sortedIds = getSortedRecordIds(found->second);
    for (int recordId : sortedIds)
        hasil.push_back(hashTransactions[recordId]);

    return hasil;
}

vector<Transaction> searchByCustomerIdHash(string id) {
    vector<Transaction> hasil;
    unordered_map<string, unordered_set<int> >::iterator found = hashCustomerIndex.find(id);
    if (found == hashCustomerIndex.end()) return hasil;

    vector<int> sortedIds = getSortedRecordIds(found->second);
    for (int recordId : sortedIds)
        hasil.push_back(hashTransactions[recordId]);

    return hasil;
}

vector<Transaction> searchByStockCodeHash(string code) {
    vector<Transaction> hasil;
    unordered_map<string, unordered_set<int> >::iterator found = hashStockIndex.find(code);
    if (found == hashStockIndex.end()) return hasil;

    vector<int> sortedIds = getSortedRecordIds(found->second);
    for (int recordId : sortedIds)
        hasil.push_back(hashTransactions[recordId]);

    return hasil;
}

bool updateByRecordIdHash(int recordId, Transaction updated) {
    unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
    if (found == hashTransactions.end()) return false;

    Transaction old = found->second;
    removeFromHashIndexes(old);

    updated.recordId = recordId;
    hashTransactions[recordId] = updated;
    addToHashIndexes(updated);
    return true;
}

bool deleteByRecordIdHash(int recordId) {
    unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
    if (found == hashTransactions.end()) return false;

    Transaction old = found->second;
    removeFromHashIndexes(old);
    hashTransactions.erase(found);
    return true;
}

vector<Transaction> getAllTransactionsHash() {
    vector<Transaction> result;
    vector<int> recordIds;

    for (unordered_map<int, Transaction>::iterator it = hashTransactions.begin(); it != hashTransactions.end(); ++it)
        recordIds.push_back(it->first);

    sort(recordIds.begin(), recordIds.end());
    for (int recordId : recordIds)
        result.push_back(hashTransactions[recordId]);

    return result;
}

vector<ProductSummary> getTopNProductsHash(int n) {
    unordered_map<string, ProductSummary> summary;

    for (unordered_map<int, Transaction>::iterator it = hashTransactions.begin(); it != hashTransactions.end(); ++it) {
        Transaction t = it->second;
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
    for (unordered_map<string, ProductSummary>::iterator it = summary.begin(); it != summary.end(); ++it)
        result.push_back(it->second);

    sort(result.begin(), result.end(), [](ProductSummary a, ProductSummary b) {
        if (a.transactionCount == b.transactionCount)
            return a.totalQuantity > b.totalQuantity;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)result.size())
        result.resize(n);

    return result;
}

vector<ProductSummary> getFrequentlyBoughtTogetherHash(string targetStockCode, int n) {
    unordered_map<string, int> togetherCount;
    unordered_map<string, ProductSummary> productInfo;

    unordered_map<string, unordered_set<int> >::iterator target = hashStockIndex.find(targetStockCode);
    if (target == hashStockIndex.end()) return vector<ProductSummary>();

    unordered_set<string> relatedInvoices;
    for (int recordId : target->second) {
        unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
        if (found != hashTransactions.end())
            relatedInvoices.insert(found->second.invoiceId);
    }

    for (string invoiceId : relatedInvoices) {
        unordered_map<string, unordered_set<int> >::iterator invoice = hashInvoiceIndex.find(invoiceId);
        if (invoice == hashInvoiceIndex.end()) continue;

        unordered_set<string> countedInInvoice;
        for (int recordId : invoice->second) {
            unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
            if (found == hashTransactions.end()) continue;

            Transaction t = found->second;
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
    for (unordered_map<string, ProductSummary>::iterator it = productInfo.begin(); it != productInfo.end(); ++it)
        result.push_back(it->second);

    sort(result.begin(), result.end(), [](ProductSummary a, ProductSummary b) {
        if (a.transactionCount == b.transactionCount)
            return a.stockCode < b.stockCode;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)result.size())
        result.resize(n);

    return result;
}

vector<ProductSummary> getCustomerRecommendationsHash(string customerId, int n) {
    unordered_map<string, unordered_set<int> >::iterator customer = hashCustomerIndex.find(customerId);
    vector<ProductSummary> result;
    if (customer == hashCustomerIndex.end()) return result;

    unordered_set<string> purchasedProducts;
    unordered_set<string> similarCustomers;
    unordered_map<string, ProductSummary> productInfo;

    for (int recordId : customer->second) {
        unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
        if (found != hashTransactions.end())
            purchasedProducts.insert(found->second.stockCode);
    }

    if (purchasedProducts.empty()) return result;

    for (string stockCode : purchasedProducts) {
        unordered_map<string, unordered_set<int> >::iterator stock = hashStockIndex.find(stockCode);
        if (stock == hashStockIndex.end()) continue;

        for (int recordId : stock->second) {
            unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
            if (found != hashTransactions.end() && found->second.customerId != customerId)
                similarCustomers.insert(found->second.customerId);
        }
    }

    unordered_set<string> countedInvoiceProduct;
    for (string similarCustomer : similarCustomers) {
        unordered_map<string, unordered_set<int> >::iterator customerRecords = hashCustomerIndex.find(similarCustomer);
        if (customerRecords == hashCustomerIndex.end()) continue;

        for (int recordId : customerRecords->second) {
            unordered_map<int, Transaction>::iterator found = hashTransactions.find(recordId);
            if (found == hashTransactions.end()) continue;

            Transaction t = found->second;
            string invoiceProductKey = t.invoiceId + "|" + t.stockCode;
            if (purchasedProducts.count(t.stockCode) || countedInvoiceProduct.count(invoiceProductKey)) continue;

            countedInvoiceProduct.insert(invoiceProductKey);

            if (productInfo.find(t.stockCode) == productInfo.end()) {
                ProductSummary item;
                item.stockCode = t.stockCode;
                item.description = t.description;
                item.category = t.category;
                item.totalQuantity = 1;
                item.transactionCount = 1;
                productInfo[t.stockCode] = item;
            } else {
                productInfo[t.stockCode].totalQuantity++;
                productInfo[t.stockCode].transactionCount++;
            }
        }
    }

    for (unordered_map<string, ProductSummary>::iterator it = productInfo.begin(); it != productInfo.end(); ++it)
        result.push_back(it->second);

    sort(result.begin(), result.end(), [](ProductSummary a, ProductSummary b) {
        if (a.transactionCount == b.transactionCount)
            return a.stockCode < b.stockCode;
        return a.transactionCount > b.transactionCount;
    });

    if (n > 0 && n < (int)result.size())
        result.resize(n);

    return result;
}

#endif
