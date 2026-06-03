#ifndef TRANSACTION_AVL_H
#define TRANSACTION_AVL_H

#include <algorithm>
#include <string>
#include <vector>
#include "transactionVector.h"

using namespace std;

struct AVLTransactionNode {
    int key;
    Transaction data;
    int height;
    AVLTransactionNode* left;
    AVLTransactionNode* right;
};

struct AVLIndexNode {
    string key;
    vector<int> recordIds;
    int height;
    AVLIndexNode* left;
    AVLIndexNode* right;
};

AVLTransactionNode* avlTransactionRoot = NULL;
AVLIndexNode* avlInvoiceRoot = NULL;
AVLIndexNode* avlCustomerRoot = NULL;
AVLIndexNode* avlStockRoot = NULL;
int avlTransactionCount = 0;
int avlInvoiceIndexCount = 0;
int avlCustomerIndexCount = 0;
int avlStockIndexCount = 0;

int avlHeight(AVLTransactionNode* node) {
    return node == NULL ? 0 : node->height;
}

int avlHeight(AVLIndexNode* node) {
    return node == NULL ? 0 : node->height;
}

int avlBalance(AVLTransactionNode* node) {
    return node == NULL ? 0 : avlHeight(node->left) - avlHeight(node->right);
}

int avlBalance(AVLIndexNode* node) {
    return node == NULL ? 0 : avlHeight(node->left) - avlHeight(node->right);
}

AVLTransactionNode* newAVLTransactionNode(Transaction t) {
    AVLTransactionNode* node = new AVLTransactionNode;
    node->key = t.recordId;
    node->data = t;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

AVLIndexNode* newAVLIndexNode(string key, int recordId) {
    AVLIndexNode* node = new AVLIndexNode;
    node->key = key;
    node->recordIds.push_back(recordId);
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

AVLTransactionNode* rotateRight(AVLTransactionNode* y) {
    AVLTransactionNode* x = y->left;
    AVLTransactionNode* middle = x->right;

    x->right = y;
    y->left = middle;

    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;
    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;

    return x;
}

AVLTransactionNode* rotateLeft(AVLTransactionNode* x) {
    AVLTransactionNode* y = x->right;
    AVLTransactionNode* middle = y->left;

    y->left = x;
    x->right = middle;

    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;
    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;

    return y;
}

AVLIndexNode* rotateRight(AVLIndexNode* y) {
    AVLIndexNode* x = y->left;
    AVLIndexNode* middle = x->right;

    x->right = y;
    y->left = middle;

    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;
    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;

    return x;
}

AVLIndexNode* rotateLeft(AVLIndexNode* x) {
    AVLIndexNode* y = x->right;
    AVLIndexNode* middle = y->left;

    y->left = x;
    x->right = middle;

    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;
    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;

    return y;
}

AVLTransactionNode* rebalanceTransactionNode(AVLTransactionNode* node) {
    if (node == NULL) return node;

    node->height = max(avlHeight(node->left), avlHeight(node->right)) + 1;
    int balance = avlBalance(node);

    // Rotasi AVL sesuai kasus LL, LR, RR, RL.
    if (balance > 1 && avlBalance(node->left) >= 0)
        return rotateRight(node);
    if (balance > 1 && avlBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && avlBalance(node->right) <= 0)
        return rotateLeft(node);
    if (balance < -1 && avlBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

AVLIndexNode* rebalanceIndexNode(AVLIndexNode* node) {
    if (node == NULL) return node;

    node->height = max(avlHeight(node->left), avlHeight(node->right)) + 1;
    int balance = avlBalance(node);

    // Rotasi AVL sesuai kasus LL, LR, RR, RL.
    if (balance > 1 && avlBalance(node->left) >= 0)
        return rotateRight(node);
    if (balance > 1 && avlBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && avlBalance(node->right) <= 0)
        return rotateLeft(node);
    if (balance < -1 && avlBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

AVLTransactionNode* insertAVLTransactionNode(AVLTransactionNode* node, Transaction t, bool& inserted) {
    if (node == NULL) {
        inserted = true;
        return newAVLTransactionNode(t);
    }

    if (t.recordId < node->key)
        node->left = insertAVLTransactionNode(node->left, t, inserted);
    else if (t.recordId > node->key)
        node->right = insertAVLTransactionNode(node->right, t, inserted);
    else {
        node->data = t;
        return node;
    }

    return rebalanceTransactionNode(node);
}

void addRecordIdToIndexList(vector<int>& recordIds, int recordId) {
    for (int id : recordIds) {
        if (id == recordId) return;
    }
    recordIds.push_back(recordId);
    sort(recordIds.begin(), recordIds.end());
}

AVLIndexNode* insertAVLIndexNode(AVLIndexNode* node, string key, int recordId, bool& insertedKey) {
    if (node == NULL) {
        insertedKey = true;
        return newAVLIndexNode(key, recordId);
    }

    if (key < node->key)
        node->left = insertAVLIndexNode(node->left, key, recordId, insertedKey);
    else if (key > node->key)
        node->right = insertAVLIndexNode(node->right, key, recordId, insertedKey);
    else {
        addRecordIdToIndexList(node->recordIds, recordId);
        return node;
    }

    return rebalanceIndexNode(node);
}

AVLTransactionNode* maxTransactionNode(AVLTransactionNode* node) {
    AVLTransactionNode* current = node;
    while (current != NULL && current->right != NULL)
        current = current->right;
    return current;
}

AVLIndexNode* maxIndexNode(AVLIndexNode* node) {
    AVLIndexNode* current = node;
    while (current != NULL && current->right != NULL)
        current = current->right;
    return current;
}

AVLTransactionNode* deleteAVLTransactionNode(AVLTransactionNode* root, int recordId, bool& deleted) {
    if (root == NULL) return root;

    if (recordId < root->key) {
        root->left = deleteAVLTransactionNode(root->left, recordId, deleted);
    } else if (recordId > root->key) {
        root->right = deleteAVLTransactionNode(root->right, recordId, deleted);
    } else {
        deleted = true;

        if (root->left == NULL || root->right == NULL) {
            AVLTransactionNode* child = root->left != NULL ? root->left : root->right;
            delete root;
            return child;
        }

        AVLTransactionNode* predecessor = maxTransactionNode(root->left);
        root->key = predecessor->key;
        root->data = predecessor->data;
        bool ignored = false;
        root->left = deleteAVLTransactionNode(root->left, predecessor->key, ignored);
    }

    return rebalanceTransactionNode(root);
}

bool removeRecordIdFromIndexList(vector<int>& recordIds, int recordId) {
    int oldSize = (int)recordIds.size();
    recordIds.erase(remove(recordIds.begin(), recordIds.end(), recordId), recordIds.end());
    return (int)recordIds.size() != oldSize;
}

AVLIndexNode* deleteAVLIndexKeyNode(AVLIndexNode* root, string key, bool& deletedKey) {
    if (root == NULL) return root;

    if (key < root->key) {
        root->left = deleteAVLIndexKeyNode(root->left, key, deletedKey);
    } else if (key > root->key) {
        root->right = deleteAVLIndexKeyNode(root->right, key, deletedKey);
    } else {
        deletedKey = true;

        if (root->left == NULL || root->right == NULL) {
            AVLIndexNode* child = root->left != NULL ? root->left : root->right;
            delete root;
            return child;
        }

        AVLIndexNode* predecessor = maxIndexNode(root->left);
        root->key = predecessor->key;
        root->recordIds = predecessor->recordIds;
        bool ignored = false;
        root->left = deleteAVLIndexKeyNode(root->left, predecessor->key, ignored);
    }

    return rebalanceIndexNode(root);
}

AVLIndexNode* removeRecordIdFromAVLIndex(AVLIndexNode* root, string key, int recordId, bool& removedKey) {
    if (root == NULL) return root;

    if (key < root->key) {
        root->left = removeRecordIdFromAVLIndex(root->left, key, recordId, removedKey);
    } else if (key > root->key) {
        root->right = removeRecordIdFromAVLIndex(root->right, key, recordId, removedKey);
    } else {
        removeRecordIdFromIndexList(root->recordIds, recordId);
        if (root->recordIds.empty()) {
            bool deletedKey = false;
            root = deleteAVLIndexKeyNode(root, key, deletedKey);
            removedKey = deletedKey;
            return root;
        }
        return root;
    }

    return rebalanceIndexNode(root);
}

AVLTransactionNode* findAVLTransactionNode(AVLTransactionNode* node, int recordId) {
    if (node == NULL) return NULL;
    if (recordId == node->key) return node;
    if (recordId < node->key) return findAVLTransactionNode(node->left, recordId);
    return findAVLTransactionNode(node->right, recordId);
}

AVLIndexNode* findAVLIndexNode(AVLIndexNode* node, string key) {
    if (node == NULL) return NULL;
    if (key == node->key) return node;
    if (key < node->key) return findAVLIndexNode(node->left, key);
    return findAVLIndexNode(node->right, key);
}

void addToAVLIndexes(const Transaction& t) {
    bool insertedInvoice = false;
    bool insertedCustomer = false;
    bool insertedStock = false;

    avlInvoiceRoot = insertAVLIndexNode(avlInvoiceRoot, t.invoiceId, t.recordId, insertedInvoice);
    avlCustomerRoot = insertAVLIndexNode(avlCustomerRoot, t.customerId, t.recordId, insertedCustomer);
    avlStockRoot = insertAVLIndexNode(avlStockRoot, t.stockCode, t.recordId, insertedStock);

    if (insertedInvoice) avlInvoiceIndexCount++;
    if (insertedCustomer) avlCustomerIndexCount++;
    if (insertedStock) avlStockIndexCount++;
}

void removeFromAVLIndexes(const Transaction& t) {
    bool removedInvoice = false;
    bool removedCustomer = false;
    bool removedStock = false;

    avlInvoiceRoot = removeRecordIdFromAVLIndex(avlInvoiceRoot, t.invoiceId, t.recordId, removedInvoice);
    avlCustomerRoot = removeRecordIdFromAVLIndex(avlCustomerRoot, t.customerId, t.recordId, removedCustomer);
    avlStockRoot = removeRecordIdFromAVLIndex(avlStockRoot, t.stockCode, t.recordId, removedStock);

    if (removedInvoice) avlInvoiceIndexCount--;
    if (removedCustomer) avlCustomerIndexCount--;
    if (removedStock) avlStockIndexCount--;
}

void insertTransactionAVL(Transaction t) {
    assignTransactionRecordId(t);

    bool inserted = false;
    avlTransactionRoot = insertAVLTransactionNode(avlTransactionRoot, t, inserted);
    if (inserted) {
        avlTransactionCount++;
        addToAVLIndexes(t);
    }
}

int getAVLTransactionCount() {
    return avlTransactionCount;
}

int getAVLInvoiceIndexCount() {
    return avlInvoiceIndexCount;
}

int getAVLCustomerIndexCount() {
    return avlCustomerIndexCount;
}

int getAVLStockIndexCount() {
    return avlStockIndexCount;
}

void clearAVLTransactions(AVLTransactionNode* node) {
    if (node == NULL) return;

    clearAVLTransactions(node->left);
    clearAVLTransactions(node->right);
    delete node;
}

void clearAVLIndex(AVLIndexNode* node) {
    if (node == NULL) return;

    clearAVLIndex(node->left);
    clearAVLIndex(node->right);
    delete node;
}

void resetAVLTransactions() {
    clearAVLTransactions(avlTransactionRoot);
    clearAVLIndex(avlInvoiceRoot);
    clearAVLIndex(avlCustomerRoot);
    clearAVLIndex(avlStockRoot);

    avlTransactionRoot = NULL;
    avlInvoiceRoot = NULL;
    avlCustomerRoot = NULL;
    avlStockRoot = NULL;
    avlTransactionCount = 0;
    avlInvoiceIndexCount = 0;
    avlCustomerIndexCount = 0;
    avlStockIndexCount = 0;
}

size_t estimateAVLTransactionNodeMemory(AVLTransactionNode* node) {
    if (node == NULL) return 0;

    size_t memory = sizeof(AVLTransactionNode);
    memory += estimateTransactionMemory(node->data) - sizeof(Transaction);
    memory += estimateAVLTransactionNodeMemory(node->left);
    memory += estimateAVLTransactionNodeMemory(node->right);
    return memory;
}

size_t estimateAVLIndexNodeMemory(AVLIndexNode* node) {
    if (node == NULL) return 0;

    size_t memory = sizeof(AVLIndexNode);
    memory += node->key.capacity() + 1;
    memory += node->recordIds.capacity() * sizeof(int);
    memory += estimateAVLIndexNodeMemory(node->left);
    memory += estimateAVLIndexNodeMemory(node->right);
    return memory;
}

size_t estimateAVLMemory() {
    size_t memory = 0;
    memory += estimateAVLTransactionNodeMemory(avlTransactionRoot);
    memory += estimateAVLIndexNodeMemory(avlInvoiceRoot);
    memory += estimateAVLIndexNodeMemory(avlCustomerRoot);
    memory += estimateAVLIndexNodeMemory(avlStockRoot);
    return memory;
}

bool getTransactionByRecordIdAVL(int recordId, Transaction& result) {
    AVLTransactionNode* node = findAVLTransactionNode(avlTransactionRoot, recordId);
    if (node == NULL) return false;

    result = node->data;
    return true;
}

int findRecordIdByInvoiceIdAVL(string id) {
    AVLIndexNode* node = findAVLIndexNode(avlInvoiceRoot, id);
    if (node == NULL || node->recordIds.empty()) return -1;
    return node->recordIds[0];
}

vector<Transaction> getTransactionsFromAVLIndex(AVLIndexNode* node) {
    vector<Transaction> result;
    if (node == NULL) return result;

    vector<int> recordIds = node->recordIds;
    sort(recordIds.begin(), recordIds.end());
    for (int recordId : recordIds) {
        Transaction t;
        if (getTransactionByRecordIdAVL(recordId, t))
            result.push_back(t);
    }
    return result;
}

vector<Transaction> searchByInvoiceIdAVL(string id) {
    return getTransactionsFromAVLIndex(findAVLIndexNode(avlInvoiceRoot, id));
}

vector<Transaction> searchByCustomerIdAVL(string id) {
    return getTransactionsFromAVLIndex(findAVLIndexNode(avlCustomerRoot, id));
}

vector<Transaction> searchByStockCodeAVL(string code) {
    return getTransactionsFromAVLIndex(findAVLIndexNode(avlStockRoot, code));
}

void inorderAVLTransactions(AVLTransactionNode* node, vector<Transaction>& result) {
    if (node == NULL) return;

    inorderAVLTransactions(node->left, result);
    result.push_back(node->data);
    inorderAVLTransactions(node->right, result);
}

vector<Transaction> getAllTransactionsAVL() {
    vector<Transaction> result;
    inorderAVLTransactions(avlTransactionRoot, result);
    return result;
}

bool updateByRecordIdAVL(int recordId, Transaction updated) {
    AVLTransactionNode* node = findAVLTransactionNode(avlTransactionRoot, recordId);
    if (node == NULL) return false;

    Transaction old = node->data;
    removeFromAVLIndexes(old);

    updated.recordId = recordId;
    node->data = updated;
    addToAVLIndexes(updated);
    return true;
}

bool deleteByRecordIdAVL(int recordId) {
    Transaction old;
    if (!getTransactionByRecordIdAVL(recordId, old)) return false;

    removeFromAVLIndexes(old);

    bool deleted = false;
    avlTransactionRoot = deleteAVLTransactionNode(avlTransactionRoot, recordId, deleted);
    if (deleted) avlTransactionCount--;
    return deleted;
}

int findProductSummaryIndexAVL(vector<ProductSummary>& items, string stockCode) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].stockCode == stockCode)
            return i;
    }
    return -1;
}

bool containsStringAVL(const vector<string>& items, string value) {
    for (string item : items) {
        if (item == value)
            return true;
    }
    return false;
}

vector<ProductSummary> getTopNProductsAVL(int n) {
    vector<Transaction> allTransactions = getAllTransactionsAVL();
    vector<ProductSummary> summary;

    for (Transaction t : allTransactions) {
        if (t.stockCode.empty()) continue;

        int pos = findProductSummaryIndexAVL(summary, t.stockCode);
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

vector<ProductSummary> getFrequentlyBoughtTogetherAVL(string targetStockCode, int n) {
    vector<string> relatedInvoices;
    vector<ProductSummary> productInfo;
    vector<Transaction> targetTransactions = searchByStockCodeAVL(targetStockCode);

    for (Transaction t : targetTransactions) {
        if (!containsStringAVL(relatedInvoices, t.invoiceId))
            relatedInvoices.push_back(t.invoiceId);
    }

    for (string invoiceId : relatedInvoices) {
        vector<string> countedInInvoice;
        vector<Transaction> invoiceTransactions = searchByInvoiceIdAVL(invoiceId);

        for (Transaction t : invoiceTransactions) {
            if (t.stockCode == targetStockCode || containsStringAVL(countedInInvoice, t.stockCode))
                continue;

            int pos = findProductSummaryIndexAVL(productInfo, t.stockCode);
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

vector<ProductSummary> getCustomerRecommendationsAVL(string customerId, int n) {
    vector<Transaction> customerTransactions = searchByCustomerIdAVL(customerId);
    vector<string> purchasedProducts;
    vector<string> similarCustomers;
    vector<ProductSummary> productInfo;

    for (Transaction t : customerTransactions) {
        if (!containsStringAVL(purchasedProducts, t.stockCode))
            purchasedProducts.push_back(t.stockCode);
    }

    if (purchasedProducts.empty()) return productInfo;

    for (string stockCode : purchasedProducts) {
        vector<Transaction> stockTransactions = searchByStockCodeAVL(stockCode);
        for (Transaction t : stockTransactions) {
            if (t.customerId != customerId && !containsStringAVL(similarCustomers, t.customerId))
                similarCustomers.push_back(t.customerId);
        }
    }

    vector<string> countedInvoiceProduct;
    for (string similarCustomer : similarCustomers) {
        vector<Transaction> customerItems = searchByCustomerIdAVL(similarCustomer);

        for (Transaction t : customerItems) {
            string invoiceProductKey = t.invoiceId + "|" + t.stockCode;
            if (containsStringAVL(purchasedProducts, t.stockCode) ||
                containsStringAVL(countedInvoiceProduct, invoiceProductKey)) {
                continue;
            }

            int pos = findProductSummaryIndexAVL(productInfo, t.stockCode);
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

            countedInvoiceProduct.push_back(invoiceProductKey);
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

#endif
