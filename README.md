
**Overview**

The Task Manager is a simple command-line application written in C++ that allows users to manage their tasks efficiently. This application supports functionalities like adding, removing, modifying, displaying, searching, saving, and loading tasks. The tasks can also be marked as recurring and can be categorized with tags.

**Features**

Add Tasks: Enter task title, description, deadline, priority (high, medium, low), and tags.

Remove Tasks: Delete a task by its title.

Modify Tasks: Update the details of an existing task.

Display Tasks: View all tasks in a structured format.

Search Tasks: Find tasks by title or keyword in the description.

Save and Load Tasks: Persist tasks to a file and load them when the application starts.

Undo/Redo: Revert the last action or redo a previously undone action.

Recurring Tasks: Mark tasks as recurring.

**Requirements**

C++11 or later

A C++ compiler (like g++, clang++, etc.)

**Code Structure**

Task Structure: Represents a task with attributes like title, description, deadline, priority, tags, and recurrence.

TaskList Class: Manages a linked list of tasks and provides methods for task operations.

UndoStack Class: Implements an undo/redo mechanism for actions performed on tasks.

Main Function: Handles user input and orchestrates the application flow.

**Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.**

**This project is licensed under the MIT License. See the LICENSE file for details.**

**Thanks to the C++ community for continuous learning and support.**
