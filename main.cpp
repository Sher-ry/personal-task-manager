#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>


using namespace std;

// Task structure
struct Task {
    string title;
    string description;
    time_t deadline;
    string priority;
    vector<string> tags;
    bool recurring;
    Task* next;

    Task(string t, string d, time_t dl, string p, vector<string> tg, bool r)
        : title(t), description(d), deadline(dl), priority(p), tags(tg), recurring(r), next(nullptr) {}
};

// Linked List for tasks
class TaskList {
private:
    Task* head;

public:
    TaskList() : head(nullptr) {}

    void addTask(const string& title, const string& description, time_t deadline, const string& priority, const vector<string>& tags, bool recurring) {
        Task* newTask = new Task(title, description, deadline, priority, tags, recurring);
        if (head == nullptr || deadline < head->deadline) {
            newTask->next = head;
            head = newTask;
        } else {
            Task* current = head;
            while (current->next != nullptr && current->next->deadline <= deadline) {
                current = current->next;
            }
            newTask->next = current->next;
            current->next = newTask;
        }
    }

    void removeTask(const string& title) {
        Task* current = head;
        Task* previous = nullptr;

        while (current != nullptr && current->title != title) {
            previous = current;
            current = current->next;
        }

        if (current != nullptr) {
            if (previous == nullptr)
                head = current->next;
            else
                previous->next = current->next;
            delete current;
            cout << "Task \"" << title << "\" removed successfully." << endl;
        } else {
            cout << "Task \"" << title << "\" not found!" << endl;
        }
    }

    void modifyTask(const string& title, const string& newTitle, const string& newDescription, time_t newDeadline, const string& newPriority, const vector<string>& newTags) {
        Task* current = head;

        while (current != nullptr) {
            if (current->title == title) {
                current->title = newTitle;
                current->description = newDescription;
                current->deadline = newDeadline;
                current->priority = newPriority;
                current->tags = newTags;
                cout << "Task \"" << title << "\" modified successfully." << endl;
                return;
            }
            current = current->next;
        }
        cout << "Task \"" << title << "\" not found!" << endl;
    }

    void displayTasks() {
        Task* current = head;
        if (current == nullptr) {
            cout << "No tasks available." << endl;
            return;
        }
        cout << "\nTasks List:\n";
        while (current != nullptr) {
            cout << "--------------------------------------" << endl;
            cout << "Title: " << current->title
                 << "\nDescription: " << current->description
                 << "\nDeadline: " << put_time(localtime(&current->deadline), "%Y-%m-%d %H:%M")
                 << "\nPriority: " << current->priority
                 << "\nTags: ";
            for (const auto& tag : current->tags) {
                cout << tag << " ";
            }
            cout << "\nRecurring: " << (current->recurring ? "Yes" : "No") << "\n" << endl;
            current = current->next;
        }
        cout << "--------------------------------------" << endl;
    }

    void saveTasks(const string& filename) {
        ofstream file(filename, ios::out);
        if (!file) {
            cout << "Error opening file for writing!" << endl;
            return;
        }

        Task* current = head;
        while (current != nullptr) {
            file << current->title << "|"
                 << current->description << "|"
                 << current->deadline << "|"
                 << current->priority << "|"
                 << (current->tags.empty() ? "" : current->tags[0]);
            for (size_t i = 1; i < current->tags.size(); ++i) {
                file << "," << current->tags[i];
            }
            file << "|" << current->recurring << endl;
            current = current->next;
        }
        file.close();
        cout << "Tasks saved to " << filename << endl;
    }

    void loadTasks(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cout << "No existing tasks file found." << endl;
            return;
        }

        // Clear existing tasks
        while (head != nullptr) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }

        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);
            size_t pos4 = line.find('|', pos3 + 1);
            size_t pos5 = line.find('|', pos4 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos && pos4 != string::npos) {
                string title = line.substr(0, pos1);
                string description = line.substr(pos1 + 1, pos2 - pos1 - 1);
                time_t deadline = stoll(line.substr(pos2 + 1, pos3 - pos2 - 1));
                string priority = line.substr(pos3 + 1, pos4 - pos3 - 1);

                vector<string> tags;
                string tagsString = line.substr(pos4 + 1, pos5 - pos4 - 1);
                stringstream ss(tagsString);
                string tag;
                while (getline(ss, tag, ',')) {
                    tags.push_back(tag);
                }

                bool recurring = line.substr(pos5 + 1) == "1";
                addTask(title, description, deadline, priority, tags, recurring);
            }
        }
        file.close();
        cout << "Tasks loaded from " << filename << endl;
    }

    Task* getHead() { return head; }

    // Search for tasks by title or keyword in description
    void searchTasks(const string& keyword) {
        Task* current = head;
        bool found = false;
        cout << "\nSearch Results for \"" << keyword << "\":\n";
        while (current != nullptr) {
            if (current->title.find(keyword) != string::npos ||
                current->description.find(keyword) != string::npos) {
                cout << "--------------------------------------" << endl;
                cout << "Title: " << current->title
                     << "\nDescription: " << current->description
                     << "\nDeadline: " << put_time(localtime(&current->deadline), "%Y-%m-%d %H:%M")
                     << "\nPriority: " << current->priority
                     << "\nTags: ";
                for (const auto& tag : current->tags) {
                    cout << tag << " ";
                }
                cout << "\nRecurring: " << (current->recurring ? "Yes" : "No") << "\n" << endl;
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No tasks found matching the keyword." << endl;
        }
        cout << "--------------------------------------" << endl;
    }
};

// Stack for undo/redo operations
class UndoStack {
private:
    struct StackNode {
        string action;  
        Task* task;
        StackNode* next;
    };
    StackNode* top;
    StackNode* redoStack;

public:
    UndoStack() : top(nullptr), redoStack(nullptr) {}

    void push(const string& action, Task* task) {
        StackNode* newNode = new StackNode{action,
            new Task(task->title, task->description, task->deadline, task->priority, task->tags, task->recurring), nullptr};
        newNode->next = top;
        top = newNode;
        // Clear redo stack on new action
        while (redoStack != nullptr) {
            StackNode* temp = redoStack;
            redoStack = redoStack->next;
            delete temp->task;
            delete temp;
        }
    }

    StackNode* pop() {
        if (top == nullptr) {
            cout << "No actions to undo." << endl;
            return nullptr;
        }
        StackNode* temp = top;
        top = top->next;
        return temp;
    }

    void pushRedo(const string& action, Task* task) {
        StackNode* newNode = new StackNode{action,
            new Task(task->title, task->description, task->deadline, task->priority, task->tags, task->recurring), nullptr};
        newNode->next = redoStack;
        redoStack = newNode;
    }

    StackNode* popRedo() {
        if (redoStack == nullptr) {
            cout << "No actions to redo." << endl;
            return nullptr;
        }
        StackNode* temp = redoStack;
        redoStack = redoStack->next;
        return temp;
    }
};

// Function to display a decorative header
void displayHeader() {
    cout << "\n======================" << endl;
    cout << "    Task Manager      " << endl;
    cout << "======================" << endl;
}

int main(int argc, char* argv[]) {
    TaskList taskList;
    UndoStack undoStack;
    string filename = "tasks.txt";

    // Load existing tasks if any
    taskList.loadTasks(filename);

    if (argc > 1) {
        filename = argv[1];
        taskList.loadTasks(filename);
    }

    int choice;
    do {
        displayHeader();
        cout << "1. Add Task\n";
        cout << "2. Remove Task\n";
        cout << "3. Modify Task\n";
        cout << "4. Display Tasks\n";
        cout << "5. Search Tasks\n";
        cout << "6. Save Tasks\n";
        cout << "7. Load Tasks\n";
        cout << "8. Undo Last Action\n";
        cout << "9. Redo Last Action\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                string title, description, priority, tagsInput;
                cout << "Enter task title: ";
                getline(cin, title);
                cout << "Enter task description: ";
                getline(cin, description);

                cout << "Enter deadline (YYYY-MM-DD HH:MM): ";
                string deadlineStr;
                getline(cin, deadlineStr);

                struct tm tm = {};
                istringstream ss(deadlineStr);
                ss >> get_time(&tm, "%Y-%m-%d %H:%M");

                if (ss.fail()) {
                    cout << "Invalid date format. Please use YYYY-MM-DD HH:MM" << endl;
                    break;
                }

                time_t deadline = mktime(&tm);
                cout << "Enter priority (high, medium, low): ";
                getline(cin, priority);
                cout << "Enter tags (comma-separated): ";
                getline(cin, tagsInput);

                vector<string> tags;
                stringstream tagStream(tagsInput);
                string tag;
                while (getline(tagStream, tag, ',')) {
                    tags.push_back(tag);
                }

                bool recurring = false;
                cout << "Is this a recurring task? (yes/no): ";
                string recurringInput;
                getline(cin, recurringInput);
                if (recurringInput == "yes") {
                    recurring = true;
                }

                taskList.addTask(title, description, deadline, priority, tags, recurring);
                undoStack.push("add", new Task(title, description, deadline, priority, tags, recurring));
                cout << "Task added successfully!" << endl;
                break;
            }
            case 2: {
                string title;
                cout << "Enter task title to remove: ";
                getline(cin, title);
                Task* current = taskList.getHead();
                while (current != nullptr) {
                    if (current->title == title) {
                        undoStack.push("remove", current);
                        break;
                    }
                    current = current->next;
                }
                taskList.removeTask(title);
                break;
            }
            case 3: {
                string title, newTitle, newDescription, newPriority, tagsInput;
                cout << "Enter task title to modify: ";
                getline(cin, title);
                cout << "Enter new task title: ";
                getline(cin, newTitle);
                cout << "Enter new task description: ";
                getline(cin, newDescription);

                cout << "Enter new deadline (YYYY-MM-DD HH:MM): ";
                string newDeadlineStr;
                getline(cin, newDeadlineStr);

                struct tm newTm = {};
                istringstream newSs(newDeadlineStr);
                newSs >> get_time(&newTm, "%Y-%m-%d %H:%M");

                if (newSs.fail()) {
                    cout << "Invalid date format. Please use YYYY-MM-DD HH:MM" << endl;
                    break;
                }

                time_t newDeadline = mktime(&newTm);
                cout << "Enter new priority (high, medium, low): ";
                getline(cin, newPriority);
                cout << "Enter new tags (comma-separated): ";
                getline(cin, tagsInput);

                vector<string> newTags;
                stringstream newTagStream(tagsInput);
                string newTag;
                while (getline(newTagStream, newTag, ',')) {
                    newTags.push_back(newTag);
                }

                taskList.modifyTask(title, newTitle, newDescription, newDeadline, newPriority, newTags);
                break;
            }
            case 4: {
                taskList.displayTasks();
                break;
            }
            case 5: {
                string keyword;
                cout << "Enter keyword to search: ";
                getline(cin, keyword);
                taskList.searchTasks(keyword);
                break;
            }
            case 6: {
                taskList.saveTasks(filename);
                break;
            }
            case 7: {
                taskList.loadTasks(filename);
                break;
            }
            case 8: {
                auto undoNode = undoStack.pop();
                if (undoNode != nullptr) {
                    if (undoNode->action == "add") {
                        taskList.removeTask(undoNode->task->title);
                    } else if (undoNode->action == "remove") {
                        taskList.addTask(undoNode->task->title,
                                         undoNode->task->description,
                                         undoNode->task->deadline,
                                         undoNode->task->priority,
                                         undoNode->task->tags,
                                         undoNode->task->recurring);
                    }
                    delete undoNode->task;
                    delete undoNode;
                    cout << "Last action undone successfully!" << endl;
                }
                break;
            }
            case 9: {
                auto redoNode = undoStack.popRedo();
                if (redoNode != nullptr) {
                    if (redoNode->action == "add") {
                        taskList.addTask(redoNode->task->title,
                                         redoNode->task->description,
                                         redoNode->task->deadline,
                                         redoNode->task->priority,
                                         redoNode->task->tags,
                                         redoNode->task->recurring);
                    } else if (redoNode->action == "remove") {
                        taskList.removeTask(redoNode->task->title);
                    }
                    delete redoNode->task;
                    delete redoNode;
                    cout << "Last action redone successfully!" << endl;
                }
                break;
            }
            case 0:
                cout << "Saving tasks before exit..." << endl;
                taskList.saveTasks(filename);
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 0);

    return 0;
}