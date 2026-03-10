#include "Transaction.h"
#include <iostream>

int main() {
    ofstream resetLog("output_log.txt", ios::trunc);
    resetLog << "=== LOG SISTEM REKOMENDASI PRODUK ===\n";
    resetLog << "[" << waktuSekarang() << "] PROGRAM DIMULAI\n";
    resetLog.close();

    vector<Transaction> data = loadCSV("transactions.csv");
    cout << "Berhasil memuat " << data.size() << " baris.\n";

    int pilih;
    do {
        cout << "\n========================================\n";
        cout << " SISTEM REKOMENDASI PRODUK - FASE 1\n";
        cout << " Struktur Data: Vector | E-Commerce\n";
        cout << "========================================\n";
        cout << " 1. Tampilkan Semua Transaksi\n";
        cout << " 2. Insert Transaksi Baru\n";
        cout << " 3. Search by Transaction ID\n";
        cout << " 4. Search by Customer ID\n";
        cout << " 5. Search by Product ID\n";
        cout << " 6. Search by Category\n";
        cout << " 7. Info Dataset\n";
        cout << "----------------------------------------\n";
        cout << " 0. Keluar\n";
        cout << "========================================\n";
        cout << "Pilih: ";
        cin >> pilih;

        switch (pilih) {
            case 1: tampilSemua(data);  break;
            case 2: insertData(data);   break;
            case 3: cariInvoice(data);  break;
            case 4: cariCustomer(data); break;
            case 5: cariProduct(data);  break;
            case 6: cariCategory(data); break;
            case 7: infoDataset(data);  break;
            case 0:
                tulisLog("\n[" + waktuSekarang() + "] PROGRAM SELESAI\n");
                cout << "Sampai jumpa!\n";
                break;
            default: cout << "Pilihan tidak valid.\n";
        }
    } while (pilih != 0);

    return 0;
}