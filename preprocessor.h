#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define MAX_ARRAYS 20
#define MAX_NAME_LEN 10
#define MAX_OP_LEN 10

// Array Table Structure
struct ArrayTable {
    char name[MAX_NAME_LEN];
    int dim; // 1 for 1D, 2 for 2D
    char size1[MAX_NAME_LEN]; // size of dimension 1
    char size2[MAX_NAME_LEN]; // size of dimension 2, if applicable
};

// Parse Table Structure
struct ParseTable {
    char oper[MAX_OP_LEN]; // Operation like @read, @copy, etc.
    char lhs[MAX_NAME_LEN]; // Array on the left-hand side
    char rhs1[MAX_NAME_LEN]; // First array on the right-hand side
    char rhs2[MAX_NAME_LEN]; // Second array on the right-hand side, if applicable
};

// Preprocessor variables
extern int P_dot;
extern int P_sum;
extern int P_aver;

// Array Table and Parse Table
extern struct ArrayTable AT[MAX_ARRAYS];
extern int array_count;
extern struct ParseTable PT;

// Function prototypes
void handle_int(const char *name, const char *size1, const char *size2);
void handle_init(const char *name, const char *value);
void handle_copy(const char* dest, const char* src);
void handle_print(const char *name);
void handle_read(const char* name, const char* filename);
void parse_line(const char *line);
void handle_sum(const char *name);
void handle_aver(const char *name);
void handle_dotp(const char *result, const char *array1, const char *array2);
void handle_add(const char *result, const char *array1, const char *array2);
void handle_mmult(const char *result, const char *array1, const char *array2);

#endif // PREPROCESSOR_H
