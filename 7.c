
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#define DESCRIPTION_SIZE 100
#define COMMAND_SIZE 32
#define INITIAL_CAPACITY 4

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef enum {
    PENDING,
    DONE
} TaskStatus;

typedef struct {
    int id;
    char description[DESCRIPTION_SIZE];
    TaskStatus status;
} Task;

typedef enum {
    INPUT_EOF,
    INPUT_OK,
    INPUT_TOO_LONG
} InputResult;

InputResult readLine(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL)
        return INPUT_EOF;

    length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        ch = getchar();

        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF)
                ch = getchar();

            printf("Error: Input is too long.\n");
            return INPUT_TOO_LONG;
        }
    }

    length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\r')
        buffer[length - 1] = '\0';

    return INPUT_OK;
}

void trimWhitespace(char *str)
{
    char *start = str;
    char *end;

    while (isspace((unsigned char)*start))
        start++;

    if (start != str)
        memmove(str, start, strlen(start) + 1);

    end = str + strlen(str);

    while (end > str && isspace((unsigned char)*(end - 1)))
        end--;

    *end = '\0';
}

int equalsIgnoreCase(const char *a, const char *b)
{
    while (*a && *b) {
        if (toupper((unsigned char)*a) !=
            toupper((unsigned char)*b))
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

CommandType getCommandType(const char *command)
{
    if (equalsIgnoreCase(command, "ADD"))
        return CMD_ADD;

    if (equalsIgnoreCase(command, "DELETE"))
        return CMD_DELETE;

    if (equalsIgnoreCase(command, "VIEW"))
        return CMD_VIEW;

    if (equalsIgnoreCase(command, "UPDATE"))
        return CMD_UPDATE;

    if (equalsIgnoreCase(command, "COMPLETE"))
        return CMD_COMPLETE;

    if (equalsIgnoreCase(command, "FIND"))
        return CMD_FIND;

    if (equalsIgnoreCase(command, "HELP"))
        return CMD_HELP;

    if (equalsIgnoreCase(command, "EXIT"))
        return CMD_EXIT;

    return CMD_UNKNOWN;
}

int searchTask(Task tasks[], int count, int id)
{
    int i;

    for (i = 0; i < count; i++) {
        if (tasks[i].id == id)
            return i;
    }

    return -1;
}

int readTaskID(int *id)
{
    char buffer[50];
    char *end;
    long value;
    InputResult result;

    result = readLine("Enter task ID: ", buffer, sizeof(buffer));

    if (result == INPUT_EOF)
        return 0;

    if (result == INPUT_TOO_LONG)
        return 1;

    trimWhitespace(buffer);

    if (buffer[0] == '\0') {
        printf("Error: Task ID cannot be empty.\n");
        return 1;
    }

    errno = 0;
    value = strtol(buffer, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (errno == ERANGE || end == buffer ||
        *end != '\0' || value <= 0 || value > INT_MAX) {
        printf("Error: Enter a valid positive integer ID.\n");
        return 1;
    }

    *id = (int)value;
    return 1;
}

void displayTask(const Task *task)
{
    printf("\nTask ID     : %d\n", task->id);
    printf("Description : %s\n", task->description);

    printf("Status      : %s\n",
           task->status == PENDING ? "PENDING" : "DONE");
}

int addTask(Task **tasks, int *count, int *capacity)
{
    int id;
    char description[DESCRIPTION_SIZE];
    InputResult result;
    Task *temp;

    if (!readTaskID(&id))
        return 0;

    if (searchTask(*tasks, *count, id) != -1) {
        printf("Error: Task ID already exists.\n");
        return 1;
    }

    result = readLine("Enter task description: ",
                      description, sizeof(description));

    if (result == INPUT_EOF)
        return 0;

    if (result == INPUT_TOO_LONG)
        return 1;

    trimWhitespace(description);

    if (description[0] == '\0') {
        printf("Error: Description cannot be empty.\n");
        return 1;
    }

    if (*count == *capacity) {
        int newCapacity;

        if (*capacity > (int)(SIZE_MAX / sizeof(Task)) / 2) {
            printf("Error: Maximum memory capacity reached.\n");
            return 1;
        }

        newCapacity = (*capacity == 0) ?
                      INITIAL_CAPACITY : (*capacity * 2);

        temp = realloc(*tasks, (size_t)newCapacity * sizeof(Task));

        if (temp == NULL) {
            printf("Error: Memory allocation failed.\n");
            return 1;
        }

        *tasks = temp;
        *capacity = newCapacity;
    }

    (*tasks)[*count].id = id;
    strcpy((*tasks)[*count].description, description);
    (*tasks)[*count].status = PENDING;

    (*count)++;

    printf("Task %d added successfully.\n", id);

    return 1;
}

int deleteTask(Task tasks[], int *count)
{
    int id, index, i;

    if (!readTaskID(&id))
        return 0;

    index = searchTask(tasks, *count, id);

    if (index == -1) {
        printf("Error: Task %d not found.\n", id);
        return 1;
    }

    for (i = index; i < *count - 1; i++)
        tasks[i] = tasks[i + 1];

    (*count)--;

    printf("Task %d deleted successfully.\n", id);

    return 1;
}

void viewTasks(Task tasks[], int count)
{
    int i;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }

    printf("\n========== TASK LIST ==========\n");

    for (i = 0; i < count; i++) {
        displayTask(&tasks[i]);
        printf("-------------------------------\n");
    }
}

int updateTask(Task tasks[], int count)
{
    int id, index;
    char description[DESCRIPTION_SIZE];
    InputResult result;

    if (!readTaskID(&id))
        return 0;

    index = searchTask(tasks, count, id);

    if (index == -1) {
        printf("Error: Task %d not found.\n", id);
        return 1;
    }

    result = readLine("Enter new description: ",
                      description, sizeof(description));

    if (result == INPUT_EOF)
        return 0;

    if (result == INPUT_TOO_LONG)
        return 1;

    trimWhitespace(description);

    if (description[0] == '\0') {
        printf("Error: Description cannot be empty.\n");
        return 1;
    }

    strcpy(tasks[index].description, description);

    printf("Task %d updated successfully.\n", id);

    return 1;
}

int completeTask(Task tasks[], int count)
{
    int id, index;

    if (!readTaskID(&id))
        return 0;

    index = searchTask(tasks, count, id);

    if (index == -1) {
        printf("Error: Task %d not found.\n", id);
        return 1;
    }

    tasks[index].status = DONE;

    printf("Task %d marked as DONE.\n", id);

    return 1;
}

int findTask(Task tasks[], int count)
{
    int id, index;

    if (!readTaskID(&id))
        return 0;

    index = searchTask(tasks, count, id);

    if (index == -1)
        printf("Task %d not found.\n", id);
    else
        displayTask(&tasks[index]);

    return 1;
}

void showHelp(void)
{
    printf("\n========== AVAILABLE COMMANDS ==========\n");
    printf("ADD      - Add a new task\n");
    printf("DELETE   - Delete a task using its ID\n");
    printf("VIEW     - Display all tasks\n");
    printf("UPDATE   - Change a task description\n");
    printf("COMPLETE - Mark a task as DONE\n");
    printf("FIND     - Search for a task by ID\n");
    printf("HELP     - Display available commands\n");
    printf("EXIT     - Exit the program\n");
    printf("========================================\n");
}

int main(void)
{
    Task *tasks = NULL;
    int count = 0;
    int capacity = 0;
    int running = 1;

    char command[COMMAND_SIZE];
    InputResult result;
    CommandType commandType;

    printf("====================================\n");
    printf("       TASK LIST MANAGER\n");
    printf("====================================\n");

    showHelp();

    while (running) {
        result = readLine("\nEnter command: ",
                          command, sizeof(command));

        if (result == INPUT_EOF) {
            printf("\nInput ended. Closing program.\n");
            break;
        }

        if (result == INPUT_TOO_LONG)
            continue;

        trimWhitespace(command);

        if (command[0] == '\0') {
            printf("Error: Command cannot be empty.\n");
            continue;
        }

        commandType = getCommandType(command);

        switch (commandType) {

            case CMD_ADD:
                running = addTask(&tasks, &count, &capacity);
                break;

            case CMD_DELETE:
                running = deleteTask(tasks, &count);
                break;

            case CMD_VIEW:
                viewTasks(tasks, count);
                break;

            case CMD_UPDATE:
                running = updateTask(tasks, count);
                break;

            case CMD_COMPLETE:
                running = completeTask(tasks, count);
                break;

            case CMD_FIND:
                running = findTask(tasks, count);
                break;

            case CMD_HELP:
                showHelp();
                break;

            case CMD_EXIT:
                printf("Exiting Task List Manager...\n");
                running = 0;
                break;

            case CMD_UNKNOWN:
                printf("Error: Unknown command. Type HELP for assistance.\n");
                break;
        }
    }

    free(tasks);
    tasks = NULL;

    printf("Memory released. Program terminated.\n");

    return 0;
}
