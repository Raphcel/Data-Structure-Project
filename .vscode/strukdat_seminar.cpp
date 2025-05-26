#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

// Struct Data untuk Peserta Seminar
struct Participant {
    string name;
    time_t registrationTime;

    Participant(string n) {
        name = n;
        registrationTime = time(0);
    }
};

// Fungsi untuk mengenkripsi nama peserta menggunakan Caesar Cipher
string encryptName(const string &name, int shift) {
    string encrypted = name;
    for (char &c : encrypted) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + shift) % 26 + base;
        }
    }
    return encrypted;
}

// Fungsi untuk membuat sertifikat peserta
string generateCertificate(const Participant &participant) {
    stringstream ss;
    ss << "Sertifikat Seminar\n";
    ss << "====================\n";
    ss << "Nama: " << encryptName(participant.name, 3) << "\n";
    ss << "Waktu Pendaftaran: " << ctime(&participant.registrationTime);
    return ss.str();
}

// Node structure for Stack and Queue
template <typename T>
struct Node {
    T data;
    Node* next;
};

// Stack implementation
template <typename T>
class Stack {
private:
    Node<T>* top;
public:
    Stack() { top = nullptr; }
    ~Stack() { while (!isEmpty()) pop(); }
    void push(T value) {
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = top;
        top = newNode;
    }
    void pop() {
        if (isEmpty()) return;
        Node<T>* temp = top;
        top = top->next;
        delete temp;
    }
    T peek() {
        if (isEmpty()) return T();
        return top->data;
    }
    bool isEmpty() { return top == nullptr; }
};

// Queue implementation
template <typename T>
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;
public:
    Queue() { front = rear = nullptr; }
    ~Queue() { while (!isEmpty()) dequeue(); }
    void enqueue(T value) {
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = nullptr;
        if (rear == nullptr) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }
    void dequeue() {
        if (isEmpty()) return;
        Node<T>* temp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
    }
    T peek() {
        if (isEmpty()) return T();
        return front->data;
    }
    bool isEmpty() { return front == nullptr; }
};

// Struktur Data untuk Sistem Manajemen Seminar
class SeminarManager {
private:
    Queue<string> questionQueue; // Antrian pertanyaan peserta menggunakan Queue
    vector<pair<string, string>> answeredQuestions; // Daftar pertanyaan yang telah dijawab
    vector<Participant> participants; // Data peserta
    Stack<pair<string, string>> history; // Stack untuk undo topik/jadwal

public:
    // Pendaftaran peserta
    void registerParticipant(string name) {
        participants.push_back(Participant(name));
        cout << name << " berhasil terdaftar!\n";
    }

    // Menampilkan daftar peserta dengan sorting berdasarkan waktu pendaftaran
    void showParticipants(bool ascending) {
        if (participants.empty()) {
            cout << "Belum ada peserta yang terdaftar.\n";
            return;
        }
        sort(participants.begin(), participants.end(), [ascending](const Participant &a, const Participant &b) {
            return ascending ? a.registrationTime < b.registrationTime : a.registrationTime > b.registrationTime;
        });

        cout << "\nDaftar Peserta (Urutan " << (ascending ? "Terlama ke Terbaru" : "Terbaru ke Terlama") << "):\n";
        for (size_t i = 0; i < participants.size(); i++) {
            cout << i + 1 << ". " << participants[i].name << " (Terdaftar: " << ctime(&participants[i].registrationTime) << ")";
        }
    }

    // Menambahkan pertanyaan ke dalam queue
    void askQuestion(string name) {
        questionQueue.enqueue(name);
        cout << name << " telah menambahkan pertanyaan ke dalam antrian.\n";
    }

    // Menjawab pertanyaan berdasarkan antrian FIFO dan menyimpan jawaban
    void answerQuestion() {
        if (questionQueue.isEmpty()) {
            cout << "Tidak ada pertanyaan dalam antrian.\n";
            return;
        }
        string questioner = questionQueue.peek();
        questionQueue.dequeue();
        string answer;
        cout << "Menjawab pertanyaan dari: " << questioner << endl;
        cout << "Masukkan jawaban: ";
        getline(cin, answer);
        answeredQuestions.push_back({questioner, answer});
        cout << "Jawaban telah disimpan.\n";
    }

    // Menampilkan daftar pertanyaan yang telah dijawab
    void showAnsweredQuestions() {
        if (answeredQuestions.empty()) {
            cout << "Belum ada pertanyaan yang dijawab.\n";
            return;
        }

        cout << "\n=== Daftar Pertanyaan yang Sudah Dijawab ===\n";
        for (size_t i = 0; i < answeredQuestions.size(); i++) {
            cout << i + 1 << ". " << answeredQuestions[i].first << " → " << answeredQuestions[i].second << endl;
        }
    }

    // Menambahkan topik atau jadwal seminar ke dalam vector
    void addSchedule(string date, string topic) {
        history.push({date, topic});
        cout << "Jadwal seminar '" << topic << "' pada " << date << " berhasil ditambahkan.\n";
    }

    // Undo topik/jadwal terakhir
    void undoSchedule() {
        if (history.isEmpty()) {
            cout << "Tidak ada jadwal yang bisa di-undo.\n";
            return;
        }
        pair<string, string> lastSchedule = history.peek();
        history.pop();
        cout << "Undo jadwal: " << lastSchedule.second << " pada " << lastSchedule.first << endl;
    }

    // Menampilkan sertifikat peserta
    void showCertificates() {
        if (participants.empty()) {
            cout << "Belum ada peserta yang terdaftar.\n";
            return;
        }

        cout << "\n=== Sertifikat Peserta ===\n";
        for (size_t i = 0; i < participants.size(); i++) {
            cout << "\nPeserta " << i + 1 << ":\n";
            cout << generateCertificate(participants[i]) << endl;
        }
    }
};

// Fungsi tampilan header
void showHeader() {
    cout << "========================================\n";
    cout << "        SEMINAR/WEBINAR MANAGER\n";
    cout << "========================================\n";
}

int main() {
    SeminarManager manager;
    int choice;
    
    while (true) {
        showHeader();
        cout << "\n1. Daftar Peserta\n";
        cout << "2. Lihat Daftar Peserta (Sorting)\n";
        cout << "3. Tambah Pertanyaan\n";
        cout << "4. Jawab Pertanyaan\n";
        cout << "5. Lihat Pertanyaan yang Sudah Dijawab\n";
        cout << "6. Tambah Jadwal Seminar\n";
        cout << "7. Undo Jadwal Seminar\n";
        cout << "8. Lihat Sertifikat Peserta\n";
        cout << "9. Keluar\n";
        cout << "Pilih menu: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string name;
            cout << "Masukkan nama peserta: ";
            getline(cin, name);
            manager.registerParticipant(name);
        } 
        else if (choice == 2) {
            int sortChoice;
            cout << "\nPilih metode sorting:\n";
            cout << "1. Terlama ke Terbaru (Ascending)\n";
            cout << "2. Terbaru ke Terlama (Descending)\n";
            cout << "Pilih: ";
            cin >> sortChoice;
            cin.ignore();

            manager.showParticipants(sortChoice == 1);
        } 
        else if (choice == 3) {
            string name;
            cout << "Masukkan nama peserta yang ingin bertanya: ";
            getline(cin, name);
            manager.askQuestion(name);
        } 
        else if (choice == 4) {
            manager.answerQuestion();
        } 
        else if (choice == 5) {
            manager.showAnsweredQuestions();
        } 
        else if (choice == 6) {
            string date, topic;
            cout << "Masukkan tanggal seminar: ";
            getline(cin, date);
            cout << "Masukkan topik seminar: ";
            getline(cin, topic);
            manager.addSchedule(date, topic);
        } 
        else if (choice == 7) {
            manager.undoSchedule();
        } 
        else if (choice == 8) {
            manager.showCertificates();
        } 
        else if (choice == 9) {
            cout << "Keluar dari program...\n";
            break;
        } 
        else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }
    
    return 0;
}
