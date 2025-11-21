# Habit Tracker and Flashcards – C++ Console Application

This project is a complete productivity assistant built in C++, combining a pomodoro timer, a to-do list system, flashcards with quiz functionality, motivational quotes, progress tracking, and lightweight notifications. Everything runs locally in a simple console interface and stores data in text files for easy persistence and portability.

## Overview

The application is designed as an all-in-one daily productivity tool. It allows users to manage tasks, study using flashcards, track habits, and monitor long-term progress. The structure focuses on clarity, reliability, and modularity, making it easy to extend or integrate additional features.

## Key Features
### Pomodoro Timer

The timer supports:

User-defined number of work cycles

Optional long breaks every 4 sessions

A real-time countdown display

Sound notifications on session completion

Automatic updates to progress statistics

### To-Do List

The task manager includes:

Adding new tasks

Viewing existing tasks

Marking tasks as completed

Automatic saving into a text file

Updating daily streak and progress count when tasks are done

### Flashcards Learning System

A complete built-in flashcard module supporting:

Adding new cards

Viewing all cards

Loading cards from a file

Saving cards to a file

An interactive quiz mode that validates answers and displays corrections

### Motivational Quotes

Provides short single-line motivational statements selected randomly to maintain user motivation.

### Progress Tracking

The tracker displays:

Total pomodoro sessions completed

Total tasks finished

ASCII based progress bar

Current streak and best streak

Seven-day history log of tasks and pomodoros

### Notifications

A simple reminder system allowing:

Adding notifications

Viewing the full list

Deleting individual items

Clearing all notifications

Automatic file storage

### File Structure

The program automatically maintains the following files in the working directory:

flashcards.txt – stores flashcard question-answer pairs
tasks.txt – stores user tasks
progress.txt – records total tasks and pomodoros
streak.txt – stores the user's daily streak data
history.txt – chronological record of completed tasks and pomodoros
notifications.txt – list of custom user notifications

These files allow long-term persistence across program runs without a database.

## Technical Design

The program relies entirely on C++ standard libraries and basic Windows functionality for optional sound effects.
Key areas of implementation include:

Vectors for storing and managing lists

fstream for robust file input/output

chrono and thread for timing operations

numeric_limits and input flushing for safe user input

Structured grouping of features into independent modules

A dedicated Flashcards class to encapsulate core study logic

## Usage

The program runs in an interactive loop with a numbered main menu. Users select the desired feature and follow the prompts. All input validation and file operations are handled internally.

## Extensibility

The project is designed for expansion. Possible future improvements include:

Theme support using ANSI colors

Multiple flashcard decks

Timer pause and resume function

JSON or CSV based data storage

Analytics for weekly and monthly performance

Ability to export progress reports

Project Image Reference
