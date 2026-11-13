
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

#define TEXT_SIZE 20
#define RECORD_COUNT 5

typedef union UnionData {
    int i;
    float f;
    char text[TEXT_SIZE];
} UnionData;

typedef struct StructData {
    int i;
    float f;
    char text[TEXT_SIZE];
} StructData;

typedef enum ValueType {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct TaggedRecord {
    UnionData data;
    ValueType type;
} TaggedRecord;

void setInteger(TaggedRecord *record, int value) {
    record->data.i = value;
    record->type = VALUE_INT;
}

void setFloat(TaggedRecord *record, float value) {
    record->data.f = value;
    record->type = VALUE_FLOAT;
}

int setText(TaggedRecord *record, const char *text) {
    size_t len = strlen(text);

    if (len >= TEXT_SIZE) {
        printf("Error: String must contain at most 19 characters.\n");
        return 0;
    }

    strcpy(record->data.text, text);
    record->type = VALUE_TEXT;

    return 1;
}

void displayRecord(const TaggedRecord *record) {
    switch (record->type) {
        case VALUE_INT:
            printf("Integer: %d\n", record->data.i);
            break;

        case VALUE_FLOAT:
            printf("Float: %.2f\n", record->data.f);
            break;

        case VALUE_TEXT:
            printf("Text: %s\n", record->data.text);
            break;

        default:
            printf("Invalid value type!\n");
    }
}

void displayStructureInfo(StructData *s) {
    printf("\n===== STRUCTURE MEMORY DETAILS =====\n");

    printf("Size of structure: %zu bytes\n", sizeof(StructData));
    printf("Alignment of structure: %zu bytes\n",
           _Alignof(StructData));

    printf("\nMember Addresses:\n");
    printf("Integer: %p\n", (void *)&s->i);
    printf("Float: %p\n", (void *)&s->f);
    printf("Text: %p\n", (void *)s->text);

    printf("\nMember Offsets:\n");
    printf("Integer offset: %zu bytes\n",
           offsetof(StructData, i));
    printf("Float offset: %zu bytes\n",
           offsetof(StructData, f));
    printf("Text offset: %zu bytes\n",
           offsetof(StructData, text));
}

void displayUnionInfo(UnionData *u) {
    printf("\n===== UNION MEMORY DETAILS =====\n");

    printf("Size of union: %zu bytes\n", sizeof(UnionData));
    printf("Alignment of union: %zu bytes\n",
           _Alignof(UnionData));

    printf("\nMember Addresses:\n");
    printf("Integer: %p\n", (void *)&u->i);
    printf("Float: %p\n", (void *)&u->f);
    printf("Text: %p\n", (void *)u->text);
}

void compareMemory() {
    printf("\n===== MEMORY COMPARISON =====\n");

    printf("Size of one structure: %zu bytes\n",
           sizeof(StructData));

    printf("Size of one union: %zu bytes\n",
           sizeof(UnionData));

    printf("Memory for 100 structures: %zu bytes\n",
           100 * sizeof(StructData));

    printf("Memory for 100 unions: %zu bytes\n",
           100 * sizeof(UnionData));

    printf("Size of tagged record: %zu bytes\n",
           sizeof(TaggedRecord));

    printf("Alignment of tagged record: %zu bytes\n",
           _Alignof(TaggedRecord));
}

void basicDemonstration() {
    StructData s;
    UnionData u;

    printf("\n===== BASIC STRUCTURE DEMONSTRATION =====\n");

    s.i = 10;
    s.f = 220.5f;
    strcpy(s.text, "Hello Structure");

    printf("Structure Integer: %d\n", s.i);
    printf("Structure Float: %.2f\n", s.f);
    printf("Structure Text: %s\n", s.text);

    printf("\n===== BASIC UNION DEMONSTRATION =====\n");

    u.i = 10;
    printf("Union Integer: %d\n", u.i);

    u.f = 220.5f;
    printf("Union Float: %.2f\n", u.f);

    strcpy(u.text, "Hello Union");
    printf("Union Text: %s\n", u.text);

    displayStructureInfo(&s);
    displayUnionInfo(&u);
}

int main() {
    TaggedRecord records[RECORD_COUNT];
    int i;


    basicDemonstration();

    compareMemory();

    printf("\n===== TAGGED UNION RECORDS =====\n");

    setInteger(&records[0], 10);
    setFloat(&records[1], 220.5f);
    setText(&records[2], "Union Programming");
    setInteger(&records[3], 500);
    setText(&records[4], "Memory Analysis");

    for (i = 0; i < RECORD_COUNT; i++) {
        printf("\nRecord %d:\n", i + 1);
        displayRecord(&records[i]);
    }

    printf("\n===== CHANGING ACTIVE MEMBER =====\n");

    setInteger(&records[0], 100);
    displayRecord(&records[0]);

    setText(&records[0], "Updated Text");
    displayRecord(&records[0]);

    setFloat(&records[0], 45.75f);
    displayRecord(&records[0]);

    printf("\n===== STRING VALIDATION =====\n");

    if (setText(&records[4], "1234567890123456789"))
        printf("19-character string accepted.\n");

    displayRecord(&records[4]);

    setText(&records[4], "12345678901234567890");

    printf("\n===== INVALID TAG TEST =====\n");

    records[0].type = (ValueType)99;
    displayRecord(&records[0]);

    printf("\nProgram completed successfully.\n");

    return 0;
}
