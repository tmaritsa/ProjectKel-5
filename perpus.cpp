//Program Pencatatan Peminjaman dan Pengembalian Buku

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

struct Peminjaman {
    string namaPeminjam;
    string judulBuku;
    string tanggalPeminjaman;
    string tanggalPengembalian;
};

// Fungsi untuk mengkonversi string tanggal menjadi struct tm
tm stringToDate(const string& date) {
    tm t = {};
    sscanf(date.c_str(), "%d/%d/%d", &t.tm_mday, &t.tm_mon, &t.tm_year);
    t.tm_mon -= 1; // Bulan dimulai dari 0 (Januari = 0)
    t.tm_year -= 1900; // Tahun dimulai dari 1900
    return t;
}

// Fungsi untuk menambahkan hari ke tanggal
tm addDaysToDate(tm t, int days) {
    const time_t oneDay = 24 * 60 * 60; // Satu hari dalam detik
    time_t dateInSec = mktime(&t); // Konversi tm ke waktu dalam detik
    dateInSec += days * oneDay; // Tambah hari dalam detik
    tm result = *localtime(&dateInSec); // Konversi kembali dalam format tm
    return result;
}

// Fungsi untuk mengkonversi struct tm kembali menjadi string
string dateToString(const tm& t) {
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    return string(buffer);
}

// Fungsi untuk membaca data peminjaman buku dari file CSV
vector<Peminjaman> bacaDataPeminjaman(const string& filename) {
    vector<Peminjaman> peminjamanList;
    ifstream file(filename);
    if (!file) {
        cerr << "Tidak dapat membuka file untuk membaca!" << endl;
        return peminjamanList;
    }

    string line;
    // Skip header
    getline(file, line);
    
    while (getline(file, line)) {
        Peminjaman peminjaman;
        size_t pos = 0;
        // Ambil nama peminjam
        pos = line.find(',');
        peminjaman.namaPeminjam = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Ambil judul buku
        pos = line.find(',');
        peminjaman.judulBuku = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Ambil tanggal peminjaman
        pos = line.find(',');
        peminjaman.tanggalPeminjaman = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Ambil tanggal pengembalian
        peminjaman.tanggalPengembalian = line;

        peminjamanList.push_back(peminjaman);
    }

    file.close();
    return peminjamanList;
}

// Fungsi untuk menulis ulang data peminjaman ke file CSV
void tulisDataPeminjaman(const string& filename, const vector<Peminjaman>& peminjamanList) {
    ofstream file(filename);
    if (!file) {
        cerr << "Tidak dapat membuka file untuk menulis!" << endl;
        return;
    }

    // Menulis header
    file << "Nama Peminjam,Judul Buku,Tanggal Peminjaman,Tanggal Pengembalian" << endl;

    for (const auto& peminjaman : peminjamanList) {
        file << peminjaman.namaPeminjam << ","
             << peminjaman.judulBuku << ","
             << peminjaman.tanggalPeminjaman << ","
             << peminjaman.tanggalPengembalian << endl;
    }

    file.close();
}

// Fungsi untuk mencatat peminjaman buku baru
void catatPeminjaman(const string& filename) {
    string namaPeminjam, judulBuku, tanggalPeminjaman;

    cout << "Masukkan nama peminjam: ";
    getline(cin, namaPeminjam);

    cout << "Masukkan judul buku: ";
    getline(cin, judulBuku);

    cout << "Masukkan tanggal peminjaman (DD/MM/YYYY): ";
    getline(cin, tanggalPeminjaman);

    tm tglPeminjaman = stringToDate(tanggalPeminjaman);
    tm tglPengembalian = addDaysToDate(tglPeminjaman, 7);
    string tanggalPengembalian = dateToString(tglPengembalian);

    vector <Peminjaman> peminjamanList = bacaDataPeminjaman(filename);

    // Menambahkan peminjaman baru ke dalam daftar
    peminjamanList.push_back({namaPeminjam, judulBuku, tanggalPeminjaman, tanggalPengembalian});

    // Menulis ulang data ke file CSV
    tulisDataPeminjaman(filename, peminjamanList);
    cout << "Tanggal pengembalian paling lambat: " << tanggalPengembalian << endl;
    cout << "Data peminjaman telah disimpan di " << filename << endl;
}

// Fungsi untuk mencatat pengembalian buku
void catatPengembalian(const string& filename) {
    string namaPeminjam, judulBuku, tanggalPengembalian;

    cout << "Masukkan nama peminjam: ";
    getline(cin, namaPeminjam);

    cout << "Masukkan judul buku: ";
    getline(cin, judulBuku);

    cout << "Masukkan tanggal pengembalian (DD/MM/YYYY): ";
    getline(cin, tanggalPengembalian);

    vector<Peminjaman> peminjamanList = bacaDataPeminjaman(filename);

    bool ditemukan = false;
    vector<Peminjaman> peminjamanBaru;
    for (const auto& peminjaman : peminjamanList) {
        if (peminjaman.namaPeminjam == namaPeminjam && peminjaman.judulBuku == judulBuku) {
            ditemukan = true;
            // Jika tanggal pengembalian lebih dari tanggal seharusnya
            tm tglPengembalianAsli = stringToDate(peminjaman.tanggalPengembalian);
            tm tglPengembalianUser   = stringToDate(tanggalPengembalian);
            int selisihHari = difftime(mktime(&tglPengembalianUser ), mktime(&tglPengembalianAsli)) / (60 * 60 * 24);
            if (selisihHari > 0) {
                int denda = selisihHari * 2000; // Denda 2000 per hari
                cout << "Peminjaman buku terlambat!\nDenda sebesar: " << denda << " rupiah." << endl;
            } else {
                cout << "Buku dikembalikan tepat waktu." << endl;
            }
            continue; // Hapus data peminjaman yang sudah mengembalikan
        }
        peminjamanBaru.push_back(peminjaman);
    }

    // Menulis ulang data ke file CSV
    tulisDataPeminjaman(filename, peminjamanBaru);
    if (!ditemukan) {
        cout << "Peminjaman tidak ditemukan!" << endl;
    }
}

int main() {
    string filename = "Datapeminjam.txt";
    int pilihan;
    do {
        cout << "\nMenu Peminjaman Buku\n" << endl;
        cout << "1. Catat Peminjaman Buku\n";
        cout << "2. Catat Pengembalian Buku\n";
        cout << "3. Keluar dan Simpan\n" << endl;
        cout << "Pilih opsi: ";
        cin >> pilihan;
        //fungsi jika input tidak sesuai dan bukan integer
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            continue;}
        cin.ignore();
        //switch untuk mengolah input
        switch (pilihan) {
            case 1:
                catatPeminjaman(filename);
                break;
            case 2:
                catatPengembalian(filename);
                break;
            case 3:
                cout << "Terima kasih! Program selesai." << endl;
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
        }
    
    } while (pilihan != 3); //jika input pilihan bukan 3
    return 0;
}
