#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "base64.h"
#include <fstream>
#include <regex>
#include <memory>

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

// Fungsi untuk membuat sertifikat peserta
string generateCertificate(const Participant &participant) {
    stringstream ss;
    string certRaw = participant.name + to_string(participant.registrationTime);
    string certNumber = base64_encode(reinterpret_cast<const unsigned char*>(certRaw.c_str()), certRaw.length());
    ss << "Sertifikat Seminar\n";
    ss << "====================\n";
    ss << "Nama: " << participant.name << "\n";
    ss << "Nomor Sertifikat (terenkripsi): " << certNumber << "\n";
    ss << "Waktu Pendaftaran: " << ctime(&participant.registrationTime);
    return ss.str();
}

// Base64 decoding function (from cpp-base64)
#include <stdexcept>
#include <cctype>

static std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = ( char_array_4[0] << 2 ) + ( ( char_array_4[1] & 0x30 ) >> 4 );
            char_array_3[1] = ( ( char_array_4[1] & 0xf ) << 4 ) + ( ( char_array_4[2] & 0x3c ) >> 2 );
            char_array_3[2] = ( ( char_array_4[2] & 0x3 ) << 6 ) + char_array_4[3];
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = ( char_array_4[0] << 2 ) + ( ( char_array_4[1] & 0x30 ) >> 4 );
        char_array_3[1] = ( ( char_array_4[1] & 0xf ) << 4 ) + ( ( char_array_4[2] & 0x3c ) >> 2 );
        char_array_3[2] = ( ( char_array_4[2] & 0x3 ) << 6 ) + char_array_4[3];
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    return ret;
}

// Fungsi untuk mendekripsi nomor sertifikat peserta
string decodeCertificateNumber(const string& encoded) {
    return base64_decode(encoded);
}

// Node structure for Stack and Queue
template <typename T>
struct Node {
    T data;
    std::unique_ptr<Node<T>> next;
    Node(const T& d) : data(d), next(nullptr) {}
};

// Stack implementation
template <typename T>
class Stack {
private:
    std::unique_ptr<Node<T>> top;
public:
    Stack() : top(nullptr) {}
    ~Stack() = default;
    void push(T value) {
        auto newNode = std::make_unique<Node<T>>(value);
        newNode->next = std::move(top);
        top = std::move(newNode);
    }
    void pop() {
        if (isEmpty()) return;
        top = std::move(top->next);
    }
    T peek() {
        if (isEmpty()) return T();
        return top->data;
    }
    bool isEmpty() { return top == nullptr; }
    // Deep clone for iteration
    Stack<T> clone() const {
        Stack<T> temp, result;
        Node<T>* curr = top.get();
        while (curr) {
            temp.push(curr->data);
            curr = curr->next.get();
        }
        // Reverse temp into result to preserve order
        while (!temp.isEmpty()) {
            result.push(temp.peek());
            temp.pop();
        }
        return result;
    }
};

// Queue implementation
template <typename T>
class Queue {
private:
    std::unique_ptr<Node<T>> front;
    Node<T>* rear;
public:
    Queue() : front(nullptr), rear(nullptr) {}
    ~Queue() = default;
    void enqueue(T value) {
        auto newNode = std::make_unique<Node<T>>(value);
        Node<T>* newRear = newNode.get();
        if (rear == nullptr) {
            front = std::move(newNode);
            rear = newRear;
        } else {
            rear->next = std::move(newNode);
            rear = newRear;
        }
    }
    void dequeue() {
        if (isEmpty()) return;
        front = std::move(front->next);
        if (!front) rear = nullptr;
    }
    T peek() {
        if (isEmpty()) return T();
        return front->data;
    }
    bool isEmpty() { return front == nullptr; }
    // Deep clone for iteration
    Queue<T> clone() const {
        Queue<T> result;
        Node<T>* curr = front.get();
        while (curr) {
            result.enqueue(curr->data);
            curr = curr->next.get();
        }
        return result;
    }
};

// Struktur Data untuk Sistem Manajemen Seminar
class SeminarManager {
private:
    Queue<string> questionQueue; // Antrian pertanyaan peserta menggunakan Queue
    vector<pair<string, string>> answeredQuestions; // Daftar pertanyaan yang telah dijawab
    vector<Participant> participants; // Data peserta
    Stack<pair<string, string>> history; // Stack untuk undo topik/jadwal
    const string dataFile = "seminar_data.txt";

    // Helper struct for schedule
    struct Schedule {
        string date;
        string topic;
    };

    // Helper to save all data to one file
    void saveAllDataToFile() {
        ofstream ofs(dataFile, ios::trunc);
        // Participants
        ofs << "[PARTICIPANTS]\n";
        for (const auto& p : participants) {
            ofs << p.name << '\t' << p.registrationTime << '\n';
        }
        // Answered Questions
        ofs << "[ANSWERED_QUESTIONS]\n";
        for (const auto& q : answeredQuestions) {
            ofs << q.first << '\t' << q.second << '\n';
        }
        // Schedules (history stack, bottom to top)
        ofs << "[SCHEDULES]\n";
        vector<pair<string, string>> temp;
        auto copy = history.clone();
        while (!copy.isEmpty()) {
            temp.push_back(copy.peek());
            copy.pop();
        }
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            ofs << it->first << '\t' << it->second << '\n';
        }
        // Question Queue
        ofs << "[QUESTION_QUEUE]\n";
        auto qcopy = questionQueue.clone();
        while (!qcopy.isEmpty()) {
            ofs << qcopy.peek() << '\n';
            qcopy.dequeue();
        }
        ofs.close();
    }

    // Helper to load all data from one file
    void loadAllDataFromFile() {
        ifstream ifs(dataFile);
        if (!ifs.is_open()) return;
        participants.clear();
        answeredQuestions.clear();
        while (!history.isEmpty()) history.pop();
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        string line, section;
        while (getline(ifs, line)) {
            if (line == "[PARTICIPANTS]" || line == "[ANSWERED_QUESTIONS]" || line == "[SCHEDULES]" || line == "[QUESTION_QUEUE]") {
                section = line;
                continue;
            }
            if (section == "[PARTICIPANTS]") {
                size_t tabPos = line.find('\t');
                if (tabPos != string::npos) {
                    string name = line.substr(0, tabPos);
                    time_t regTime = static_cast<time_t>(stoll(line.substr(tabPos + 1)));
                    Participant p(name);
                    p.registrationTime = regTime;
                    participants.push_back(p);
                }
            } else if (section == "[ANSWERED_QUESTIONS]") {
                size_t tabPos = line.find('\t');
                if (tabPos != string::npos) {
                    string name = line.substr(0, tabPos);
                    string answer = line.substr(tabPos + 1);
                    answeredQuestions.push_back({name, answer});
                }
            } else if (section == "[SCHEDULES]") {
                size_t tabPos = line.find('\t');
                if (tabPos != string::npos) {
                    string date = line.substr(0, tabPos);
                    string topic = line.substr(tabPos + 1);
                    history.push({date, topic});
                }
            } else if (section == "[QUESTION_QUEUE]") {
                if (!line.empty()) questionQueue.enqueue(line);
            }
        }
        ifs.close();
    }
public:
    SeminarManager() { loadAllDataFromFile(); }
    // Pendaftaran peserta
    void registerParticipant(string name) {
        participants.push_back(Participant(name));
        saveAllDataToFile();
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
        saveAllDataToFile();
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
        saveAllDataToFile();
        string answer;
        cout << "Menjawab pertanyaan dari: " << questioner << endl;
        cout << "Masukkan jawaban: ";
        getline(cin, answer);
        answeredQuestions.push_back({questioner, answer});
        saveAllDataToFile();
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
            cout << i + 1 << ". " << answeredQuestions[i].first << " -> " << answeredQuestions[i].second << endl;
        }
    }

    // Menambahkan topik atau jadwal seminar ke dalam vector
    void addSchedule(string date, string topic) {
        history.push({date, topic});
        saveAllDataToFile();
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
        saveAllDataToFile();
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

    // Delete participant by name
    void deleteParticipant(const string& name) {
        auto it = remove_if(participants.begin(), participants.end(), [&](const Participant& p){ return p.name == name; });
        if (it != participants.end()) {
            participants.erase(it, participants.end());
            saveAllDataToFile();
            cout << "Peserta " << name << " berhasil dihapus.\n";
        } else {
            cout << "Peserta tidak ditemukan.\n";
        }
    }
    // Update participant name
    void updateParticipant(const string& oldName, const string& newName) {
        bool found = false;
        for (auto& p : participants) {
            if (p.name == oldName) {
                p.name = newName;
                found = true;
            }
        }
        if (found) {
            saveAllDataToFile();
            cout << "Nama peserta berhasil diupdate.\n";
        } else {
            cout << "Peserta tidak ditemukan.\n";
        }
    }
    // Delete answered question by index (1-based)
    void deleteAnsweredQuestion(int idx) {
        if (idx < 1 || idx > (int)answeredQuestions.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        answeredQuestions.erase(answeredQuestions.begin() + idx - 1);
        saveAllDataToFile();
        cout << "Pertanyaan berhasil dihapus.\n";
    }
    // Update answered question by index (1-based)
    void updateAnsweredQuestion(int idx, const string& newAnswer) {
        if (idx < 1 || idx > (int)answeredQuestions.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        answeredQuestions[idx - 1].second = newAnswer;
        saveAllDataToFile();
        cout << "Jawaban berhasil diupdate.\n";
    }
    // Delete schedule by index (1-based, from bottom)
    void deleteSchedule(int idx) {
        vector<pair<string, string>> temp;
        auto copy = history.clone();
        while (!copy.isEmpty()) {
            temp.push_back(copy.peek());
            copy.pop();
        }
        if (idx < 1 || idx > (int)temp.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp.erase(temp.begin() + (idx - 1));
        // Rebuild stack
        while (!history.isEmpty()) history.pop();
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            history.push(*it);
        }
        saveAllDataToFile();
        cout << "Jadwal berhasil dihapus.\n";
    }
    // Update schedule by index (1-based, from bottom)
    void updateSchedule(int idx, const string& newDate, const string& newTopic) {
        vector<pair<string, string>> temp;
        auto copy = history.clone();
        while (!copy.isEmpty()) {
            temp.push_back(copy.peek());
            copy.pop();
        }
        if (idx < 1 || idx > (int)temp.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp[idx - 1] = {newDate, newTopic};
        // Rebuild stack
        while (!history.isEmpty()) history.pop();
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            history.push(*it);
        }
        saveAllDataToFile();
        cout << "Jadwal berhasil diupdate.\n";
    }
    // Delete question from queue by index (1-based, front is 1)
    void deleteQuestionFromQueue(int idx) {
        vector<string> temp;
        auto copy = questionQueue.clone();
        while (!copy.isEmpty()) {
            temp.push_back(copy.peek());
            copy.dequeue();
        }
        if (idx < 1 || idx > (int)temp.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp.erase(temp.begin() + (idx - 1));
        // Rebuild queue
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        for (const auto& q : temp) questionQueue.enqueue(q);
        saveAllDataToFile();
        cout << "Pertanyaan dalam antrian berhasil dihapus.\n";
    }
    // Update question in queue by index (1-based, front is 1)
    void updateQuestionInQueue(int idx, const string& newName) {
        vector<string> temp;
        auto copy = questionQueue.clone();
        while (!copy.isEmpty()) {
            temp.push_back(copy.peek());
            copy.dequeue();
        }
        if (idx < 1 || idx > (int)temp.size()) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp[idx - 1] = newName;
        // Rebuild queue
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        for (const auto& q : temp) questionQueue.enqueue(q);
        saveAllDataToFile();
        cout << "Pertanyaan dalam antrian berhasil diupdate.\n";
    }

    // Expose history for menu display
    const Stack<pair<string, string>>& getHistory() const { return history; }
    const Queue<string>& getQuestionQueue() const { return questionQueue; }
};

// Fungsi tampilan header
void showHeader() {
    cout << "========================================\n";
    cout << "        SEMINAR/WEBINAR MANAGER\n";
    cout << "========================================\n";
}

// Fungsi validasi format tanggal YYYY-MM-DD
bool isValidDateFormat(const string& date) {
    // Regex cek format
    if (!regex_match(date, regex("\\d{4}-\\d{2}-\\d{2}"))) return false;
    int year, month, day;
    char dash1, dash2;
    istringstream iss(date);
    iss >> year >> dash1 >> month >> dash2 >> day;
    if (dash1 != '-' || dash2 != '-') return false;
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;
    // Days in month
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    // Leap year check
    if (month == 2) {
        bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (leap) {
            if (day > 29) return false;
        } else {
            if (day > 28) return false;
        }
    } else {
        if (day > daysInMonth[month-1]) return false;
    }
    // Tambahan: tanggal tidak boleh di masa lalu
    tm inputDate = {};
    inputDate.tm_year = year - 1900;
    inputDate.tm_mon = month - 1;
    inputDate.tm_mday = day;
    inputDate.tm_hour = 0; inputDate.tm_min = 0; inputDate.tm_sec = 0;
    time_t inputTime = mktime(&inputDate);
    time_t now = time(0);
    tm* nowTm = localtime(&now);
    nowTm->tm_hour = 0; nowTm->tm_min = 0; nowTm->tm_sec = 0;
    time_t today = mktime(nowTm);
    if (difftime(inputTime, today) < 0) return false; // tanggal sebelum hari ini
    return true;
}

int main() {
    SeminarManager manager;
    int choice;
    
    while (true) {
        showHeader();
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Menu Peserta\n";
        cout << "2. Menu Pertanyaan\n";
        cout << "3. Menu Jadwal Seminar\n";
        cout << "4. Lihat Sertifikat Peserta\n";
        cout << "5. Keluar\n";
        cout << "Pilih menu utama: ";
        int mainChoice;
        cin >> mainChoice;
        cin.ignore();
        if (mainChoice == 1) {
            // Peserta submenu
            while (true) {
                cout << "\n=== MENU PESERTA ===\n";
                cout << "1. Pendaftaran Peserta\n";
                cout << "2. Lihat Daftar Peserta (Sorting)\n";
                cout << "3. Hapus Peserta\n";
                cout << "4. Update Nama Peserta\n";
                cout << "5. Kembali ke Menu Utama\n";
                cout << "Pilih menu: ";
                int sub;
                cin >> sub;
                cin.ignore();
                if (sub == 1) {
                    string name;
                    do {
                        cout << "Masukkan nama peserta: ";
                        getline(cin, name);
                        if (name.empty()) cout << "Nama peserta tidak boleh kosong!\n";
                    } while (name.empty());
                    manager.registerParticipant(name);
                } else if (sub == 2) {
                    int sortChoice;
                    cout << "\nPilih metode sorting:\n";
                    cout << "1. Terlama ke Terbaru (Ascending)\n";
                    cout << "2. Terbaru ke Terlama (Descending)\n";
                    cout << "Pilih: ";
                    cin >> sortChoice;
                    cin.ignore();
                    manager.showParticipants(sortChoice == 1);
                } else if (sub == 3) {
                    manager.showParticipants(true);
                    string name;
                    do {
                        cout << "Masukkan nama peserta yang ingin dihapus: ";
                        getline(cin, name);
                        if (name.empty()) cout << "Nama peserta tidak boleh kosong!\n";
                    } while (name.empty());
                    manager.deleteParticipant(name);
                } else if (sub == 4) {
                    manager.showParticipants(true);
                    string oldName, newName;
                    do {
                        cout << "Masukkan nama peserta yang ingin diupdate: ";
                        getline(cin, oldName);
                        if (oldName.empty()) cout << "Nama peserta tidak boleh kosong!\n";
                    } while (oldName.empty());
                    do {
                        cout << "Masukkan nama baru: ";
                        getline(cin, newName);
                        if (newName.empty()) cout << "Nama baru tidak boleh kosong!\n";
                    } while (newName.empty());
                    manager.updateParticipant(oldName, newName);
                } else if (sub == 5) {
                    break;
                } else {
                    cout << "Pilihan tidak valid.\n";
                }
            }
        } else if (mainChoice == 2) {
            // Pertanyaan submenu
            while (true) {
                cout << "\n=== MENU PERTANYAAN ===\n";
                cout << "1. Tambah Pertanyaan\n";
                cout << "2. Jawab Pertanyaan\n";
                cout << "3. Lihat Pertanyaan yang Sudah Dijawab\n";
                cout << "4. Hapus Pertanyaan yang Sudah Dijawab\n";
                cout << "5. Update Jawaban Pertanyaan\n";
                cout << "6. Hapus Pertanyaan dalam Antrian\n";
                cout << "7. Update Pertanyaan dalam Antrian\n";
                cout << "8. Kembali ke Menu Utama\n";
                cout << "Pilih menu: ";
                int sub;
                cin >> sub;
                cin.ignore();
                if (sub == 1) {
                    string name;
                    do {
                        cout << "Masukkan nama peserta yang ingin bertanya: ";
                        getline(cin, name);
                        if (name.empty()) cout << "Nama tidak boleh kosong!\n";
                    } while (name.empty());
                    manager.askQuestion(name);
                } else if (sub == 2) {
                    manager.answerQuestion();
                } else if (sub == 3) {
                    manager.showAnsweredQuestions();
                } else if (sub == 4) {
                    manager.showAnsweredQuestions();
                    int idx;
                    cout << "Masukkan nomor pertanyaan yang ingin dihapus: ";
                    cin >> idx;
                    cin.ignore();
                    manager.deleteAnsweredQuestion(idx);
                } else if (sub == 5) {
                    manager.showAnsweredQuestions();
                    int idx;
                    string newAnswer;
                    cout << "Masukkan nomor pertanyaan yang ingin diupdate: ";
                    cin >> idx;
                    cin.ignore();
                    do {
                        cout << "Masukkan jawaban baru: ";
                        getline(cin, newAnswer);
                        if (newAnswer.empty()) cout << "Jawaban tidak boleh kosong!\n";
                    } while (newAnswer.empty());
                    manager.updateAnsweredQuestion(idx, newAnswer);
                } else if (sub == 6) {
                    vector<string> temp;
                    auto copy = manager.getQuestionQueue().clone();
                    while (!copy.isEmpty()) {
                        temp.push_back(copy.peek());
                        copy.dequeue();
                    }
                    cout << "\nDaftar Pertanyaan dalam Antrian (dari depan):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i] << endl;
                    }
                    int idx;
                    cout << "Masukkan nomor pertanyaan dalam antrian yang ingin dihapus (dari depan): ";
                    cin >> idx;
                    cin.ignore();
                    manager.deleteQuestionFromQueue(idx);
                } else if (sub == 7) {
                    vector<string> temp;
                    auto copy = manager.getQuestionQueue().clone();
                    while (!copy.isEmpty()) {
                        temp.push_back(copy.peek());
                        copy.dequeue();
                    }
                    cout << "\nDaftar Pertanyaan dalam Antrian (dari depan):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i] << endl;
                    }
                    int idx;
                    string newName;
                    cout << "Masukkan nomor pertanyaan dalam antrian yang ingin diupdate (dari depan): ";
                    cin >> idx;
                    cin.ignore();
                    do {
                        cout << "Masukkan nama baru: ";
                        getline(cin, newName);
                        if (newName.empty()) cout << "Nama baru tidak boleh kosong!\n";
                    } while (newName.empty());
                    manager.updateQuestionInQueue(idx, newName);
                } else if (sub == 8) {
                    break;
                } else {
                    cout << "Pilihan tidak valid.\n";
                }
            }
        } else if (mainChoice == 3) {
            // Jadwal submenu
            while (true) {
                cout << "\n=== MENU JADWAL SEMINAR ===\n";
                cout << "1. Tambah Jadwal Seminar\n";
                cout << "2. Undo Jadwal Seminar\n";
                cout << "3. Hapus Jadwal Seminar\n";
                cout << "4. Update Jadwal Seminar\n";
                cout << "5. Kembali ke Menu Utama\n";
                cout << "Pilih menu: ";
                int sub;
                cin >> sub;
                cin.ignore();
                if (sub == 1) {
                    string date, topic;
                    do {
                        cout << "Masukkan tanggal seminar (YYYY-MM-DD): ";
                        getline(cin, date);
                        if (date.empty()) cout << "Tanggal tidak boleh kosong!\n";
                        else if (!isValidDateFormat(date)) cout << "Format tanggal tidak valid! Gunakan YYYY-MM-DD dan pastikan tanggal benar.\n";
                    } while (date.empty() || !isValidDateFormat(date));
                    do {
                        cout << "Masukkan topik seminar: ";
                        getline(cin, topic);
                        if (topic.empty()) cout << "Topik tidak boleh kosong!\n";
                    } while (topic.empty());
                    manager.addSchedule(date, topic);
                } else if (sub == 2) {
                    manager.undoSchedule();
                } else if (sub == 3) {
                    vector<pair<string, string>> temp;
                    auto copy = manager.getHistory().clone();
                    while (!copy.isEmpty()) {
                        temp.push_back(copy.peek());
                        copy.pop();
                    }
                    cout << "\nDaftar Jadwal (dari bawah):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i].first << " - " << temp[i].second << endl;
                    }
                    int idx;
                    cout << "Masukkan nomor jadwal yang ingin dihapus (dari bawah): ";
                    cin >> idx;
                    cin.ignore();
                    manager.deleteSchedule(idx);
                } else if (sub == 4) {
                    vector<pair<string, string>> temp;
                    auto copy = manager.getHistory().clone();
                    while (!copy.isEmpty()) {
                        temp.push_back(copy.peek());
                        copy.pop();
                    }
                    cout << "\nDaftar Jadwal (dari bawah):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i].first << " - " << temp[i].second << endl;
                    }
                    int idx;
                    string newDate, newTopic;
                    cout << "Masukkan nomor jadwal yang ingin diupdate (dari bawah): ";
                    cin >> idx;
                    cin.ignore();
                    do {
                        cout << "Masukkan tanggal baru (YYYY-MM-DD): ";
                        getline(cin, newDate);
                        if (newDate.empty()) cout << "Tanggal tidak boleh kosong!\n";
                        else if (!isValidDateFormat(newDate)) cout << "Format tanggal tidak valid! Gunakan YYYY-MM-DD dan pastikan tanggal benar.\n";
                    } while (newDate.empty() || !isValidDateFormat(newDate));
                    do {
                        cout << "Masukkan topik baru: ";
                        getline(cin, newTopic);
                        if (newTopic.empty()) cout << "Topik tidak boleh kosong!\n";
                    } while (newTopic.empty());
                    manager.updateSchedule(idx, newDate, newTopic);
                } else if (sub == 5) {
                    break;
                } else {
                    cout << "Pilihan tidak valid.\n";
                }
            }
        } else if (mainChoice == 4) {
            manager.showCertificates();
        } else if (mainChoice == 5) {
            cout << "Keluar dari program...\n";
            break;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }
    return 0;
}
