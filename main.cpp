#include <iostream>
#include <cstdlib>
#include <string>
#include<chrono>
#include<thread>
#include<Windows.h>
#include<vector>
#include<fstream>
using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header() {
    cout << "\033[1;36m";
    cout << "=====================================\n";
    cout << "     🕓 HABIT TRACKER    \n";
    cout << "=====================================\n";
    cout << "\033[0m";
}

void Timer();
void todoList();
void flashcards();
void motivationalQuote();
void yourProgress();
void notification();

int main() {
    int selection;

    while (true) {
        clearScreen();
        header();
        cout << "\033[1;33m";
        cout << "1. Pomodoro Timer\n";
        cout << "2. To-do List\n";
        cout << "3. Flashcards\n";
        cout << "4. Motivational Quote\n";
        cout << "5. Your Progress\n";
        cout << "6. Notifications\n";
        cout << "7. Quit\n";
        cout << "\033[0m";
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
                cout << "\033[1;31mQuitting the app... Goodbye! 👋\033[0m\n";
                return 0;
            default:
                cout << "\033[1;31mInvalid selection! Please choose between 1–7.\033[0m\n";
                break;
        }

        cout << "\n\033[1;34mPress Enter to return to main menu...\033[0m";
        cin.ignore();
        cin.get();
    }

    return 0;
}

void Timer() {
    cout << "⏳ Pomodoro Timer selected.\n";
    cout << "Work for 25 minutes, then take a 5-minute break.\n";
    int focusmin = 25, breakmin = 5, longbreakmin = 0, cycles = 0;
	int totalFocusSec = focusmin * 60;
	cout << "Enter number of Pomodoro cycles: ";
	cin >> cycles;
	if (cycles >= 4)
	{
		cout << "Enter long break duration (minutes): ";
		cin >> longbreakmin;
	}

	for (int session = 1; session <= cycles; ++session) {
		cout << "\n\033[1;36m🕒 Focus Session " << session
			<< " Started (" << focusmin << " minutes)\033[0m\n";
		for (int i = focusmin * 60; i >= 0; --i) {
			int mins = i / 60;
			int secs = i % 60;
			cout << "\rTime left: " << mins << "m "
				<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
			this_thread::sleep_for(chrono::seconds(1));
		}


		Beep(750, 300); // Alert after focus session

		if (session % 4 == 0 && session != cycles) {
			cout << "\033[1;34m🎉 Time for a LONG break! ("
				<< longbreakmin << " minutes)\033[0m\n";
			for (int i = longbreakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;
				cout << "\rLong break left: " << mins << "m "
					<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
				this_thread::sleep_for(chrono::seconds(1));
			}
		}
		else if (session != cycles) {
			cout << "\033[1;32mFocus complete! Take a short break ("
				<< breakmin << " minutes).\033[0m\n";
			for (int i = breakmin * 60; i >= 0; --i) {
				int mins = i / 60;
				int secs = i % 60;
				cout << "\rBreak left: " << mins << "m "
					<< (secs < 10 ? "0" : "") << secs << "s   " << flush;
				this_thread::sleep_for(chrono::seconds(1));
			}

		}

		Beep(1000, 300); // Alert after break
		if (session != cycles)
			cout << "\033[1;33mBreak finished! Get ready for the next Pomodoro.\033[0m\n";
		}

		cout << "\n\033[1;35m🏁 All Pomodoro cycles complete! Well done!\033[0m\n";
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
			cout << "\n\033[1;34m---- TO-DO LIST ----\033[0m\n";  // Blue title
			cout << "\033[1;36m1. View Tasks\033[0m\n";
			cout << "\033[1;36m2. Add Task\033[0m\n";
			cout << "\033[1;36m3. Mark Task Done\033[0m\n";
			cout << "\033[1;36m4. Back to Main Menu\033[0m\n";
			cout << "Enter choice: ";
			cin >> choice;
			cin.ignore();

			if (choice == 1) {
				cout << "\n\033[1;33mYour Tasks:\033[0m\n"; // Yellow
				if (tasks.empty()) {
					cout << "\033[1;31mNo tasks yet!\033[0m\n"; // Red
				}
				for (int i = 0; i < (int)tasks.size(); ++i)
					cout << "\033[1;32m" << i + 1 << ". " << tasks[i] << "\033[0m\n"; // Green

			}
			else if (choice == 2) {
				cout << "\033[1;35mEnter new task: \033[0m";
				getline(cin, task);
				tasks.push_back(task);
				cout << "\033[1;32mTask added!\033[0m\n"; // Green

			}
			else if (choice == 3) {
				cout << "\033[1;35mEnter task number to mark done: \033[0m";
				int num; cin >> num;

				if (num > 0 && num <= (int)tasks.size()) {
					cout << "\033[1;32mTask \"" << tasks[num - 1] << "\" completed!\033[0m\n";
					tasks.erase(tasks.begin() + num - 1);
				}
				else {
					cout << "\033[1;31mInvalid task number!\033[0m\n";
				}

			}
			else if (choice == 4) {
				break;

			}
			else {
				cout << "\033[1;31mInvalid option.\033[0m\n";
			}
		}

		// Save tasks
		ofstream outfile("tasks.txt");
		for (auto &t : tasks) outfile << t << endl;
		outfile.close();
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

	cout << "\n\033[1;33m💡 Motivational Quote 💡\033[0m\n"; // Yellow title
	cout << "\033[1;32m\"" << quotes[randomIndex] << "\"\033[0m\n"; // Green quote
}

void yourProgress() {
    cout << "📈 Your Progress selected.\n";
    cout << "Keep going — you’re improving every day!\n";
}

void notification() {
    cout << "🔔 Notifications selected.\n";
    cout << "Stay updated with your reminders.\n";
}
