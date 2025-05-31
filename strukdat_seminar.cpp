#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip> // Untuk std::put_time
#include "base64.h" // Pastikan base64.h ada di direktori proyek Anda
#include <fstream>
#include <regex>
#include <stdexcept> // Untuk std::stoll, std::invalid_argument, std::out_of_range
#include <limits>    // Untuk std::numeric_limits

using namespace std;

// Struct Data untuk Peserta Seminar
struct Participant {
    string name;
    time_t registrationTime;
    string seminarDate;    // Jadwal seminar yang dipilih
    string seminarTopic;   // Topik seminar yang dipilih

    Participant(string n, string date, string topic) {
        name = n;
        seminarDate = date;
        seminarTopic = topic;
        registrationTime = time(0);
    }
    // Default constructor untuk penggunaan vector resize atau map jika diperlukan
    Participant() : registrationTime(0) {}
};

// Fungsi untuk membuat sertifikat peserta
string generateCertificate(const Participant &participant) {
    stringstream ss;
    string certRaw = participant.name + participant.seminarDate + participant.seminarTopic + to_string(participant.registrationTime);
    // Asumsikan base64_encode didefinisikan dengan benar di "base64.h"
    string certNumber = base64_encode(reinterpret_cast<const unsigned char*>(certRaw.c_str()), certRaw.length());
    ss << "Sertifikat Seminar\n";
    ss << "====================\n";
    ss << "Nama: " << participant.name << "\n";
    ss << "Jadwal: " << participant.seminarDate << " - " << participant.seminarTopic << "\n";
    ss << "Nomor Sertifikat (terenkripsi): " << certNumber << "\n";
    ss << "Waktu Pendaftaran: " << put_time(localtime(&participant.registrationTime), "%Y-%m-%d %H:%M:%S") << "\n"; // Menggunakan put_time untuk format konsisten
    return ss.str();
}

// --- Awal utilitas dekode Base64 (seperti yang diberikan dalam kode asli) ---
// Ini ada di sini jika Anda ingin menggunakannya nanti. Saat ini, decodeCertificateNumber tidak dipanggil.
static const std::string base64_chars_local = // Diganti nama untuk menghindari konflik jika base64.h memiliki miliknya sendiri
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64_local(unsigned char c) { // Diganti nama
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::string base64_decode_local(std::string const& encoded_string) { // Diganti nama
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64_local(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars_local.find(char_array_4[i]);
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
            char_array_4[j] = base64_chars_local.find(char_array_4[j]);
        char_array_3[0] = ( char_array_4[0] << 2 ) + ( ( char_array_4[1] & 0x30 ) >> 4 );
        char_array_3[1] = ( ( char_array_4[1] & 0xf ) << 4 ) + ( ( char_array_4[2] & 0x3c ) >> 2 );
        char_array_3[2] = ( ( char_array_4[2] & 0x3 ) << 6 ) + char_array_4[3];
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    return ret;
}

// Fungsi untuk mendekripsi nomor sertifikat peserta (tidak digunakan dalam alur program saat ini)
string decodeCertificateNumber(const string& encoded) {
    return base64_decode_local(encoded);
}
// --- Akhir utilitas dekode Base64 ---


// Struktur Node untuk Stack dan Queue
template <typename T>
struct Node {
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {} // Konstruktor untuk kemudahan
};

// Implementasi Stack
template <typename T>
class Stack {
private:
    Node<T>* top;
    int count; // Untuk melacak ukuran
public:
    Stack() : top(nullptr), count(0) {}
    ~Stack() { while (!isEmpty()) pop(); }
    void push(T value) {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = top;
        top = newNode;
        count++;
    }
    void pop() {
        if (isEmpty()) return;
        Node<T>* temp = top;
        top = top->next;
        delete temp;
        count--;
    }
    T peek() {
        if (isEmpty()) return T(); // Mengembalikan T yang dibangun secara default
        return top->data;
    }
    bool isEmpty() const { return top == nullptr; } // Dibuat const
    int size() const { return count; } // Menambahkan metode size

    vector<T> getAllElements() const {
        vector<T> elements;
        Node<T>* current = top;
        while(current != nullptr) {
            elements.push_back(current->data);
            current = current->next;
        }
        reverse(elements.begin(), elements.end()); // Untuk mendapatkan urutan dari bawah ke atas
        return elements;
    }
};

// Implementasi Queue
template <typename T>
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;
    int count;
public:
    Queue() : front(nullptr), rear(nullptr), count(0) {}
    ~Queue() { while (!isEmpty()) dequeue(); }
    void enqueue(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }
    void dequeue() {
        if (isEmpty()) return;
        Node<T>* temp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        count--;
    }
    T peek() {
        if (isEmpty()) return T(); // Mengembalikan T yang dibangun secara default
        return front->data;
    }
    bool isEmpty() const { return front == nullptr; } // Dibuat const
    int size() const { return count; } // Menambahkan metode size

    vector<T> getAllElements() const {
        vector<T> elements;
        Node<T>* current = front;
        while(current != nullptr) {
            elements.push_back(current->data);
            current = current->next;
        }
        return elements; // Urutan dari depan ke belakang
    }
};

// Tambahkan struct untuk pertanyaan
typedef struct Question {
    string name;
    string question;
    Question() {}
    Question(const string& n, const string& q) : name(n), question(q) {}
} Question;

// Struktur Data untuk Sistem Manajemen Seminar
class SeminarManager {
private:
    Queue<Question> questionQueue;
    vector<pair<string, string>> answeredQuestions;
    vector<Participant> participants;
    Stack<pair<string, string>> history; // Stack untuk undo topik/jadwal

    const string participantsFile = "participants.csv";
    const string answeredQuestionsFile = "answered_questions.csv";
    const string schedulesFile = "schedules.csv";
    const string questionQueueFile = "question_queue.csv";

    // --- Metode Penyimpanan CSV ---
    void saveParticipantsToFile() {
        ofstream ofs(participantsFile, ios::trunc);
        if (!ofs) {
            cerr << "Error: Tidak dapat membuka " << participantsFile << " untuk penulisan." << endl;
            return;
        }
        for (const auto& p : participants) {
            ofs << p.name << "," << p.registrationTime << "," << p.seminarDate << "," << p.seminarTopic << "\n";
        }
        ofs.close();
    }

    void saveAnsweredQuestionsToFile() {
        ofstream ofs(answeredQuestionsFile, ios::trunc);
        if (!ofs) {
            cerr << "Error: Tidak dapat membuka " << answeredQuestionsFile << " untuk penulisan." << endl;
            return;
        }
        for (const auto& q : answeredQuestions) {
            // CSV Dasar: mengasumsikan nama penanya dan jawaban tidak mengandung koma atau baris baru
            ofs << q.first << "," << q.second << "\n";
        }
        ofs.close();
    }

    void saveSchedulesToFile() {
        ofstream ofs(schedulesFile, ios::trunc);
        if (!ofs) {
            cerr << "Error: Tidak dapat membuka " << schedulesFile << " untuk penulisan." << endl;
            return;
        }
        vector<pair<string, string>> temp = history.getAllElements(); // Mendapatkan elemen dari bawah ke atas
        for (const auto& schedule : temp) {
            // CSV Dasar: mengasumsikan tanggal dan topik tidak mengandung koma atau baris baru
            ofs << schedule.first << "," << schedule.second << "\n";
        }
        ofs.close();
    }

    void saveQuestionQueueToFile() {
        ofstream ofs(questionQueueFile, ios::trunc);
        if (!ofs) {
            cerr << "Error: Tidak dapat membuka " << questionQueueFile << " untuk penulisan." << endl;
            return;
        }
        vector<Question> temp = questionQueue.getAllElements();
        for (const auto& q : temp) {
            ofs << q.name << "," << q.question << "\n";
        }
        ofs.close();
    }

    void saveAllDataToCsvFiles() {
        saveParticipantsToFile();
        saveAnsweredQuestionsToFile();
        saveSchedulesToFile();
        saveQuestionQueueToFile();
    }

    // --- Metode Pemuatan CSV ---
    void loadParticipantsFromFile() {
        ifstream ifs(participantsFile);
        if (!ifs) { return; }
        participants.clear();
        string line;
        while (getline(ifs, line)) {
            stringstream ss(line);
            string name, timeStr, seminarDate, seminarTopic;
            if (getline(ss, name, ',') && getline(ss, timeStr, ',') && 
                getline(ss, seminarDate, ',') && getline(ss, seminarTopic)) {
                try {
                    time_t regTime = static_cast<time_t>(stoll(timeStr));
                    Participant p(name, seminarDate, seminarTopic);
                    p.registrationTime = regTime;
                    participants.push_back(p);
                } catch (const std::invalid_argument& ia) {
                    cerr << "Error memuat peserta: Format time_t tidak valid di baris: " << line << endl;
                } catch (const std::out_of_range& oor) {
                    cerr << "Error memuat peserta: Nilai time_t di luar jangkauan di baris: " << line << endl;
                }
            }
        }
        ifs.close();
    }

    void loadAnsweredQuestionsFromFile() {
        ifstream ifs(answeredQuestionsFile);
        if (!ifs) { /* cerr << "Info: " << answeredQuestionsFile << " tidak ditemukan. Memulai dari awal." << endl; */ return; }
        answeredQuestions.clear();
        string line;
        while (getline(ifs, line)) {
            stringstream ss(line);
            string name, answer;
            // Asumsikan jawaban adalah sisa baris setelah koma pertama
            size_t comma_pos = line.find(',');
            if (comma_pos != string::npos) {
                name = line.substr(0, comma_pos);
                answer = line.substr(comma_pos + 1);
                answeredQuestions.push_back({name, answer});
            }
        }
        ifs.close();
    }

    void loadSchedulesFromFile() {
        ifstream ifs(schedulesFile);
        if (!ifs) { /* cerr << "Info: " << schedulesFile << " tidak ditemukan. Memulai dari awal." << endl; */ return; }
        // Bersihkan stack sebelum memuat
        while (!history.isEmpty()) history.pop();
        string line;
        while (getline(ifs, line)) {
            stringstream ss(line);
            string date, topic;
            // Asumsikan topik adalah sisa baris setelah koma pertama
            size_t comma_pos = line.find(',');
            if (comma_pos != string::npos) {
                date = line.substr(0, comma_pos);
                topic = line.substr(comma_pos + 1);
                history.push({date, topic}); // Dimuat berurutan, akan menjadi urutan stack yang benar
            }
        }
        ifs.close();
    }

    void loadQuestionQueueFromFile() {
        ifstream ifs(questionQueueFile);
        if (!ifs) { return; }
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        string line;
        while (getline(ifs, line)) {
            if (!line.empty()) {
                size_t comma_pos = line.find(',');
                if (comma_pos != string::npos) {
                    string name = line.substr(0, comma_pos);
                    string question = line.substr(comma_pos + 1);
                    questionQueue.enqueue(Question(name, question));
                }
            }
        }
        ifs.close();
    }

    void loadAllDataFromCsvFiles() {
        loadParticipantsFromFile();
        loadAnsweredQuestionsFromFile();
        loadSchedulesFromFile();
        loadQuestionQueueFromFile();
    }

public:
    SeminarManager() { loadAllDataFromCsvFiles(); }

    void registerParticipant(string name, string seminarDate, string seminarTopic) {
        for (const auto& p : participants) {
            if (p.name == name && p.seminarDate == seminarDate && p.seminarTopic == seminarTopic) {
                cout << "Nama peserta sudah terdaftar untuk jadwal ini!\n";
                return;
            }
        }
        participants.push_back(Participant(name, seminarDate, seminarTopic));
        saveAllDataToCsvFiles();
        cout << name << " berhasil terdaftar untuk seminar '" << seminarTopic << "' pada " << seminarDate << "!\n";
    }

    void showParticipants(bool ascending) {
        if (participants.empty()) {
            cout << "Belum ada peserta yang terdaftar.\n";
            return;
        }
        vector<Participant> sortedParticipants = participants;
        sort(sortedParticipants.begin(), sortedParticipants.end(), [ascending](const Participant &a, const Participant &b) {
            return ascending ? a.registrationTime < b.registrationTime : a.registrationTime > b.registrationTime;
        });

        cout << "\nDaftar Peserta (Urutan " << (ascending ? "Terlama ke Terbaru" : "Terbaru ke Terlama") << "):\n";
        for (size_t i = 0; i < sortedParticipants.size(); i++) {
            cout << i + 1 << ". " << sortedParticipants[i].name 
                 << " (Jadwal: " << sortedParticipants[i].seminarDate << " - " << sortedParticipants[i].seminarTopic 
                 << ", Terdaftar: " << put_time(localtime(&sortedParticipants[i].registrationTime), "%Y-%m-%d %H:%M:%S") << ")\n";
        }
    }

    void displayParticipantsForSelection() { // Bantuan untuk hapus/perbarui
        if (participants.empty()) {
            // Pesan ini mungkin tidak diperlukan jika sudah diperiksa di menu utama
            // cout << "Belum ada peserta yang terdaftar.\n";
            return;
        }
        cout << "\nDaftar Peserta Saat Ini:\n";
        for (size_t i = 0; i < participants.size(); i++) {
            cout << i + 1 << ". " << participants[i].name 
                 << " (Jadwal: " << participants[i].seminarDate << " - " << participants[i].seminarTopic 
                 << ", Terdaftar: " << put_time(localtime(&participants[i].registrationTime), "%Y-%m-%d %H:%M:%S") << ")\n";
        }
    }


    void askQuestion(const string& name, const string& question) {
        questionQueue.enqueue(Question(name, question));
        saveAllDataToCsvFiles();
        cout << name << " telah menambahkan pertanyaan ke dalam antrian.\n";
    }

    void answerQuestion() {
        if (questionQueue.isEmpty()) {
            cout << "Tidak ada pertanyaan dalam antrian.\n";
            return;
        }
        string questioner = questionQueue.peek().name;
        questionQueue.dequeue(); // Dequeue terlebih dahulu

        string answer;
        cout << "Menjawab pertanyaan dari: " << questioner << endl;
        cout << "Masukkan jawaban: ";
        // cin.ignore() sebelum getline jika input sebelumnya adalah cin >>
        // Mengasumsikan answerQuestion dipanggil dari menu di mana cin.ignore ditangani setelah pilihan
        getline(cin, answer);
        while (answer.empty()){
            cout << "Jawaban tidak boleh kosong. Masukkan jawaban: ";
            getline(cin, answer);
        }

        answeredQuestions.push_back({questioner, answer});
        saveAllDataToCsvFiles(); // Simpan setelah semua operasi untuk fungsi ini selesai
        cout << "Jawaban telah disimpan.\n";
    }

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

    void addSchedule(string date, string topic) {
        history.push({date, topic});
        saveAllDataToCsvFiles();
        cout << "Jadwal seminar '" << topic << "' pada " << date << " berhasil ditambahkan.\n";
    }

    void undoSchedule() {
        if (history.isEmpty()) {
            cout << "Tidak ada jadwal yang bisa di-undo.\n";
            return;
        }
        pair<string, string> lastSchedule = history.peek();
        history.pop();
        saveAllDataToCsvFiles();
        cout << "Undo jadwal: " << lastSchedule.second << " pada " << lastSchedule.first << endl;
    }

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

    void deleteParticipant(const string& name) {
        auto it = remove_if(participants.begin(), participants.end(), [&](const Participant& p){ return p.name == name; });
        if (it != participants.end()) {
            participants.erase(it, participants.end());
            saveAllDataToCsvFiles();
            cout << "Peserta " << name << " berhasil dihapus.\n";
        } else {
            cout << "Peserta '" << name << "' tidak ditemukan.\n";
        }
    }

    void updateParticipant(const string& oldName, const string& newName) {
        // Pertama, periksa apakah newName sudah ada (jika nama harus unik)
        bool newNameExists = false;
        for (const auto& p : participants) {
            if (p.name == newName) {
                newNameExists = true;
                break;
            }
        }
        if (newNameExists) {
             cout << "Nama peserta '" << newName << "' sudah digunakan. Pilih nama lain.\n";
             return;
        }

        bool found = false;
        for (auto& p : participants) {
            if (p.name == oldName) {
                p.name = newName;
                found = true;
                break;
            }
        }
        if (found) {
            saveAllDataToCsvFiles();
            cout << "Nama peserta berhasil diupdate dari " << oldName << " ke " << newName << ".\n";
        } else {
            cout << "Peserta dengan nama '" << oldName << "' tidak ditemukan.\n";
        }
    }

    void deleteAnsweredQuestion(int idx) {
        if (idx < 1 || idx > static_cast<int>(answeredQuestions.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        answeredQuestions.erase(answeredQuestions.begin() + idx - 1);
        saveAllDataToCsvFiles();
        cout << "Pertanyaan berhasil dihapus.\n";
    }

    void updateAnsweredQuestion(int idx, const string& newAnswer) {
        if (idx < 1 || idx > static_cast<int>(answeredQuestions.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        answeredQuestions[idx - 1].second = newAnswer;
        saveAllDataToCsvFiles();
        cout << "Jawaban berhasil diupdate.\n";
    }

    void deleteSchedule(int idx) { // idx berbasis 1 dari bawah stack (paling lama)
        vector<pair<string, string>> temp = history.getAllElements(); // dari bawah ke atas
        if (idx < 1 || idx > static_cast<int>(temp.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp.erase(temp.begin() + (idx - 1)); // Hapus berdasarkan vektor 0-indeks

        // Bangun ulang stack
        while (!history.isEmpty()) history.pop();
        for (const auto& item : temp) { // temp sudah dari bawah ke atas
            history.push(item);
        }
        saveAllDataToCsvFiles();
        cout << "Jadwal berhasil dihapus.\n";
    }

    void updateSchedule(int idx, const string& newDate, const string& newTopic) { // idx berbasis 1 dari bawah
        vector<pair<string, string>> temp = history.getAllElements(); // dari bawah ke atas
        if (idx < 1 || idx > static_cast<int>(temp.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp[idx - 1] = {newDate, newTopic}; // Perbarui berdasarkan vektor 0-indeks

        // Bangun ulang stack
        while (!history.isEmpty()) history.pop();
        for (const auto& item : temp) {
            history.push(item);
        }
        saveAllDataToCsvFiles();
        cout << "Jadwal berhasil diupdate.\n";
    }

    void deleteQuestionFromQueue(int idx) { // idx berbasis 1 dari depan
        vector<Question> temp = questionQueue.getAllElements(); // dari depan ke belakang
        if (idx < 1 || idx > static_cast<int>(temp.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp.erase(temp.begin() + (idx - 1));

        // Bangun ulang queue
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        for (const auto& q : temp) questionQueue.enqueue(q);
        saveAllDataToCsvFiles();
        cout << "Pertanyaan dalam antrian berhasil dihapus.\n";
    }

    void updateQuestionInQueue(int idx, const string& newName, const string& newQuestion) { // idx berbasis 1 dari depan
        vector<Question> temp = questionQueue.getAllElements(); // dari depan ke belakang
        if (idx < 1 || idx > static_cast<int>(temp.size())) {
            cout << "Indeks tidak valid.\n";
            return;
        }
        temp[idx - 1] = Question(newName, newQuestion);

        // Bangun ulang queue
        while (!questionQueue.isEmpty()) questionQueue.dequeue();
        for (const auto& q : temp) questionQueue.enqueue(q);
        saveAllDataToCsvFiles();
        cout << "Pertanyaan dalam antrian berhasil diupdate.\n";
    }

    const Stack<pair<string, string>>& getHistory() const { return history; }
    const Queue<Question>& getQuestionQueue() const { return questionQueue; }
    const vector<Participant>& getParticipants() const { return participants; }
    const vector<pair<string, string>>& getAnsweredQuestions() const { return answeredQuestions; } // <-- FUNGSI YANG DIPERBAIKI/DITAMBAHKAN
};

void showHeader() {
    cout << "========================================\n";
    cout << "        SEMINAR/WEBINAR MANAGER\n";
    cout << "========================================\n";
}

bool isValidDateFormat(const string& date) {
    if (!regex_match(date, regex("\\d{4}-\\d{2}-\\d{2}"))) return false;
    int year, month, day;
    char dash1, dash2;
    istringstream iss(date);
    iss >> year >> dash1 >> month >> dash2 >> day;

    if (iss.fail() || dash1 != '-' || dash2 != '-') return false; // Periksa kegagalan parsing
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;

    int daysInMonth[] = {0, 31,28,31,30,31,30,31,31,30,31,30,31}; // Indeks 0 tidak digunakan
    if (month == 2) {
        bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (leap) {
            if (day > 29) return false;
        } else {
            if (day > 28) return false;
        }
    } else {
         if (month > 12) return false; // Periksa tambahan jika month > 12 sebelum mengakses daysInMonth
        if (day > daysInMonth[month]) return false;
    }

    tm inputDateTm = {};
    inputDateTm.tm_year = year - 1900;
    inputDateTm.tm_mon = month - 1;
    inputDateTm.tm_mday = day;
    inputDateTm.tm_hour = 0; inputDateTm.tm_min = 0; inputDateTm.tm_sec = 0;
    inputDateTm.tm_isdst = -1; // Biarkan mktime mencari tahu DST
    time_t inputTime = mktime(&inputDateTm);

    if (inputTime == -1) return false; // Komponen tanggal tidak valid membuat mktime gagal

    time_t now = time(0);
    tm nowLocalTmStruct; // Gunakan struct baru, jangan pointer ke memori statis localtime
    #ifdef _WIN32
        localtime_s(&nowLocalTmStruct, &now); // Windows specific
    #else
        localtime_r(&now, &nowLocalTmStruct); // POSIX specific (thread-safe)
    #endif

    nowLocalTmStruct.tm_hour = 0; nowLocalTmStruct.tm_min = 0; nowLocalTmStruct.tm_sec = 0;
    time_t today = mktime(&nowLocalTmStruct);

    return difftime(inputTime, today) >= 0; // Tanggal harus hari ini atau di masa depan
}

int main() {
    SeminarManager manager;
    int mainChoice;

    while (true) {
        showHeader();
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Menu Peserta\n";
        cout << "2. Menu Pertanyaan\n";
        cout << "3. Menu Jadwal Seminar\n";
        cout << "4. Lihat Sertifikat Peserta\n";
        cout << "5. Keluar\n";
        cout << "Pilih menu utama: ";

        cin >> mainChoice;
        if (cin.fail()) {
            cin.clear(); // Hapus flag error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Buang input tidak valid
            cout << "Input tidak valid. Silakan masukkan angka.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Buang sisa baris

        if (mainChoice == 1) {
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
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Input tidak valid. Silakan masukkan angka.\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (sub == 1) {
                    // Pilih jadwal seminar terlebih dahulu
                    vector<pair<string, string>> schedules = manager.getHistory().getAllElements();
                    if (schedules.empty()) {
                        cout << "Belum ada jadwal seminar. Tidak dapat mendaftar peserta.\n";
                        continue;
                    }
                    cout << "\nPilih Jadwal Seminar untuk Pendaftaran:\n";
                    for (size_t i = 0; i < schedules.size(); ++i) {
                        cout << i + 1 << ". Tanggal: " << schedules[i].first << " - Topik: " << schedules[i].second << endl;
                    }
                    int scheduleIdx;
                    cout << "Masukkan nomor jadwal seminar: ";
                    cin >> scheduleIdx;
                    if (cin.fail() || scheduleIdx < 1 || scheduleIdx > (int)schedules.size()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Pilihan jadwal tidak valid.\n";
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    string seminarDate = schedules[scheduleIdx - 1].first;
                    string seminarTopic = schedules[scheduleIdx - 1].second;

                    string name;
                    bool name_is_valid_for_registration;
                    do {
                        name_is_valid_for_registration = true;
                        cout << "Masukkan nama peserta: ";
                        getline(cin, name);
                        if (name.empty()) {
                            cout << "Nama peserta tidak boleh kosong!\n";
                            name_is_valid_for_registration = false;
                        } else {
                            for (const auto& p : manager.getParticipants()) {
                                if (p.name == name && p.seminarDate == seminarDate && p.seminarTopic == seminarTopic) {
                                    cout << "Nama peserta sudah terdaftar untuk jadwal ini! Silakan masukkan nama lain.\n";
                                    name_is_valid_for_registration = false;
                                    break;
                                }
                            }
                        }
                    } while (!name_is_valid_for_registration);
                    manager.registerParticipant(name, seminarDate, seminarTopic);
                } else if (sub == 2) {
                    int sortChoice;
                    cout << "\nPilih metode sorting:\n";
                    cout << "1. Terlama ke Terbaru (Ascending)\n";
                    cout << "2. Terbaru ke Terlama (Descending)\n";
                    cout << "Pilih: ";
                    cin >> sortChoice;
                     if (cin.fail() || (sortChoice != 1 && sortChoice != 2)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Pilihan sorting tidak valid.\n";
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    manager.showParticipants(sortChoice == 1);
                } else if (sub == 3) {
                    if (manager.getParticipants().empty()){
                        cout << "Belum ada peserta untuk dihapus.\n";
                        continue;
                    }
                    manager.displayParticipantsForSelection(); // Tampilkan peserta tanpa sorting
                    string name;
                    do {
                        cout << "Masukkan nama peserta yang ingin dihapus: ";
                        getline(cin, name);
                        if (name.empty()) cout << "Nama peserta tidak boleh kosong!\n";
                    } while (name.empty());
                    manager.deleteParticipant(name);
                } else if (sub == 4) {
                     if (manager.getParticipants().empty()){
                        cout << "Belum ada peserta untuk diupdate.\n";
                        continue;
                    }
                    manager.displayParticipantsForSelection();
                    string oldName, newName;
                    do {
                        cout << "Masukkan nama peserta yang ingin diupdate: ";
                        getline(cin, oldName);
                        if (oldName.empty()) cout << "Nama peserta (lama) tidak boleh kosong!\n";
                    } while (oldName.empty());
                    do {
                        cout << "Masukkan nama baru: ";
                        getline(cin, newName);
                        if (newName.empty()) cout << "Nama baru tidak boleh kosong!\n";
                        else if (newName == oldName) {
                            cout << "Nama baru tidak boleh sama dengan nama lama!\n";
                            // Loop akan berlanjut karena newName.empty() adalah false
                        }
                    } while (newName.empty() || newName == oldName);
                    manager.updateParticipant(oldName, newName);
                } else if (sub == 5) {
                    break;
                } else {
                    cout << "Pilihan tidak valid.\n";
                }
            }
        } else if (mainChoice == 2) { // Menu Pertanyaan
            while (true) {
                cout << "\n=== MENU PERTANYAAN ===\n";
                cout << "1. Tambah Pertanyaan (oleh peserta)\n";
                cout << "2. Jawab Pertanyaan (oleh admin/pembicara)\n";
                cout << "3. Lihat Pertanyaan yang Sudah Dijawab\n";
                cout << "4. Hapus Pertanyaan yang Sudah Dijawab\n";
                cout << "5. Update Jawaban Pertanyaan\n";
                cout << "6. Hapus Pertanyaan dalam Antrian\n";
                cout << "7. Update Pertanyaan dalam Antrian\n";
                cout << "8. Kembali ke Menu Utama\n";
                cout << "Pilih menu: ";
                int sub;
                cin >> sub;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Input tidak valid. Silakan masukkan angka.\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (sub == 1) {
                    if (manager.getParticipants().empty()) {
                        cout << "Belum ada peserta yang terdaftar. Tidak dapat menambah pertanyaan.\n";
                        continue;
                    }
                    cout << "\nDaftar Peserta Terdaftar:\n";
                    for (size_t i = 0; i < manager.getParticipants().size(); ++i) {
                        cout << i + 1 << ". " << manager.getParticipants()[i].name << endl;
                    }
                    string name, question;
                    do {
                        cout << "Masukkan nama peserta yang ingin bertanya: ";
                        getline(cin, name);
                        if (name.empty()) cout << "Nama tidak boleh kosong!\n";
                    } while (name.empty());
                    do {
                        cout << "Masukkan kalimat pertanyaan: ";
                        getline(cin, question);
                        if (question.empty()) cout << "Pertanyaan tidak boleh kosong!\n";
                    } while (question.empty());
                    manager.askQuestion(name, question);
                } else if (sub == 2) {
                    if (manager.getQuestionQueue().isEmpty()) {
                        cout << "Tidak ada pertanyaan dalam antrian.\n";
                        continue;
                    }
                    cout << "\nDaftar Peserta Terdaftar:\n";
                    for (size_t i = 0; i < manager.getParticipants().size(); ++i) {
                        cout << i + 1 << ". " << manager.getParticipants()[i].name << endl;
                    }
                    // Tampilkan pertanyaan yang akan dijawab
                    vector<Question> temp = manager.getQuestionQueue().getAllElements();
                    if (!temp.empty()) {
                        cout << "\nPertanyaan berikut akan dijawab:\n";
                        cout << "Dari: " << temp.front().name << endl;
                        cout << "Pertanyaan: " << temp.front().question << endl;
                    }
                    manager.answerQuestion();
                } else if (sub == 3) {
                    manager.showAnsweredQuestions();
                } else if (sub == 4) { // Hapus Pertanyaan yang Sudah Dijawab
                    if (manager.getAnsweredQuestions().empty()){ // MENGGUNAKAN FUNGSI YANG DIPERBAIKI
                         cout << "Belum ada pertanyaan dijawab untuk dihapus.\n";
                         continue;
                    }
                    manager.showAnsweredQuestions();
                    int idx;
                    cout << "Masukkan nomor pertanyaan yang ingin dihapus: ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    manager.deleteAnsweredQuestion(idx);
                } else if (sub == 5) { // Update Jawaban Pertanyaan
                    if (manager.getAnsweredQuestions().empty()){ // MENGGUNAKAN FUNGSI YANG DIPERBAIKI
                         cout << "Belum ada pertanyaan dijawab untuk diupdate.\n";
                         continue;
                    }
                    manager.showAnsweredQuestions();
                    int idx;
                    string newAnswer;
                    cout << "Masukkan nomor pertanyaan yang ingin diupdate: ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    do {
                        cout << "Masukkan jawaban baru: ";
                        getline(cin, newAnswer);
                        if (newAnswer.empty()) cout << "Jawaban tidak boleh kosong!\n";
                    } while (newAnswer.empty());
                    manager.updateAnsweredQuestion(idx, newAnswer);
                } else if (sub == 6) { // Hapus Pertanyaan dalam Antrian
                    if (manager.getQuestionQueue().isEmpty()){
                         cout << "Antrian pertanyaan kosong.\n";
                         continue;
                    }
                    vector<Question> temp = manager.getQuestionQueue().getAllElements();
                    cout << "\nDaftar Pertanyaan dalam Antrian (dari depan):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i].name << " : " << temp[i].question << endl;
                    }
                    int idx;
                    cout << "Masukkan nomor pertanyaan dalam antrian yang ingin dihapus (dari depan): ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    manager.deleteQuestionFromQueue(idx);
                } else if (sub == 7) { // Update Pertanyaan dalam Antrian
                     if (manager.getQuestionQueue().isEmpty()){
                         cout << "Antrian pertanyaan kosong.\n";
                         continue;
                    }
                    vector<Question> temp = manager.getQuestionQueue().getAllElements();
                    cout << "\nDaftar Pertanyaan dalam Antrian (dari depan):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". " << temp[i].name << " : " << temp[i].question << endl;
                    }
                    int idx;
                    string newName, newQuestion;
                    cout << "Masukkan nomor pertanyaan dalam antrian yang ingin diupdate (dari depan): ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    do {
                        cout << "Masukkan nama peserta baru: ";
                        getline(cin, newName);
                        if (newName.empty()) cout << "Nama tidak boleh kosong!\n";
                    } while (newName.empty());
                    do {
                        cout << "Masukkan kalimat pertanyaan baru: ";
                        getline(cin, newQuestion);
                        if (newQuestion.empty()) cout << "Pertanyaan tidak boleh kosong!\n";
                    } while (newQuestion.empty());
                    manager.updateQuestionInQueue(idx, newName, newQuestion);
                } else if (sub == 8) {
                    break;
                } else {
                    cout << "Pilihan tidak valid.\n";
                }
            }
        } else if (mainChoice == 3) { // Menu Jadwal Seminar
            while (true) {
                cout << "\n=== MENU JADWAL SEMINAR ===\n";
                cout << "1. Tambah Jadwal Seminar\n";
                cout << "2. Undo Jadwal Seminar\n";
                cout << "3. Hapus Jadwal Seminar\n";
                cout << "4. Update Jadwal Seminar\n";
                cout << "5. Lihat Daftar Jadwal Seminar\n";
                cout << "6. Kembali ke Menu Utama\n";
                cout << "Pilih menu: ";
                int sub;
                cin >> sub;
                 if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Input tidak valid. Silakan masukkan angka.\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (sub == 1) {
                    string date, topic;
                    do {
                        cout << "Masukkan tanggal seminar (YYYY-MM-DD): ";
                        getline(cin, date);
                        if (date.empty()) cout << "Tanggal tidak boleh kosong!\n";
                        else if (!isValidDateFormat(date)) cout << "Format tanggal tidak valid! Gunakan YYYY-MM-DD, pastikan tanggal benar dan tidak di masa lalu.\n";
                    } while (date.empty() || !isValidDateFormat(date));
                    do {
                        cout << "Masukkan topik seminar: ";
                        getline(cin, topic);
                        if (topic.empty()) cout << "Topik tidak boleh kosong!\n";
                    } while (topic.empty());
                    manager.addSchedule(date, topic);
                } else if (sub == 2) {
                    manager.undoSchedule();
                } else if (sub == 3) { // Hapus Jadwal Seminar
                    if (manager.getHistory().isEmpty()){
                         cout << "Tidak ada jadwal untuk dihapus.\n";
                         continue;
                    }
                    vector<pair<string, string>> temp = manager.getHistory().getAllElements(); // dari bawah ke atas
                    cout << "\nDaftar Jadwal (dari yang paling lama ditambahkan):\n";
                    for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". Tanggal: " << temp[i].first << " - Topik: " << temp[i].second << endl;
                    }
                    int idx;
                    cout << "Masukkan nomor jadwal yang ingin dihapus (berdasarkan daftar di atas): ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    manager.deleteSchedule(idx);
                } else if (sub == 4) { // Update Jadwal Seminar
                     if (manager.getHistory().isEmpty()){
                         cout << "Tidak ada jadwal untuk diupdate.\n";
                         continue;
                    }
                    vector<pair<string, string>> temp = manager.getHistory().getAllElements(); // dari bawah ke atas
                    cout << "\nDaftar Jadwal (dari yang paling lama ditambahkan):\n";
                     for (size_t i = 0; i < temp.size(); ++i) {
                        cout << i + 1 << ". Tanggal: " << temp[i].first << " - Topik: " << temp[i].second << endl;
                    }
                    int idx;
                    string newDate, newTopic;
                    cout << "Masukkan nomor jadwal yang ingin diupdate (berdasarkan daftar di atas): ";
                    cin >> idx;
                    if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Input indeks tidak valid.\n"; continue;}
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    do {
                        cout << "Masukkan tanggal baru (YYYY-MM-DD): ";
                        getline(cin, newDate);
                        if (newDate.empty()) cout << "Tanggal tidak boleh kosong!\n";
                        else if (!isValidDateFormat(newDate)) cout << "Format tanggal tidak valid! Gunakan YYYY-MM-DD, pastikan tanggal benar dan tidak di masa lalu.\n";
                    } while (newDate.empty() || !isValidDateFormat(newDate));
                    do {
                        cout << "Masukkan topik baru: ";
                        getline(cin, newTopic);
                        if (newTopic.empty()) cout << "Topik tidak boleh kosong!\n";
                    } while (newTopic.empty());
                    manager.updateSchedule(idx, newDate, newTopic);
                } else if (sub == 5) { // Lihat Daftar Jadwal Seminar
                    vector<pair<string, string>> temp = manager.getHistory().getAllElements();
                    if (temp.empty()) {
                        cout << "Belum ada jadwal seminar yang terdaftar.\n";
                    } else {
                        cout << "\nDaftar Jadwal Seminar (dari yang paling lama ditambahkan):\n";
                        for (size_t i = 0; i < temp.size(); ++i) {
                            cout << i + 1 << ". Tanggal: " << temp[i].first << " - Topik: " << temp[i].second << endl;
                        }
                    }
                } else if (sub == 6) {
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