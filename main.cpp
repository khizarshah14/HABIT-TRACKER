// habit_tracker_with_flashcards.cpp
// Complete program — copy/paste ready for Visual Studio (and other compilers).

#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#define NOMINMAX                // Prevent Windows.h from defining min/max macros that break std::numeric_limits::max()
#include <Windows.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <sstream>              // for safe parsing
#include <random>

using namespace std;

// ============= CLEAR SCREEN ===================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ============= HEADER ==========================
void header() {
    cout << "=====================================\n";
    cout << "          HABIT TRACKER\n";
    cout << "=====================================\n";
}

// FUNCTION DECLARATIONS
void Timer();
void todoList();
void motivationalQuote();
void yourProgress();
void notification();
string getCurrentDate();
void updateStreak();
void logHistory(int pomodoros, int tasks);
void playNotificationSound();

// =============================================================
//                       FLASHCARDS CLASS
// =============================================================
class Flashcards {
private:
    vector<string> questions;
    vector<string> answers;

public:

    void load_from_file(const string& filename) {
        ifstream fin(filename);

        if (!fin.is_open()) {
            cout << "No previous flashcard data found!\n";
            return;
        }

        string q, a;
        while (getline(fin, q) && getline(fin, a)) {
            questions.push_back(q);
            answers.push_back(a);
        }

        fin.close();
        cout << "Flashcards loaded.\n";
    }

    void save_to_file(const string& filename) {
        ofstream fout(filename);

        for (size_t i = 0; i < questions.size(); i++) {
            fout << questions[i] << "\n";
            fout << answers[i] << "\n";
        }

        fout.close();
        cout << "Flashcards saved.\n";
    }

    void add_card() {
        // flush any leftover input
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        string q, a;
        cout << "Enter question: ";
        getline(cin, q);

        cout << "Enter answer: ";
        getline(cin, a);

        questions.push_back(q);
        answers.push_back(a);

        cout << "Flashcard added!\n";
    }

    void show_all() {
        if (questions.empty()) {
            cout << "No flashcards available.\n";
            return;
        }

        cout << "\n--- All Flashcards ---\n";
        for (size_t i = 0; i < questions.size(); i++) {
            cout << "\nQ" << i + 1 << ": " << questions[i];
            cout << "\nA: " << answers[i] << "\n";
        }
        cout << endl;
    }

    void start_quiz() {
        if (questions.empty()) {
            cout << "No flashcards available to quiz!\n";
            return;
        }

        // make sure input buffer is clean
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        string user;
        int score = 0;

        cout << "\n--- Flashcard Quiz ---\n";

        for (size_t i = 0; i < questions.size(); i++) {
            cout << "\nQ" << i + 1 << ": " << questions[i] << "\n";
            cout << "Your answer: ";
            getline(cin, user);

            if (user == answers[i]) {
                cout << "Correct!\n";
                score++;
            }
            else {
                cout << "Wrong! Correct answer: " << answers[i] << "\n";
            }
        }

        cout << "\nQuiz finished! Score: " << score << "/" << questions.size() << "\n";
    }

    void flashcardMenu() {
        string filename = "flashcards.txt";
        int c;

        while (true) {
            cout << "\n----- FLASHCARDS MENU -----\n";
            cout << "1. Load From File\n";
            cout << "2. Save To File\n";
            cout << "3. Add Card\n";
            cout << "4. Show All\n";
            cout << "5. Start Quiz\n";
            cout << "6. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> c)) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Invalid input. Try again.\n";
                continue;
            }

            switch (c) {
            case 1: load_from_file(filename); break;
            case 2: save_to_file(filename); break;
            case 3: add_card(); break;
            case 4: show_all(); break;
            case 5: start_quiz(); break;
            case 6: return;
            default: cout << "Invalid option.\n";
            }
        }
    }
};

// =============================================================
//                       NOTIFICATION SOUND
// =============================================================
void playNotificationSound() {
    // simple beep sequence (Windows)
#ifdef _WIN32
    Beep(1200, 150);
    Beep(900, 120);
    Beep(1500, 180);
#else
    // fallback beep for non-windows (may not make sound)
    cout << '\a' << flush;
#endif
}

// =============================================================
//                         POMODORO TIMER
// =============================================================
void Timer() {
    cout << "Pomodoro Timer selected.\n";
    int focusmin = 25, breakmin = 5, longbreakmin = 0, cycles = 0;

    cout << "Enter number of Pomodoro cycles: ";
    if (!(cin >> cycles)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid number.\n";
        return;
    }

    if (cycles >= 4) {
        cout << "Enter long break duration (minutes): ";
        if (!(cin >> longbreakmin)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            longbreakmin = 0;
        }
    }

    for (int session = 1; session <= cycles; ++session) {

        cout << "\nFocus Session " << session << " (" << focusmin << " minutes)\n";

        for (int i = focusmin * 60; i >= 0; i--) {
            int m = i / 60;
            int s = i % 60;

            cout << "\rTime left: " << m << "m " << (s < 10 ? "0" : "") << s << "s " << flush;
            this_thread::sleep_for(chrono::seconds(1));
        }

        playNotificationSound();

        if (session != cycles) {
            if (session % 4 == 0) {
                cout << "\nLong break (" << longbreakmin << " min)\n";

                for (int i = longbreakmin * 60; i >= 0; i--) {
                    int m = i / 60, s = i % 60;
                    cout << "\rBreak: " << m << "m " << (s < 10 ? "0" : "") << s << "s " << flush;
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
            else {
                cout << "\nShort break (" << breakmin << " min)\n";

                for (int i = breakmin * 60; i >= 0; i--) {
                    int m = i / 60, s = i % 60;
                    cout << "\rBreak: " << m << "m " << (s < 10 ? "0" : "") << s << "s " << flush;
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
        }

        playNotificationSound();
    }

    cout << "\nAll Pomodoro cycles complete!\n";

    int totalP = 0, tasks = 0;
    ifstream in("progress.txt");
    if (in.is_open()) in >> totalP >> tasks;
    in.close();

    totalP++;

    ofstream out("progress.txt");
    out << totalP << " " << tasks;
    out.close();

    updateStreak();
    logHistory(1, 0);
}

// =============================================================
//                         TODO LIST
// =============================================================
void todoList() {
    cout << "To-do List selected.\n";

    vector<string> tasks;
    string task;
    int choice;

    ifstream infile("tasks.txt");
    while (getline(infile, task)) tasks.push_back(task);
    infile.close();

    while (true) {
        cout << "\n---- TO-DO LIST ----\n";
        cout << "1. View Tasks\n";
        cout << "2. Add Task\n";
        cout << "3. Mark Task Done\n";
        cout << "4. Back\n";
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid entry.\n";
            continue;
        }

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            if (tasks.empty()) cout << "No tasks.\n";
            else {
                for (size_t i = 0; i < tasks.size(); ++i)
                    cout << i + 1 << ". " << tasks[i] << "\n";
            }
        }
        else if (choice == 2) {
            cout << "Enter task: ";
            getline(cin, task);
            if (!task.empty()) tasks.push_back(task);
            cout << "Added!\n";
        }
        else if (choice == 3) {
            cout << "Enter task number: ";
            int num;
            if (!(cin >> num)) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Invalid number.\n";
                continue;
            }
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num > 0 && num <= static_cast<int>(tasks.size())) {
                cout << "Completed: " << tasks[num - 1] << "\n";
                tasks.erase(tasks.begin() + (num - 1));
            }
            else cout << "Invalid.\n";
        }
        else if (choice == 4) break;
        else cout << "Invalid.\n";
    }

    ofstream out("tasks.txt");
    for (auto& t : tasks) out << t << "\n";
    out.close();

    int totalP = 0, completed = 0;
    ifstream prog("progress.txt");
    if (prog.is_open()) prog >> totalP >> completed;
    prog.close();

    completed++;

    ofstream pout("progress.txt");
    pout << totalP << " " << completed;
    pout.close();

    updateStreak();
    logHistory(0, 1);
}

// =============================================================
//                    MOTIVATIONAL QUOTE
// =============================================================
void motivationalQuote() {
    vector<string> q = {
        "Push yourself, because no one else will.",
        "It always seems impossible until it's done.",
        "Discipline today = success tomorrow.",
        "Small steps daily lead to greatness.",
        "Your only limit is your mind."
    };

    // Use C++ random for slightly better randomness
    static std::mt19937 rng(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(0, static_cast<int>(q.size()) - 1);
    int i = dist(rng);

    cout << "\n\"" << q[i] << "\"\n";
}

// =============================================================
//                       PROGRESS SYSTEM
// =============================================================
void yourProgress() {
    cout << "Your Progress:\n";

    int totalP = 0, tasks = 0;

    ifstream file("progress.txt");
    if (file.is_open()) file >> totalP >> tasks;
    file.close();

    cout << "Pomodoros: " << totalP << "\n";
    cout << "Tasks: " << tasks << "\n";

    // ASCII progress bar safely
    int percent = min(100, (totalP + tasks) * 10);
    cout << "\nProgress: [";
    int filled = percent / 5;
    for (int i = 0; i < filled; i++) cout << "#";
    for (int i = filled; i < 20; i++) cout << "-";
    cout << "] " << percent << "%\n";

    // streak
    string last;
    int streak = 0, best = 0;

    ifstream sfile("streak.txt");
    if (sfile.is_open()) sfile >> last >> streak >> best;
    sfile.close();

    cout << "\nCurrent Streak: " << streak;
    cout << "\nBest Streak: " << best << "\n";

    // history (ASCII only)
    ifstream h("history.txt");
    string line;
    vector<string> hist;

    while (getline(h, line)) hist.push_back(line);

    cout << "\nLast 7 days:\n";
    int start = max(0, static_cast<int>(hist.size()) - 7);

    for (int i = start; i < static_cast<int>(hist.size()); i++) {
        cout << hist[i] << "\n";
    }
}

// =============================================================
//                     NOTIFICATIONS
// =============================================================
void notification() {
    vector<string> notes;
    string line;
    int choice;

    ifstream infile("notifications.txt");
    while (getline(infile, line)) notes.push_back(line);
    infile.close();

    while (true) {
        cout << "\n----- NOTIFICATIONS -----\n";
        cout << "1. View\n2. Add\n3. Delete\n4. Clear All\n5. Back\n";
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid entry.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            if (notes.empty()) cout << "No notifications.\n";
            else for (size_t i = 0; i < notes.size(); i++) cout << i + 1 << ". " << notes[i] << "\n";
        }
        else if (choice == 2) {
            cout << "Enter: ";
            getline(cin, line);
            if (!line.empty()) notes.push_back(line);
            playNotificationSound();
        }
        else if (choice == 3) {
            cout << "Enter number: ";
            int num;
            if (!(cin >> num)) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "Invalid number.\n";
                continue;
            }
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num > 0 && num <= static_cast<int>(notes.size())) {
                notes.erase(notes.begin() + (num - 1));
                playNotificationSound();
            } else cout << "Invalid.\n";
        }
        else if (choice == 4) {
            notes.clear();
            playNotificationSound();
        }
        else if (choice == 5) break;
    }

    ofstream out("notifications.txt");
    for (auto& n : notes) out << n << "\n";
    out.close();
}

// =============================================================
//                   DATE + STREAK SYSTEM (safe localtime)
// =============================================================
string getCurrentDate() {
    time_t now = time(nullptr);
    tm local_tm{};
#if defined(_MSC_VER)
    // safe localtime on MSVC
    localtime_s(&local_tm, &now);
#elif defined(__unix__) || defined(__APPLE__)
    localtime_r(&now, &local_tm);
#else
    // fallback (not thread-safe)
    tm* tmp = localtime(&now);
    if (tmp) local_tm = *tmp;
#endif

    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
        1900 + local_tm.tm_year,
        1 + local_tm.tm_mon,
        local_tm.tm_mday
    );

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

// =============================================================
//                      HISTORY LOGGING
// =============================================================
void logHistory(int pomodoros, int tasks) {
    string today = getCurrentDate();
    vector<string> lines;
    string l;

    ifstream infile("history.txt");
    while (getline(infile, l)) lines.push_back(l);
    infile.close();

    bool found = false;

    for (string& entry : lines) {
        if (entry.size() >= 10 && entry.substr(0, 10) == today) {
            // safer parse using istringstream
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

// =============================================================
//                        MAIN FUNCTION
// =============================================================
int main() {
    // seed randomness
    std::mt19937 rng(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    int select;

    while (true) {
        clearScreen();
        header();
        cout << "1. Pomodoro Timer\n";
        cout << "2. To-do List\n";
        cout << "3. Flashcards\n";
        cout << "4. Motivational Quote\n";
        cout << "5. Your Progress\n";
        cout << "6. Notifications\n";
        cout << "7. Quit\n";
        cout << "\nEnter: ";
        if (!(cin >> select)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid choice.\n";
            continue;
        }

        clearScreen();
        header();

        switch (select) {
        case 1: Timer(); break;
        case 2: todoList(); break;
        case 3: {
            Flashcards fc;
            fc.flashcardMenu();
            break;
        }
        case 4: motivationalQuote(); break;
        case 5: yourProgress(); break;
        case 6: notification(); break;
        case 7:
            cout << "Goodbye!\n";
            return 0;
        default:
            cout << "Invalid option.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin.get();
    }
}
