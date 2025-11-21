#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <chrono>
#include <thread>
#include <random>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cctype>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

using namespace std;

// ================= CLEAR SCREEN =================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ================= HEADER =======================
void header() {
    cout << "=====================================\n";
    cout << "          HABIT TRACKER\n";
    cout << "=====================================\n";
}

// ================= DATE & STREAK =================
string getCurrentDate() {
    time_t now = time(nullptr);
    tm local_tm{};
#ifdef _MSC_VER
    localtime_s(&local_tm, &now);
#else
    localtime_r(&now, &local_tm);
#endif
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
        1900 + local_tm.tm_year,
        1 + local_tm.tm_mon,
        local_tm.tm_mday);
    return string(buffer);
}

void updateStreak() {
    string today = getCurrentDate();
    string last;
    int streak = 0, best = 0;

    ifstream in("streak.txt");
    if (in.is_open()) in >> last >> streak >> best;
    in.close();

    if (last != today) {
        streak++;
        if (streak > best) best = streak;
    }

    ofstream out("streak.txt");
    out << today << " " << streak << " " << best;
    out.close();
}

void logHistory(int pomodoros, int tasks) {
    string today = getCurrentDate();
    vector<string> lines;
    string line;

    ifstream infile("history.txt");
    while (getline(infile, line)) lines.push_back(line);
    infile.close();

    bool found = false;
    for (string& entry : lines) {
        if (entry.size() >= 10 && entry.substr(0, 10) == today) {
            istringstream iss(entry);
            string datePart;
            int p = 0, t = 0;
            iss >> datePart >> p >> t;
            p += pomodoros;
            t += tasks;
            entry = today + " " + to_string(p) + " " + to_string(t);
            found = true;
            break;
        }
    }
    if (!found) {
        lines.push_back(today + " " + to_string(pomodoros) + " " + to_string(tasks));
    }

    ofstream out("history.txt");
    for (auto& x : lines) out << x << "\n";
    out.close();
}

// ================= NOTIFICATION SOUND =================
void playNotificationSound() {
#ifdef _WIN32
    Beep(1200, 150);
    Beep(900, 120);
    Beep(1500, 180);
#else
    cout << '\a' << flush;
#endif
}

// ================= PROGRESS MANAGER =================
class ProgressManager {
private:
    int pomodoros = 0;
    int tasks = 0;
public:
    ProgressManager() { load(); }

    void load() {
        ifstream in("progress.txt");
        if (in.is_open()) in >> pomodoros >> tasks;
        in.close();
    }

    void save() {
        ofstream out("progress.txt");
        out << pomodoros << " " << tasks;
        out.close();
    }

    void addPomodoro() { pomodoros++; save(); updateStreak(); logHistory(1, 0); }
    void addTask() { tasks++; save(); updateStreak(); logHistory(0, 1); }

    void display() {
        cout << "Pomodoros: " << pomodoros << "\n";
        cout << "Tasks: " << tasks << "\n";
        int percent = min(100, (pomodoros + tasks) * 10);
        cout << "\nProgress: [";
        int filled = percent / 5;
        for (int i = 0; i < filled; i++) cout << "#";
        for (int i = filled; i < 20; i++) cout << "-";
        cout << "] " << percent << "%\n";

        string last;
        int streak = 0, best = 0;
        ifstream sfile("streak.txt");
        if (sfile.is_open()) sfile >> last >> streak >> best;
        sfile.close();

        cout << "\nCurrent Streak: " << streak;
        cout << "\nBest Streak: " << best << "\n";

        cout << "\nLast 7 Days:\n";
        ifstream h("history.txt");
        vector<string> hist;
        while (getline(h, last)) hist.push_back(last);
        int start = max(0, (int)hist.size() - 7);
        for (int i = start; i < hist.size(); i++) cout << hist[i] << "\n";
    }
};

// ================= NOTIFICATION MANAGER =================
class NotificationManager {
private:
    struct Note {
        string message;
        string reminder; // YYYY-MM-DD HH:MM
    };
    vector<Note> notes;

    bool isReminderDue(const string& r) {
        if (r.empty()) return false;
        time_t now = time(nullptr);
        tm local_tm{};
#ifdef _MSC_VER
        localtime_s(&local_tm, &now);
#else
        localtime_r(&now, &local_tm);
#endif
        int y, m, d, hh, mm;
        if (sscanf(r.c_str(), "%d-%d-%d %d:%d", &y, &m, &d, &hh, &mm) != 5) return false;
        tm reminder_tm{};
        reminder_tm.tm_year = y - 1900;
        reminder_tm.tm_mon = m - 1;
        reminder_tm.tm_mday = d;
        reminder_tm.tm_hour = hh;
        reminder_tm.tm_min = mm;
        reminder_tm.tm_sec = 0;
        time_t reminder_time = mktime(&reminder_tm);
        return difftime(now, reminder_time) >= 0;
    }

public:
    NotificationManager() { load(); }

    void load() {
        notes.clear();
        ifstream in("notifications.txt");
        string line;
        while (getline(in, line)) {
            size_t sep = line.find('|');
            if (sep != string::npos) {
                notes.push_back({ line.substr(0, sep), line.substr(sep + 1) });
            } else {
                notes.push_back({ line, "" });
            }
        }
        in.close();
    }

    void save() {
        ofstream out("notifications.txt");
        for (auto& n : notes)
            out << n.message << "|" << n.reminder << "\n";
        out.close();
    }

    void showDueReminders() {
        for (auto& n : notes)
            if (isReminderDue(n.reminder)) {
                cout << "REMINDER: " << n.message << " (" << n.reminder << ")\n";
                playNotificationSound();
            }
    }

    void menu() {
        int choice;
        string line, timeStr;
        while (true) {
            cout << "\n----- NOTIFICATIONS -----\n";
            cout << "1. View\n2. Add\n3. Delete\n4. Clear All\n5. Back\nChoice: ";
            if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                if (notes.empty()) cout << "No notifications.\n";
                else for (int i = 0; i < notes.size(); i++)
                    cout << i + 1 << ". " << notes[i].message << (notes[i].reminder.empty() ? "" : " | " + notes[i].reminder) << "\n";
            }
            else if (choice == 2) {
                cout << "Enter message: "; getline(cin, line);
                cout << "Enter reminder (YYYY-MM-DD HH:MM) or leave empty: "; getline(cin, timeStr);
                if (!line.empty()) notes.push_back({ line, timeStr });
                playNotificationSound();
            }
            else if (choice == 3) {
                cout << "Enter number: "; int num; if (!(cin >> num)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (num > 0 && num <= notes.size()) { notes.erase(notes.begin() + num - 1); playNotificationSound(); }
            }
            else if (choice == 4) { notes.clear(); playNotificationSound(); }
            else if (choice == 5) { save(); return; }
        }
    }
};

// ================= FLASHCARDS =================
class Flashcards {
private:
    vector<string> questions;
    vector<string> answers;

    string normalize(const string& s) {
        string res;
        for (char c : s) if (!isspace(c)) res += tolower(c);
        return res;
    }

public:
    void loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) { cout << "No previous flashcard data found!\n"; return; }
        string q, a;
        while (getline(fin, q) && getline(fin, a)) { questions.push_back(q); answers.push_back(a); }
        fin.close(); cout << "Flashcards loaded.\n";
    }

    void saveToFile(const string& filename) {
        ofstream fout(filename);
        for (size_t i = 0; i < questions.size(); i++) { fout << questions[i] << "\n" << answers[i] << "\n"; }
        fout.close(); cout << "Flashcards saved.\n";
    }

    void addCard() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string q, a;
        cout << "Enter question: "; getline(cin, q);
        cout << "Enter answer: "; getline(cin, a);
        questions.push_back(q); answers.push_back(a);
        cout << "Flashcard added!\n";
    }

    void showAll() {
        if (questions.empty()) { cout << "No flashcards available.\n"; return; }
        cout << "\n--- All Flashcards ---\n";
        for (size_t i = 0; i < questions.size(); i++)
            cout << "\nQ" << i + 1 << ": " << questions[i] << "\nA: " << answers[i] << "\n";
    }

    void startQuiz() {
        if (questions.empty()) { cout << "No flashcards available to quiz!\n"; return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        vector<int> order(questions.size());
        for (size_t i = 0; i < order.size(); i++) order[i] = i;
        shuffle(order.begin(), order.end(), mt19937(static_cast<unsigned>(chrono::high_resolution_clock::now().time_since_epoch().count())));
        string user; int score = 0;
        cout << "\n--- Flashcard Quiz ---\n";
        for (size_t idx : order) {
            cout << "\nQ: " << questions[idx] << "\nYour answer: ";
            getline(cin, user);
            if (normalize(user) == normalize(answers[idx])) { cout << "Correct!\n"; score++; }
            else cout << "Wrong! Correct answer: " << answers[idx] << "\n";
        }
        cout << "\nQuiz finished! Score: " << score << "/" << questions.size() << "\n";
    }

    void menu() {
        string filename = "flashcards.txt"; int c;
        while (true) {
            cout << "\n----- FLASHCARDS MENU -----\n";
            cout << "1. Load From File\n2. Save To File\n3. Add Card\n4. Show All\n5. Start Quiz\n6. Back\nEnter choice: ";
            if (!(cin >> c)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
            switch (c) {
            case 1: loadFromFile(filename); break;
            case 2: saveToFile(filename); break;
            case 3: addCard(); break;
            case 4: showAll(); break;
            case 5: startQuiz(); break;
            case 6: return;
            default: cout << "Invalid option.\n";
            }
        }
    }
};

// ================= POMODORO TIMER =================
class PomodoroTimer {
private:
    int focus = 25;
    int shortBreak = 5;
    int longBreak = 15;
public:
    void start(ProgressManager& pm) {
        int cycles = 0;
        cout << "Enter number of Pomodoro cycles: "; if (!(cin >> cycles)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }
        if (cycles >= 4) { cout << "Enter long break (minutes): "; if (!(cin >> longBreak)) { longBreak = 15; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); } }

        for (int s = 1; s <= cycles; s++) {
            cout << "\nFocus Session " << s << " (" << focus << " min)\n";
            for (int i = focus * 60; i >= 0; i--) { cout << "\rTime left: " << i / 60 << "m " << i % 60 << "s " << flush; this_thread::sleep_for(chrono::seconds(1)); }
            playNotificationSound();
            pm.addPomodoro();

            if (s != cycles) {
                int brk = (s % 4 == 0) ? longBreak : shortBreak;
                cout << "\nBreak (" << brk << " min)\n";
                for (int i = brk * 60; i >= 0; i--) { cout << "\rBreak: " << i / 60 << "m " << i % 60 << "s " << flush; this_thread::sleep_for(chrono::seconds(1)); }
                playNotificationSound();
            }
        }
        cout << "\nAll Pomodoro cycles complete!\n";
    }
};

// ================= TO-DO LIST =================
class TodoList {
private:
    vector<string> tasks;
public:
    void load() { string t; ifstream in("tasks.txt"); while (getline(in, t)) tasks.push_back(t); in.close(); }
    void save() { ofstream out("tasks.txt"); for (auto& t : tasks) out << t << "\n"; out.close(); }

    void menu(ProgressManager& pm) {
        load();
        int choice;
        string task;
        while (true) {
            cout << "\n---- TO-DO LIST ----\n1. View Tasks\n2. Add Task\n3. Mark Task Done\n4. Back\nChoice: ";
            if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (choice == 1) {
                if (tasks.empty()) cout << "No tasks.\n"; else { for (int i = 0; i < tasks.size(); i++) cout << i + 1 << ". " << tasks[i] << "\n"; }
            }
            else if (choice == 2) {
                cout << "Enter task: "; getline(cin, task); if (!task.empty()) { tasks.push_back(task); pm.addTask(); cout << "Added!\n"; }
            }
            else if (choice == 3) {
                cout << "Enter task number: "; int num; if (!(cin >> num)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (num > 0 && num <= tasks.size()) { cout << "Completed: " << tasks[num - 1] << "\n"; tasks.erase(tasks.begin() + num - 1); }
            }
            else if (choice == 4) { save(); break; }
        }
        save();
    }
};

// ================= MAIN =================
int main() {
    ProgressManager pm;
    NotificationManager nm;
    Flashcards fc;
    PomodoroTimer pt;
    TodoList tdl;

    clearScreen();
    header();
    nm.showDueReminders(); // Show due reminders at startup

    int select;
    while (true) {
        cout << "\n1. Pomodoro Timer\n2. To-do List\n3. Flashcards\n4. Motivational Quote\n5. Your Progress\n6. Notifications\n7. Quit\nEnter: ";
        if (!(cin >> select)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        clearScreen(); header();
        switch (select) {
        case 1: pt.start(pm); break;
        case 2: tdl.menu(pm); break;
        case 3: fc.menu(); break;
        case 4: {
            vector<string> quotes = { "Push yourself.", "Discipline today = success tomorrow.", "Small steps daily lead to greatness." };
            mt19937 rng(static_cast<unsigned>(chrono::high_resolution_clock::now().time_since_epoch().count()));
            uniform_int_distribution<int> dist(0, quotes.size() - 1);
            cout << "\n\"" << quotes[dist(rng)] << "\"\n"; break;
        }
        case 5: pm.display(); break;
        case 6: nm.menu(); break;
        case 7: cout << "Goodbye!\n"; return 0;
        default: cout << "Invalid option.\n";
        }
        cout << "\nPress Enter to continue..."; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
    }
}
