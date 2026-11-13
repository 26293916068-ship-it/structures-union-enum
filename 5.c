
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_RECTANGLES 1000
#define TOLERANCE 0.000001

typedef struct Rectangle {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

Rectangle *allocateMemory(size_t n) {
    return malloc(n * sizeof(Rectangle));
}

int resizeArray(Rectangle **r, size_t *capacity) {
    size_t newCapacity;
    Rectangle *temp;

    if (*capacity >= MAX_RECTANGLES)
        return 0;

    newCapacity = (*capacity > MAX_RECTANGLES / 2)
                  ? MAX_RECTANGLES : *capacity * 2;

    if (newCapacity > SIZE_MAX / sizeof(Rectangle))
        return 0;

    temp = realloc(*r, newCapacity * sizeof(Rectangle));

    if (temp == NULL)
        return 0;

    *r = temp;
    *capacity = newCapacity;

    return 1;
}

int inputInteger(const char *message, int *value) {
    char str[100], *end;
    long num;

    printf("%s", message);

    if (fgets(str, sizeof(str), stdin) == NULL)
        return 0;

    if (strchr(str, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Input too long!\n");
        return -1;
    }

    errno = 0;
    num = strtol(str, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (end == str || *end != '\0' ||
        errno == ERANGE || num < INT_MIN || num > INT_MAX) {
        printf("Invalid integer!\n");
        return -1;
    }

    *value = (int)num;
    return 1;
}

int inputDimension(const char *message, float *value) {
    char str[100], *end;
    double num;

    printf("%s", message);

    if (fgets(str, sizeof(str), stdin) == NULL)
        return 0;

    if (strchr(str, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Input too long!\n");
        return -1;
    }

    errno = 0;
    num = strtod(str, &end);

    while (isspace((unsigned char)*end))
        end++;

    if (end == str || *end != '\0' ||
        errno == ERANGE || !isfinite(num) ||
        num <= 0 || num > FLT_MAX) {
        printf("Invalid dimension! Enter a positive finite value.\n");
        return -1;
    }

    *value = (float)num;

    if (!isfinite(*value) || *value <= 0) {
        printf("Dimension is outside the supported range!\n");
        return -1;
    }

    return 1;
}

int calculateArea(Rectangle *r) {
    double a, p;

    a = (double)r->width * r->height;
    p = 2.0 * ((double)r->width + r->height);

    if (!isfinite(a) || !isfinite(p) ||
        a > FLT_MAX || p > FLT_MAX)
        return 0;

    r->area = (float)a;
    r->perimeter = (float)p;

    return 1;
}

int checkSquare(Rectangle r) {
    double diff = fabs((double)r.width - r.height);
    double scale = fmax(1.0, fmax(r.width, r.height));

    return diff <= TOLERANCE * scale;
}

int searchIndex(Rectangle *r, size_t count, int id) {
    size_t i;

    for (i = 0; i < count; i++) {
        if (r[i].id == id)
            return (int)i;
    }

    return -1;
}

void addRectangle(Rectangle **r, size_t *count,
                  size_t *capacity) {
    Rectangle temp;

    if (inputInteger("Enter rectangle ID: ", &temp.id) != 1)
        return;

    if (temp.id <= 0 || searchIndex(*r, *count, temp.id) != -1) {
        printf("Invalid or duplicate ID!\n");
        return;
    }

    if (inputDimension("Enter width: ", &temp.width) != 1)
        return;

    if (inputDimension("Enter height: ", &temp.height) != 1)
        return;

    if (!calculateArea(&temp)) {
        printf("Area or perimeter exceeds supported range!\n");
        return;
    }

    if (*count == *capacity) {
        if (!resizeArray(r, capacity)) {
            printf("Memory expansion failed!\n");
            return;
        }
    }

    (*r)[*count] = temp;
    (*count)++;

    printf("Rectangle added successfully!\n");
}

void displayRectangles(Rectangle *r, size_t count) {
    size_t i;

    if (count == 0) {
        printf("No rectangles available!\n");
        return;
    }

    printf("\nID\tWidth\tHeight\tArea\tPerimeter\tType\n");

    for (i = 0; i < count; i++) {
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\t\t%s\n",
               r[i].id, r[i].width, r[i].height,
               r[i].area, r[i].perimeter,
               checkSquare(r[i]) ? "Square" : "Non-square");
    }
}

void searchRectangle(Rectangle *r, size_t count) {
    int id, index;

    if (count == 0) {
        printf("Collection is empty!\n");
        return;
    }

    if (inputInteger("Enter ID to search: ", &id) != 1)
        return;

    index = searchIndex(r, count, id);

    if (index == -1) {
        printf("Rectangle not found!\n");
        return;
    }

    printf("ID: %d\n", r[index].id);
    printf("Width: %.2f\n", r[index].width);
    printf("Height: %.2f\n", r[index].height);
    printf("Area: %.2f\n", r[index].area);
    printf("Perimeter: %.2f\n", r[index].perimeter);
    printf("Type: %s\n",
           checkSquare(r[index]) ? "Square" : "Non-square");
}

void updateRectangle(Rectangle *r, size_t count) {
    int id, index;
    Rectangle temp;

    if (count == 0) {
        printf("Collection is empty!\n");
        return;
    }

    if (inputInteger("Enter ID to update: ", &id) != 1)
        return;

    index = searchIndex(r, count, id);

    if (index == -1) {
        printf("Rectangle not found!\n");
        return;
    }

    temp = r[index];

    if (inputDimension("Enter new width: ", &temp.width) != 1)
        return;

    if (inputDimension("Enter new height: ", &temp.height) != 1)
        return;

    if (!calculateArea(&temp)) {
        printf("Invalid calculated values!\n");
        return;
    }

    r[index] = temp;

    printf("Rectangle updated successfully!\n");
}

void deleteRectangle(Rectangle *r, size_t *count) {
    int id, index;
    size_t i;

    if (*count == 0) {
        printf("Collection is empty!\n");
        return;
    }

    if (inputInteger("Enter ID to delete: ", &id) != 1)
        return;

    index = searchIndex(r, *count, id);

    if (index == -1) {
        printf("Rectangle not found!\n");
        return;
    }

    for (i = (size_t)index; i < *count - 1; i++)
        r[i] = r[i + 1];

    (*count)--;

    printf("Rectangle deleted successfully!\n");
}

void displaySummary(Rectangle *r, size_t count) {
    size_t i, largest = 0, smallest = 0;
    double total = 0;

    if (count == 0) {
        printf("No records available for analysis!\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += r[i].area;

        if (r[i].area > r[largest].area)
            largest = i;

        if (r[i].area < r[smallest].area)
            smallest = i;
    }

    printf("\n----- GEOMETRY SUMMARY -----\n");
    printf("Largest Area: ID %d (%.2f)\n",
           r[largest].id, r[largest].area);

    printf("Smallest Area: ID %d (%.2f)\n",
           r[smallest].id, r[smallest].area);

    printf("Total Area: %.2f\n", total);
    printf("Average Area: %.2f\n", total / count);
}

void sortRectangles(Rectangle *r, size_t count) {
    Rectangle *copy, temp;
    size_t i, j;

    if (count == 0) {
        printf("No records to sort!\n");
        return;
    }

    if (count > SIZE_MAX / sizeof(Rectangle)) {
        printf("Sorting size overflow!\n");
        return;
    }

    copy = malloc(count * sizeof(Rectangle));

    if (copy == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    memcpy(copy, r, count * sizeof(Rectangle));

    for (i = 1; i < count; i++) {
        temp = copy[i];
        j = i;

        while (j > 0 &&
              (copy[j - 1].area > temp.area ||
              (copy[j - 1].area == temp.area &&
               copy[j - 1].id > temp.id))) {
            copy[j] = copy[j - 1];
            j--;
        }

        copy[j] = temp;
    }

    printf("\n----- SORTED RECTANGLES -----\n");
    displayRectangles(copy, count);

    free(copy);
}

void filterRectangles(Rectangle *r, size_t count) {
    float limit;
    size_t i;
    int found = 0;

    if (count == 0) {
        printf("Collection is empty!\n");
        return;
    }

    if (inputDimension("Enter area threshold: ", &limit) != 1)
        return;

    printf("\nRectangles with area greater than %.2f:\n", limit);

    for (i = 0; i < count; i++) {
        if (r[i].area > limit) {
            printf("ID: %d | Area: %.2f\n",
                   r[i].id, r[i].area);
            found = 1;
        }
    }

    if (!found)
        printf("No matching rectangles found!\n");
}

void freeRectangles(Rectangle *r) {
    free(r);
}

int main() {
    Rectangle *r = NULL;
    size_t count = 0, capacity;
    int n, choice, status;

    printf("===== RECTANGLE MANAGEMENT SYSTEM =====\n");

    do {
        status = inputInteger(
            "Enter initial number of rectangles (1-1000): ", &n);

        if (status == 0)
            goto cleanup;

        if (status == 1 && (n < 1 || n > MAX_RECTANGLES))
            printf("Invalid capacity!\n");

    } while (status != 1 || n < 1 || n > MAX_RECTANGLES);

    capacity = (size_t)n;

    r = allocateMemory(capacity);

    if (r == NULL) {
        printf("Memory allocation failed!\n");
        goto cleanup;
    }

    printf("Memory allocated successfully!\n");

    while (1) {
        printf("\n========== MENU ==========\n");
        printf("1. Add Rectangle\n");
        printf("2. Display Rectangles\n");
        printf("3. Search Rectangle\n");
        printf("4. Update Rectangle\n");
        printf("5. Delete Rectangle\n");
        printf("6. Geometry Summary\n");
        printf("7. Sort Rectangles\n");
        printf("8. Filter by Area\n");
        printf("9. Exit\n");

        status = inputInteger("Enter your choice: ", &choice);

        if (status == 0)
            break;

        if (status != 1)
            continue;

        switch (choice) {
            case 1:
                addRectangle(&r, &count, &capacity);
                break;

            case 2:
                displayRectangles(r, count);
                break;

            case 3:
                searchRectangle(r, count);
                break;

            case 4:
                updateRectangle(r, count);
                break;

            case 5:
                deleteRectangle(r, &count);
                break;

            case 6:
                displaySummary(r, count);
                break;

            case 7:
                sortRectangles(r, count);
                break;

            case 8:
                filterRectangles(r, count);
                break;

            case 9:
                goto cleanup;

            default:
                printf("Invalid choice!\n");
        }
    }

cleanup:
    freeRectangles(r);
    printf("Memory released successfully.\n");

    return 0;
}
