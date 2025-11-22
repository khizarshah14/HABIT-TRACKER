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

/* ============================================================
                      UTILITY FUNCTIONS
   ============================================================ */

// Clear terminal screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Display header banner
void header() {
    cout << "=====================================\n";
    cout << "          HABIT TRACKER\n";
    cout << "=====================================\n";
}

/* ============================================================
                       DATE & STREAK SYSTEM
   ============================================================ */

// Returns the current date in YYYY-MM-DD format
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
        local_tm.tm_mday
    );

    return string(buffer);
}

// Increase streak if today's date is new
void updateStreak() {
    string today = getCurrentDate();
    string lastDate;
    int streak = 0, best = 0;

    // Load streak data
    ifstream in("streak.txt");
    if (in.is_open()) in >> lastDate >> streak >> best;
    in.close();

    // Update only if a new day
    if (today != lastDate) {
        streak++;
        if (streak > best) best = streak;
    }

    // Save new streak data
    ofstream out("streak.txt");
    out << today << " " << streak << " " << best;
    out.close();
}

// Log daily pomodoros + tasks in history
void logHistory(int pomodoros, int tasks) {
    string today = getCurrentDate();
    vector<string> lines;
    string line;

    // Load previous history
    ifstream infile("history.txt");
    while (getline(infile, line)) lines.push_back(line);
    infile.close();

    bool found = false;

    // Update today's entry if exists
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
};

    // Otherwise create new record
    if (!found)
        lines.push_back(today + " " + to_string(pomodoros) + " " + to_string(tasks));

    // Save updated history
    ofstream out("history.txt");
    for (auto& x : lines) out << x << "\n";
    out.close();
}

/* ============================================================
                       NOTIFICATION SOUND
   ============================================================ */

// Play a beep sound (Windows only)
void playNotificationSound() {
#ifdef _WIN32
    Beep(1200, 150);
    Beep(900, 120);
    Beep(1500, 180);
#else
    cout << '\a' << flush;
#endif
}

/* ============================================================
                        PROGRESS MANAGER
   ============================================================ */

class ProgressManager {
private:
    int pomodoros = 0;
    int tasks = 0;

public:

    // Constructor loads progress
    ProgressManager() { load(); }

    // Load data from file
    void load() {
        ifstream in("progress.txt");
        if (in.is_open()) in >> pomodoros >> tasks;
        in.close();
    }

    // Save data
    void save() {
        ofstream out("progress.txt");
        out << pomodoros << " " << tasks;
        out.close();
    }

    // Increase counters + record streak + history
    void addPomodoro() { pomodoros++; save(); updateStreak(); logHistory(1, 0); }
    void addTask() { tasks++; save(); updateStreak(); logHistory(0, 1); }

    // Display progress info
    void display() {
        cout << "Pomodoros: " << pomodoros << "\n";
        cout << "Tasks: " << tasks << "\n";

        // Progress bar
        int percent = min(100, (pomodoros + tasks) * 10);
        int filled = percent / 5;

        cout << "\nProgress: [";
        for (int i = 0; i < filled; i++) cout << "#";
        for (int i = filled; i < 20; i++) cout << "-";
        cout << "] " << percent << "%\n";

        // Streak info
        string last;
        int streak = 0, best = 0;

        ifstream sfile("streak.txt");
        if (sfile.is_open()) sfile >> last >> streak >> best;
        sfile.close();

        cout << "\nCurrent Streak: " << streak;
        cout << "\nBest Streak: " << best << "\n";

        // History last 7 days
        cout << "\nLast 7 Days:\n";
        ifstream h("history.txt");

        vector<string> hist;
        while (getline(h, last)) hist.push_back(last);

        int start = max(0, (int)hist.size() - 7);
        for (int i = start; i < hist.size(); i++) cout << hist[i] << "\n";
    }
};

/* ============================================================
                        NOTIFICATION MANAGER
   ============================================================ */

class NotificationManager {
private:

    // Single notification
    struct Note {
        string message;
        string reminder;  // Format: YYYY-MM-DD HH:MM
    };

    vector<Note> notes;

    // Returns true if reminder time has passed
    bool isReminderDue(const string& r) {
        if (r.empty()) return false;

        // Current time
        time_t now = time(nullptr);
        tm local_tm{};

#ifdef _MSC_VER
        localtime_s(&local_tm, &now);
#else
        localtime_r(&now, &local_tm);
#endif

        // Parse reminder
        int y, m, d, hh, mm;

        if (sscanf(r.c_str(), "%d-%d-%d %d:%d", &y, &m, &d, &hh, &mm) != 5)
            return false;

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

    // Load saved notifications
    NotificationManager() { load(); }

    void load() {
        notes.clear();
        ifstream in("notifications.txt");

        string line;
        while (getline(in, line)) {
            size_t sep = line.find('|');

            if (sep != string::npos)
                notes.push_back({ line.substr(0, sep), line.substr(sep + 1) });
            else
                notes.push_back({ line, "" });
        }

        in.close();
    }

    // Save notifications
    void save() {
        ofstream out("notifications.txt");

        for (auto& n : notes)
            out << n.message << "|" << n.reminder << "\n";

        out.close();
    }

    // Show reminders that are due
    void showDueReminders() {
        for (auto& n : notes)
            if (isReminderDue(n.reminder)) {
                cout << "REMINDER: " << n.message << " (" << n.reminder << ")\n";
                playNotificationSound();
            }
    }

    // User menu
    void menu() {
        int choice;
        string line, timeStr;

        while (true) {
            cout << "\n----- NOTIFICATIONS -----\n";
            cout << "1. View\n2. Add\n3. Delete\n4. Clear All\n5. Back\nChoice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // View saved
            if (choice == 1) {
                if (notes.empty())
                    cout << "No notifications.\n";
                else
                    for (int i = 0; i < notes.size(); i++)
                        cout << i + 1 << ". " << notes[i].message
                        << (notes[i].reminder.empty() ? "" : " | " + notes[i].reminder)
                        << "\n";
            }

            // Add new
            else if (choice == 2) {
                cout << "Enter message: ";
                getline(cin, line);

                cout << "Enter reminder (YYYY-MM-DD HH:MM) or leave empty: ";
                getline(cin, timeStr);

                if (!line.empty())
                    notes.push_back({ line, timeStr });

                playNotificationSound();
            }

            // Delete single
            else if (choice == 3) {
                int num;
                cout << "Enter number: ";

                if (!(cin >> num)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (num > 0 && num <= notes.size()) {
                    notes.erase(notes.begin() + num - 1);
                    playNotificationSound();
                }
            }

            // Clear all
            else if (choice == 4) {
                notes.clear();
                playNotificationSound();
            }

            // Exit menu
            else if (choice == 5) {
                save();
                return;
            }
        }
    }
};

/* ============================================================
                          MOTIVATIONAL QUOTE
   ============================================================ */

void motivationalQuote() {
    cout << "Motivational Quote selected.\n";

    vector<string> quotes = {
        "Push yourself, because no one else is going to do it for you.",
        "It always seems impossible until it’s done.",
        "Don’t watch the clock; do what it does. Keep going.",
        "Success is the sum of small efforts repeated day in and day out.",
        "Discipline is choosing between what you want now and what you want most.",
        "Believe you can and you're halfway there.",
        "Small steps every day lead to big results.",
        "Your only limit is your mind.",
        "Dream big. Start small. Act now.",
        "Success doesn’t come from what you do occasionally, it comes from what you do consistently.",
        "Stay focused and never give up.",
        "Great things never come from comfort zones.",
        "Work hard in silence, let success make the noise.",
        "Progress is progress, no matter how small.",
        "You don’t have to be perfect, just consistent."
    };

    // Random index
    srand(time(0));
    int randomIndex = rand() % quotes.size();

    cout << "\n Motivational Quote \n";
    cout << quotes[randomIndex] << "\n";
}

/* ============================================================
                         FLASHCARDS SYSTEM
   ============================================================ */

class Flashcards {
private:
    vector<string> questions;
    vector<string> answers;

    // Normalize strings: lowercase + remove whitespace
    string normalize(const string& s) {
        string res;
        for (char c : s)
            if (!isspace(c))
                res += tolower(c);
        return res;
    }

public:
    // Load flashcards from file
    void loadFromFile(const string& filename = "flashcards.txt") {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cout << "No previous flashcard data found!\n";
            return;
        }

        questions.clear();
        answers.clear();

        string q, a;
        while (getline(fin, q) && getline(fin, a)) {
            if (!q.empty() && !a.empty()) {
                questions.push_back(q);
                answers.push_back(a);
            }
        }

        fin.close();
        cout << "Flashcards loaded.\n";
    }

    // Save flashcards to file
    void saveToFile(const string& filename = "flashcards.txt") {
        ofstream fout(filename);
        if (!fout.is_open()) {
            cout << "Error opening file for writing!\n";
            return;
        }

        for (size_t i = 0; i < questions.size(); i++) {
            fout << questions[i] << "\n" << answers[i] << "\n";
        }

        fout.close();
        cout << "Flashcards saved.\n";
    }

    // Add a new flashcard
    void addCard() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        string q, a;
        cout << "Enter question: ";
        getline(cin, q);

        cout << "Enter answer: ";
        getline(cin, a);

        if (!q.empty() && !a.empty()) {
            questions.push_back(q);
            answers.push_back(a);
            cout << "Flashcard added!\n";
        } else {
            cout << "Question and answer cannot be empty!\n";
        }
    }

    // Show all flashcards
    void showAll() {
        if (questions.empty()) {
            cout << "No flashcards available.\n";
            return;
        }

        cout << "\n--- All Flashcards ---\n";
        for (size_t i = 0; i < questions.size(); i++) {
            cout << "\nQ" << i + 1 << ": " << questions[i] << "\n";
            cout << "A: " << answers[i] << "\n";
        }
    }

    // Start a quiz
    void startQuiz() {
        if (questions.empty()) {
            cout << "No flashcards available to quiz!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        vector<int> order(questions.size());
        for (size_t i = 0; i < order.size(); i++) order[i] = i;

        shuffle(order.begin(), order.end(),
            mt19937(static_cast<unsigned>(chrono::high_resolution_clock::now().time_since_epoch().count())));

        int score = 0;
        string userAnswer;

        cout << "\n--- Flashcard Quiz ---\n";
        for (size_t idx : order) {
            cout << "\nQ: " << questions[idx] << "\nYour answer: ";
            getline(cin, userAnswer);

            if (normalize(userAnswer) == normalize(answers[idx])) {
                cout << "Correct!\n";
                score++;
            } else {
                cout << "Wrong! Correct answer: " << answers[idx] << "\n";
            }
        }

        cout << "\nQuiz finished! Score: " << score << "/" << questions.size() << "\n";
    }

    // Flashcards main menu
    void menu() {
        string filename = "flashcards.txt";
        int choice;

        while (true) {
            cout << "\n----- FLASHCARDS MENU -----\n";
            cout << "1. Load From File\n2. Save To File\n3. Add Card\n4. Show All\n5. Start Quiz\n6. Back\nEnter choice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice) {
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

/* ============================================================
                          POMODORO TIMER
   ============================================================ */

void Timer() {
   cout << "Pomodoro Timer selected.\n";
	cout << "Work for 25 minutes, then take a 5-minute break.\n";

	int focusmin = 25;
	int breakmin = 5;
	int longbreakmin = 0;
	int cycles = 0;

	cout << "Enter number of Pomodoro cycles: ";
	cin >> cycles;

	// Long break input
	if (cycles >= 4) {
		cout << "Enter long break duration (15-30 minutes): ";
		cin >> longbreakmin;
		while (longbreakmin < 15 || longbreakmin > 30)
		{
			cout << "Enter long break duration (15-30 minutes): ";
			cin >> longbreakmin;
		}
	}

	// Run all sessions
	for (int session = 1; session <= cycles; ++session) {

		// Focus session
		cout << "\nFocus Session " << session << " Started (" << focusmin << " minutes)\n";

		for (int i = focusmin * 60; i >= 0; --i) {
			int mins = i / 60;
			int secs = i % 60;

			cout << "\rTime left: " << mins << "m " << (secs < 10 ? "0" : "") << secs << "s   " << flush;
			this_thread::sleep_for(chrono::seconds(1));
		}

		Beep(750, 300);

		// Long break every 4th session
		if (session % 4 == 0 && session != cycles) {
			cout << "Time for a LONG break! (" << longbreakmin << " minutes)\n";

			for (int i = longbreakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;

				cout << "\rLong break left: " << mins << "m " << (secs < 10 ? "0" : "") << secs << "s   " << flush;
				this_thread::sleep_for(chrono::seconds(1));
			}
		}

		// Short break otherwise
		else if (session != cycles) {
			cout << "Focus Session complete! Take a short break (" << breakmin << " minutes)\n";

			for (int i = breakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;

				cout << "\rBreak left: " << mins << "m " << (secs < 10 ? "0" : "") << secs << "s   " << flush;
				this_thread::sleep_for(chrono::seconds(1));
			}
		}

		Beep(1000, 300);

		if (session != cycles)
			cout << "Break finished! Get ready for the next Pomodoro.\n";
	}

	cout << "\n All Pomodoro cycles complete! Well done!\n";
}

/* ============================================================
                           TO-DO LIST
   ============================================================ */

void todoList() {
    cout << "To-do List selected.\n";
    cout << "Add and check off tasks to stay productive!\n";

    vector<string> tasks;
    string task;
    int choice;

    // Load existing tasks
    ifstream infile("tasks.txt");
    while (getline(infile, task))
        tasks.push_back(task);
    infile.close();

    // To-do list menu
    while (true) {

        cout << "\n---- TO-DO LIST ----\n";
        cout << "1. View Tasks\n";
        cout << "2. Add Task\n";
        cout << "3. Mark Task Done\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter choice: ";

        cin >> choice;
        cin.ignore();

        // Show tasks
        if (choice == 1) {
            cout << "\nYour Tasks:\n";

            if (tasks.empty()) {
                cout << "No tasks yet!\n";
            }
            else {
                for (int i = 0; i < tasks.size(); ++i)
                    cout  << i + 1 << ". " << tasks[i] << "\n";
            }
        }

        // Add new task
        else if (choice == 2) {
            cout << "Enter new task: ";
            getline(cin, task);
            tasks.push_back(task);

            cout << "Task added!\n";
        }

        // Complete a task
        else if (choice == 3) {
            int num;
            cout << "Enter task number to mark done: ";
            cin >> num;

            if (num > 0 && num <= tasks.size()) {
                cout << "Task \"" << tasks[num - 1] << "\" completed!\n";
                tasks.erase(tasks.begin() + num - 1);
            }
            else {
                cout << "Invalid task number!\n";
            }
        }

        // Exit menu
        else if (choice == 4) {
            break;
        }

        else {
            cout << "Invalid option.\n";
        }
    }

    // Save tasks back to file
    ofstream outfile("tasks.txt");
    for (auto& t : tasks)
        outfile << t << endl;

    outfile.close();
}

/* ============================================================
                            MAIN PROGRAM
   ============================================================ */

int main() {
    ProgressManager pm;
    NotificationManager nm;
    Flashcards fc;

    clearScreen();
    header();

    // Display due reminders
    nm.showDueReminders();

    int select;

    while (true) {
        cout << "\n1. Pomodoro Timer\n2. To-do List\n3. Flashcards\n4. Motivational Quote\n5. Your Progress\n6. Notifications\n7. Quit\nEnter: ";

        if (!(cin >> select)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        clearScreen();
        header();

        switch (select) {
        case 1:
            Timer();
            pm.addPomodoro();
            break;

        case 2:
            todoList();
            pm.addTask();
            break;

        case 3:
            fc.menu();
            break;

        case 4:
            motivationalQuote();
            break;

        case 5:
            pm.display();
            break;

        case 6:
            nm.menu();
            break;

        case 7:
            cout << "Goodbye!\n";
            return 0;

        default:
            cout << "Invalid option.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}

