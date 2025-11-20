#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <vector>
#include <fstream>
using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header() {
    cout << "=====================================\n";
    cout << "     🕓 HABIT TRACKER    \n";
    cout << "=====================================\n";
}

void Timer();
void todoList();
void flashcards();
void motivationalQuote();
void yourProgress();
void notification();
string getCurrentDate();
void updateStreak();
void logHistory(int pomodoros, int tasks);
void playNotificationSound();

int main() {
    int selection;

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
        cout << "\nEnter your selection (1-7): ";
        cin >> selection;

        clearScreen();
        header();

        switch (selection) {
            case 1: Timer(); break;
            case 2: todoList(); break;
            case 3: flashcards(); break;
            case 4: motivationalQuote(); break;
            case 5: yourProgress(); break;
            case 6: notification(); break;
            case 7:
                cout << "Quitting the app... Goodbye! 👋\n";
                return 0;
            default:
                cout << "Invalid selection! Please choose between 1–7.\n";
                break;
        }

        cout << "\nPress Enter to return to main menu...\n";
        cin.ignore();
        cin.get();
    }

    return 0;
}
void playNotificationSound() {
    Beep(1200, 150);   // High beep
    Beep(900, 120);    // Medium beep
    Beep(1500, 180);   // High beep
}


void Timer() {
    cout << "⏳ Pomodoro Timer selected.\n";
    cout << "Work for 25 minutes, then take a 5-minute break.\n";
    int focusmin = 25, breakmin = 5, longbreakmin = 0, cycles = 0;
	cout << "Enter number of Pomodoro cycles: ";
	cin >> cycles;
	if (cycles >= 4)
	{
		cout << "Enter long break duration (minutes): ";
		cin >> longbreakmin;
	}

		for (int session = 1; session <= cycles; ++session) {
			cout << "\n🕒 Focus Session " << session
				<< " Started (" << focusmin << " minutes)\n";
		for (int i = focusmin * 60; i >= 0; --i) {
			int mins = i / 60;
			int secs = i % 60;
		cout << "\rTime left: " << mins << "m "
			<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
		this_thread::sleep_for(chrono::seconds(1));
		}


		playNotificationSound();   // When a break ends


		if (session % 4 == 0 && session != cycles) {
			cout << "🎉 Time for a LONG break! ("
				<< longbreakmin << " minutes)\n";
			for (int i = longbreakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;
			cout << "\rLong break left: " << mins << "m "
				<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
			this_thread::sleep_for(chrono::seconds(1));
			}
		}
		else if (session != cycles) {
			cout << "Focus complete! Take a short break ("
				<< breakmin << " minutes).\n";
			for (int i = breakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;
			cout << "\rBreak left: " << mins << "m "
				<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
			this_thread::sleep_for(chrono::seconds(1));
			}

		}

		playNotificationSound();   // When a break ends

		if (session != cycles)
			cout << "Break finished! Get ready for the next Pomodoro.\n";
		}

		cout << "\n🏁 All Pomodoro cycles complete! Well done!\n";
		// Update progress
        int totalPomodoro = 0, completedTasks = 0;
        ifstream prog("progress.txt");
        if (prog.is_open()) prog >> totalPomodoro >> completedTasks;
        prog.close();

        totalPomodoro++;

        ofstream pout("progress.txt");
        pout << totalPomodoro << " " << completedTasks;
        pout.close();
		updateStreak();
		logHistory(1, 0);  // 1 pomodoro added today
		



}

void todoList() {
    cout << "📝 To-do List selected.\n";
    cout << "Add and check off tasks to stay productive!\n";
    vector<string> tasks;
		string task;
		int choice;
		ifstream infile("tasks.txt");

		// Load existing tasks
		while (getline(infile, task)) tasks.push_back(task);
		infile.close();

		while (true) {
			cout << "\n---- TO-DO LIST ----\n";
			cout << "1. View Tasks\n";
			cout << "2. Add Task\n";
			cout << "3. Mark Task Done\n";
			cout << "4. Back to Main Menu\n";
			cout << "Enter choice: ";
			cin >> choice;
			cin.ignore();

			if (choice == 1) {
				cout << "\nYour Tasks:\n";
				if (tasks.empty()) {
					cout << "No tasks yet!\n";
				}
				for (int i = 0; i < (int)tasks.size(); ++i)
					cout << i + 1 << ". " << tasks[i] << "\n";

			}
			else if (choice == 2) {
				cout << "Enter new task: ";
				getline(cin, task);
				tasks.push_back(task);
				cout << "Task added!\n";

			}
			else if (choice == 3) {
				cout << "Enter task number to mark done: ";
				int num; cin >> num;

				if (num > 0 && num <= (int)tasks.size()) {
					cout << "Task \"" << tasks[num - 1] << "\" completed!\n";
					tasks.erase(tasks.begin() + num - 1);
				}
				else {
					cout << "Invalid task number!\n";
				}

			}
			else if (choice == 4) {
				break;

			}
			else {
				cout << "Invalid option.\n";
			}
		}

		// Save tasks
		ofstream outfile("tasks.txt");
		for (auto &t : tasks) outfile << t << endl;
		outfile.close();

		// Update progress
        int totalPomodoro = 0, completedTasks = 0;
        ifstream prog("progress.txt");
        if (prog.is_open()) prog >> totalPomodoro >> completedTasks;
        prog.close();

        completedTasks++;

        ofstream pout("progress.txt");
        pout << totalPomodoro << " " << completedTasks;
        pout.close();
		updateStreak();
		logHistory(0, 1);  // 1 completed task added today



}

void flashcards() {
    cout << "📚 Flashcards selected.\n";
    cout << "Review key concepts easily!\n";
}

void motivationalQuote() {
    cout << "💬 Motivational Quote selected.\n";
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

	srand(time(0));
	int randomIndex = rand() % quotes.size();

	cout << "\n💡 Motivational Quote 💡\n";
	cout << "\"" << quotes[randomIndex] << "\"\n";
}

void yourProgress() {
    cout << "📈 Your Progress selected.\n";

    int totalPomodoro = 0, completedTasks = 0;

    // Read existing progress
    ifstream file("progress.txt");
    if (file.is_open()) {
        file >> totalPomodoro >> completedTasks;
        file.close();
    }

    cout << "\n----- YOUR PROGRESS -----\n";

    cout << "Total Pomodoro Sessions Completed: "
         << totalPomodoro << "\n";

    cout << "Total Tasks Completed: "
         << completedTasks << "\n";

    int totalScore = totalPomodoro + completedTasks;
    int progressPercent = (totalScore > 0) ? min(100, totalScore * 10) : 0;

    cout << "\nProgress Bar:\n[";
    int bars = progressPercent / 5;
    for (int i = 0; i < bars; i++) cout << "#";
    for (int i = bars; i < 20; i++) cout << "-";
    cout << "] " << progressPercent << "%\n";

    cout << "\nKeep going — you're doing great! 🌟\n";
// ===== Streak System =====
    string lastDate;
    int streak = 0, best = 0;

    ifstream sfile("streak.txt");
    if (sfile.is_open()) {
        sfile >> lastDate >> streak >> best;
        sfile.close();
    }

    cout << "\n🔥 DAILY STREAKS 🔥\n";
    cout << "Current Streak: "
         << streak << " days\n";
    cout << "Longest Streak: "
         << best << " days\n";
    cout << "Last Activity: "
         << lastDate << "\n";
	
	// ===================== GRAPH SECTION ========================
    cout << "\n📊 Weekly Productivity Graph\n";

    vector<string> history;
    ifstream hfile("history.txt");
    string hline;
    while (getline(hfile, hline)) history.push_back(hline);
    hfile.close();

    // Get last 7 entries only
    vector<string> last7;
    for (int i = max(0, (int)history.size() - 7); i < history.size(); i++)
        last7.push_back(history[i]);

    if (last7.empty()) {
        cout << "No data to show graph yet.\n";
    } else {
     for (string l : last7) {
        string date = l.substr(5); // show DD-MM
        date = l.substr(5, 5);     // MM-DD format

        int p, t;
        sscanf(l.c_str(), "%*s %d %d", &p, &t);

        cout << "\n" << date << "\n";

        cout << " Pomodoros: ";
        for (int i = 0; i < p; i++) cout << "█";
        cout << " (" << p << ")\n";

        cout << " Tasks    : ";
        for (int i = 0; i < t; i++) cout << "█";
        cout << " (" << t << ")\n";
        }
    }


}


void notification() {
    vector<string> notes;
    string line;
    int choice;

    // Load existing notifications
    ifstream infile("notifications.txt");
    while (getline(infile, line))
        notes.push_back(line);
    infile.close();

    while (true) {
        cout << "\n----- NOTIFICATIONS -----\n";
        cout << "1. View Notifications\n";
        cout << "2. Add Notification\n";
        cout << "3. Delete Notification\n";
        cout << "4. Clear All\n";
        cout << "5. Back to Main Menu\n";

        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "\nYour Notifications:\n";
            if (notes.empty())
                cout << "No notifications yet!\n";
            else {
                for (int i = 0; i < notes.size(); i++)
                    cout << i + 1 << ". " << notes[i] << "\n";
            }
        }
        else if (choice == 2) {
            cout << "Enter new notification: ";
            getline(cin, line);
			notes.push_back(line);
            playNotificationSound(); // Sound when new notification is added
            cout << "Notification added!\n";

        }
        else if (choice == 3) {
            cout << "Enter notification number to delete: ";
            int num;
            cin >> num;

            if (num > 0 && num <= (int)notes.size()) {
				playNotificationSound();
                cout << "Deleted: " << notes[num - 1] << "\n";
                notes.erase(notes.begin() + num - 1);
            }
            else {
                cout << "Invalid number!\n";
            }
        }
        else if (choice == 4) {
            notes.clear();
			playNotificationSound();
            cout << "All notifications cleared!\n";
        }
        else if (choice == 5) {
            break;
        }
        else {
            cout << "Invalid input!\n";
        }
    }

    // Save notifications
    ofstream outfile("notifications.txt");
    for (auto &n : notes)
        outfile << n << endl;
    outfile.close();





}
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(buffer);
}
void updateStreak() {
    string today = getCurrentDate();
    string lastDate;
    int currentStreak = 0, longestStreak = 0;

    ifstream file("streak.txt");
    if (file.is_open()) {
        file >> lastDate >> currentStreak >> longestStreak;
        file.close();
    } else {
        lastDate = "";
    }

    if (lastDate == today) {
        // Already updated today → do nothing
    } 
    else {
        // Check if yesterday was last activity
        tm last_tm = {};
        sscanf(lastDate.c_str(), "%d-%d-%d",
               &last_tm.tm_year, &last_tm.tm_mon, &last_tm.tm_mday);

        last_tm.tm_year -= 1900;
        last_tm.tm_mon -= 1;

        time_t last_time = mktime(&last_tm);
        time_t now = time(0);

        double diffDays = difftime(now, last_time) / (60 * 60 * 24);

        if (lastDate != "" && diffDays <= 2.0)
            currentStreak++;  // Continue streak
        else
            currentStreak = 1;  // New streak
    }

    if (currentStreak > longestStreak)
        longestStreak = currentStreak;

    ofstream out("streak.txt");
    out << today << " " << currentStreak << " " << longestStreak;
    out.close();
}
void logHistory(int pomodoros, int tasks) {
    string today = getCurrentDate();

    vector<string> lines;
    ifstream infile("history.txt");
    string line;
    while (getline(infile, line)) lines.push_back(line);
    infile.close();

    bool updated = false;

    for (string &l : lines) {
        if (l.substr(0, 10) == today) {
            int p, t;
            sscanf(l.c_str(), "%*s %d %d", &p, &t);

            p += pomodoros;
            t += tasks;

            l = today + " " + to_string(p) + " " + to_string(t);
            updated = true;
            break;
        }
    }

    if (!updated) {
        lines.push_back(today + " " + to_string(pomodoros) + " " + to_string(tasks));
    }

    ofstream outfile("history.txt");
    for (auto &x : lines) outfile << x << endl;
    outfile.close();
}


