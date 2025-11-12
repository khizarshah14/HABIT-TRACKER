#include <iostream>
#include <cstdlib>
#include <string>
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
}

void todoList() {
    cout << "📝 To-do List selected.\n";
    cout << "Add and check off tasks to stay productive!\n";
}

void flashcards() {
    cout << "📚 Flashcards selected.\n";
    cout << "Review key concepts easily!\n";
}

void motivationalQuote() {
    cout << "💬 Motivational Quote selected.\n";
    cout << "\"Success is the sum of small efforts repeated day in and day out.\"\n";
}

void yourProgress() {
    cout << "📈 Your Progress selected.\n";
    cout << "Keep going — you’re improving every day!\n";
}

void notification() {
    cout << "🔔 Notifications selected.\n";
    cout << "Stay updated with your reminders.\n";
}
