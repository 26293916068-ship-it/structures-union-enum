
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>

#define NAME_SIZE 50
#define INPUT_SIZE 256

// Structure definition
typedef struct Student {
    int rollNo;
    char name[NAME_SIZE];
    int age;
    float GPA;
} Student;

Student *allocateStudents(size_t capacity);
int growArray(Student **students, size_t *capacity);
int readLine(const char *prompt, char *buffer, size_t size);
int readInteger(const char *prompt, int *value);
int readGPA(const char *prompt, float *value);
int readName(char *name);
int findStudent(Student *students, size_t count, int rollNo);
int addStudent(Student **students, size_t *count, size_t *capacity);
void displayStudents(Student *students, size_t count);
void searchStudent(Student *students, size_t count);
void updateStudent(Student *students, size_t count);
void deleteStudent(Student *students, size_t *count);
void displaySummary(Student *students, size_t count);
void sortStudents(Student *students, size_t count);
void freeStudents(Student *students);

Student *allocateStudents(size_t capacity) {
    if (capacity == 0 || capacity > SIZE_MAX / sizeof(Student))
        return NULL;

    return malloc(capacity * sizeof(Student));
}

int growArray(Student **students, size_t *capacity) {
    size_t newCapacity;
    Student *temp;

    if (*capacity == 0)
        newCapacity = 1;
    else {
        if (*capacity > SIZE_MAX / 2)
            return 0;
        newCapacity = *capacity * 2;
    }

    if (newCapacity > SIZE_MAX / sizeof(Student))
        return 0;

    temp = realloc(*students, newCapacity * sizeof(Student));

    if (temp == NULL)
        return 0;

    *students = temp;
    *capacity = newCapacity;

    return 1;
}

int readLine(const char *prompt, char *buffer, size_t size) {
    size_t len;
    int ch;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        ch = getchar();

        if (ch != EOF) {
            while (ch != '\n' && ch != EOF)
                ch = getchar();

            printf("Input is too long. Try again.\n");
            return -1;
        }
    }

    return 1;
}

int readInteger(const char *prompt, int *value) {
    char buffer[INPUT_SIZE];
    char *end;
    long number;
    int status;

    status = readLine(prompt, buffer, sizeof(buffer));

    if (status != 1)
        return status;

    errno = 0;
    number = strtol(buffer, &end, 10);

    while (*end == ' ' || *end == '\t')
        end++;

    if (buffer[0] == '\0' || *end != '\0' ||
        errno == ERANGE || number < INT_MIN || number > INT_MAX) {
        printf("Invalid integer input.\n");
        return -1;
    }

    *value = (int)number;
    return 1;
}

int readGPA(const char *prompt, float *value) {
    char buffer[INPUT_SIZE];
    char *end;
    float number;
    int status;

    status = readLine(prompt, buffer, sizeof(buffer));

    if (status != 1)
        return status;

    errno = 0;
    number = strtof(buffer, &end);

    while (*end == ' ' || *end == '\t')
        end++;

    if (buffer[0] == '\0' || *end != '\0' ||
        errno == ERANGE || !isfinite(number) ||
        number < 0.0f || number > 4.0f) {
        printf("Invalid GPA! Enter a value between 0.00 and 4.00.\n");
        return -1;
    }

    *value = number;
    return 1;
}

int readName(char *name) {
    char buffer[INPUT_SIZE];
    int status;
    size_t len;

    status = readLine("Enter student name: ", buffer, sizeof(buffer));

    if (status != 1)
        return status;

    len = strlen(buffer);

    if (len == 0 || len >= NAME_SIZE) {
        printf("Name must contain 1 to 49 characters.\n");
        return -1;
    }

    strcpy(name, buffer);
    return 1;
}

int findStudent(Student *students, size_t count, int rollNo) {
    size_t i;

    for (i = 0; i < count; i++) {
        if (students[i].rollNo == rollNo)
            return (int)i;
    }

    return -1;
}

int addStudent(Student **students, size_t *count,
               size_t *capacity) {
    Student newStudent;

    if (readInteger("Enter roll number: ", &newStudent.rollNo) != 1)
        return 0;

    if (newStudent.rollNo <= 0) {
        printf("Roll number must be positive.\n");
        return 0;
    }

    if (findStudent(*students, *count, newStudent.rollNo) != -1) {
        printf("Error: Duplicate roll number!\n");
        return 0;
    }

    if (readName(newStudent.name) != 1)
        return 0;

    if (readInteger("Enter age: ", &newStudent.age) != 1)
        return 0;

    if (newStudent.age < 16 || newStudent.age > 100) {
        printf("Age must be between 16 and 100.\n");
        return 0;
    }

    if (readGPA("Enter GPA: ", &newStudent.GPA) != 1)
        return 0;

    if (*count == *capacity) {
        if (!growArray(students, capacity)) {
            printf("Memory allocation failed!\n");
            return 0;
        }

        printf("Array capacity increased successfully.\n");
    }

    (*students)[*count] = newStudent;
    (*count)++;

    printf("Student added successfully!\n");
    return 1;
}

void displayStudents(Student *students, size_t count) {
    size_t i;

    if (count == 0) {
        printf("No student records available.\n");
        return;
    }

    printf("\n%-10s %-20s %-10s %-10s\n",
           "Roll No", "Name", "Age", "GPA");

    printf("--------------------------------------------------\n");

    for (i = 0; i < count; i++) {
        printf("%-10d %-20s %-10d %-10.2f\n",
               students[i].rollNo,
               students[i].name,
               students[i].age,
               students[i].GPA);
    }
}

void searchStudent(Student *students, size_t count) {
    int rollNo, index;

    if (count == 0) {
        printf("Student collection is empty.\n");
        return;
    }

    if (readInteger("Enter roll number to search: ", &rollNo) != 1)
        return;

    index = findStudent(students, count, rollNo);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    printf("\nStudent Found:\n");
    printf("Roll Number: %d\n", students[index].rollNo);
    printf("Name: %s\n", students[index].name);
    printf("Age: %d\n", students[index].age);
    printf("GPA: %.2f\n", students[index].GPA);
}

void updateStudent(Student *students, size_t count) {
    int rollNo, index;
    Student updated;

    if (count == 0) {
        printf("Student collection is empty.\n");
        return;
    }

    if (readInteger("Enter roll number to update: ", &rollNo) != 1)
        return;

    index = findStudent(students, count, rollNo);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    updated = students[index];

    printf("Enter updated details:\n");

    if (readName(updated.name) != 1)
        return;

    if (readInteger("Enter new age: ", &updated.age) != 1)
        return;

    if (updated.age < 16 || updated.age > 100) {
        printf("Invalid age!\n");
        return;
    }

    if (readGPA("Enter new GPA: ", &updated.GPA) != 1)
        return;

    students[index] = updated;

    printf("Student details updated successfully!\n");
}

void deleteStudent(Student *students, size_t *count) {
    int rollNo, index;
    size_t i;

    if (*count == 0) {
        printf("Student collection is empty.\n");
        return;
    }

    if (readInteger("Enter roll number to delete: ", &rollNo) != 1)
        return;

    index = findStudent(students, *count, rollNo);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    for (i = (size_t)index; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }

    (*count)--;

    printf("Student deleted successfully!\n");
}

void displaySummary(Student *students, size_t count) {
    size_t i, highest = 0, lowest = 0;
    double total = 0.0;

    if (count == 0) {
        printf("No records available for GPA analysis.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += students[i].GPA;

        if (students[i].GPA > students[highest].GPA)
            highest = i;

        if (students[i].GPA < students[lowest].GPA)
            lowest = i;
    }

    printf("\n----- ACADEMIC SUMMARY -----\n");

    printf("Highest GPA: %s (%.2f)\n",
           students[highest].name, students[highest].GPA);

    printf("Lowest GPA: %s (%.2f)\n",
           students[lowest].name, students[lowest].GPA);

    printf("Average GPA: %.2f\n", total / count);
}

void sortStudents(Student *students, size_t count) {
    Student *copy;
    Student temp;
    size_t i, j;

    if (count == 0) {
        printf("No records available for sorting.\n");
        return;
    }

    if (count > SIZE_MAX / sizeof(Student)) {
        printf("Sorting allocation size overflow!\n");
        return;
    }

    copy = malloc(count * sizeof(Student));

    if (copy == NULL) {
        printf("Unable to allocate memory for sorting.\n");
        return;
    }

    memcpy(copy, students, count * sizeof(Student));

    for (i = 1; i < count; i++) {
        temp = copy[i];
        j = i;

        while (j > 0 &&
              (copy[j - 1].GPA < temp.GPA ||
              (copy[j - 1].GPA == temp.GPA &&
               copy[j - 1].rollNo > temp.rollNo))) {

            copy[j] = copy[j - 1];
            j--;
        }

        copy[j] = temp;
    }

    printf("\n----- STUDENTS SORTED BY GPA -----\n");
    displayStudents(copy, count);

    free(copy);
}

void freeStudents(Student *students) {
    free(students);
}

int main() {
    Student *students = NULL;
    size_t count = 0, capacity;
    int initialCapacity, choice, status;

    printf("===== STUDENT RECORDS MANAGEMENT SYSTEM =====\n");


    do {
        status = readInteger("Enter initial capacity: ",
                             &initialCapacity);

        if (status == 0)
            goto cleanup;

        if (status == 1 && initialCapacity <= 0)
            printf("Capacity must be a positive integer.\n");

    } while (status != 1 || initialCapacity <= 0);

    capacity = (size_t)initialCapacity;

    students = allocateStudents(capacity);

    if (students == NULL) {
        printf("Unable to allocate initial memory.\n");
        goto cleanup;
    }

    printf("Memory allocated successfully for %zu students.\n",
           capacity);

    while (1) {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Summary\n");
        printf("7. Sort Students by GPA\n");
        printf("8. Exit\n");

        status = readInteger("Enter your choice: ", &choice);

        if (status == 0)
            break;

        if (status != 1)
            continue;

        switch (choice) {
            case 1:
                addStudent(&students, &count, &capacity);
                break;

            case 2:
                displayStudents(students, count);
                break;

            case 3:
                searchStudent(students, count);
                break;

            case 4:
                updateStudent(students, count);
                break;

            case 5:
                deleteStudent(students, &count);
                break;

            case 6:
                displaySummary(students, count);
                break;

            case 7:
                sortStudents(students, count);
                break;

            case 8:
                printf("Exiting program...\n");
                goto cleanup;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

cleanup:
    freeStudents(students);
    printf("Allocated memory released successfully.\n");

    return 0;
}
