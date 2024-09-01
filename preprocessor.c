#include "preprocessor.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
int P_dot;
int P_sum;
int P_aver;

struct ArrayTable AT[MAX_ARRAYS];
int array_count = 0;
struct ParseTable PT;

// Function for @int (declaration)
void handle_int(const char *name, const char *size1, const char *size2) {
    FILE *out = fopen("expanded.c", "a+");
    
    if (size2 == NULL || size2[0] == '\0' || strcmp(size2, "") == 0) {   
        fprintf(out, "  int %s[%s];\n",name, size1);
    } else {        
        fprintf(out, "  int %s[%s][%s];\n",name, size1, size2);
    }
    fclose(out);
    
    strcpy(AT[array_count].name, name);
    AT[array_count].dim = (size2[0] == '\0') ? 1 : 2;
    strcpy(AT[array_count].size1, size1);
    if (size2[0] != '\0') {
        strcpy(AT[array_count].size2, size2);
    }
    array_count++;
}

// Function for @init
void handle_init(const char *name, const char *value) {
    FILE *out = fopen("expanded.c", "a");

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, name) == 0) {
            if (AT[i].dim == 1) {
                fprintf(out, "  for (int i = 0; i < %s; ++i){\n", AT[i].size1);
                fprintf(out, "      %s[i] = %s;\n", name, value);
                fprintf(out, "  }\n");
            } else if (AT[i].dim == 2) {
                fprintf(out, "  for (int i = 0; i < %s; ++i){\n", AT[i].size1);
                fprintf(out, "      for (int j = 0; j < %s; ++j){\n", AT[i].size2);
                fprintf(out, "      %s[i][j] = %s;\n", name, value);
                fprintf(out, "      }\n");
                fprintf(out, "      printf(\"\\n\");\n");
                fprintf(out, "  }\n");
            }
            break;
        }
    }

    fclose(out);
}
// Function for @print
void handle_print(const char *name) {
    FILE *out = fopen("expanded.c", "a");

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, name) == 0) {
            if (AT[i].dim == 1) {
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      printf(\"%%d \", %s[i]);\n", name);
                fprintf(out, "  }\n");
                fprintf(out, "  printf(\"\\n\");\n");
            } else if (AT[i].dim == 2) {
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      for (int j = 0; j < %s; ++j) {\n", AT[i].size2);
                fprintf(out, "          printf(\"%%d \", %s[i][j]);\n", name);
                fprintf(out, "      }\n");
                fprintf(out, "      printf(\"\\n\");\n");
                fprintf(out, "  }\n");
            }
            break;
        }
    }

    fclose(out);
}
void handle_read(const char* name, const char* filename) {
    FILE* in = fopen(filename, "r"); //read mode
    FILE* out = fopen("expanded.c", "a"); //append mode

    for (int i; i < array_count; ++i) {
        if (strcmp(AT[i].name, name) == 0) {
            if (AT[i].dim == 1) {
                fprintf(out, "FILE* in = fopen(\"%s\", \"r\");\n", filename); //open file for reading
                fprintf(out, "for(int i = 0; i < %s; ++i){\n", AT[i].size1);
                fprintf(out, "fscanf(in, \"%%d\", &%s[i]);\n", name);
                fprintf(out, "}\n");
                fprintf(out, "fclose(in);\n");
            }
            else if (AT[i].dim == 2) {
                fprintf(out, "  FILE* in = fopen(\"%s\", \"r\");\n", filename);
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      for (int j = 0; j < %s; ++j) {\n", AT[i].size2);
                fprintf(out, "          fscanf(in, \"%%d\", &%s[i][j]);\n", name);
                fprintf(out, "      }\n");
                fprintf(out, "  }\n");
                fprintf(out, "  fclose(in);\n");
            }
            break;
        }
    }
    fclose(out);
}

void handle_copy(const char* dest, const char* src) {
    FILE *out = fopen("expanded.c", "a");

    int dest_i = -1, src_i = -1;

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, dest) == 0) {
            dest_i = i;
        }
        if (strcmp(AT[i].name, src) == 0) {
            src_i = i;
        }
    }

    if (dest_i == -1 || src_i == -1) {
        fprintf(out, "/* Error: One of the arrays does not exist */\n");
        fclose(out);
        return;
    }

    if (AT[dest_i].dim == 1) {
        fprintf(out, "for (int i = 0; i < %s; ++i) {\n", AT[dest_i].size1);
        fprintf(out, "  int %s[i] = %s[i];\n", dest, src);
        fprintf(out, "}\n");
    } else if (AT[dest_i].dim == 2) {
        fprintf(out, "for (int i = 0; i < %s; ++i) {\n", AT[dest_i].size1);
        fprintf(out, "  for (int j = 0; j < %s; ++j) {\n", AT[dest_i].size2);
        fprintf(out, "    int %s[i][j] = %s[i][j];\n", dest, src);
        fprintf(out, "  }\n");
        fprintf(out, "}\n");
    }

    fclose(out);
}

// Reduction of operation
void handle_sum(const char *name) {
    FILE *out = fopen("expanded.c", "a");

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, name) == 0) {
            if (AT[i].dim == 1) {
                fprintf(out, "  int P_sum1 = 0;\n");
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      P_sum1 += %s[i];\n", name);
                fprintf(out, "  }\n");
            } else if (AT[i].dim == 2) {
                fprintf(out, "  int P_sum1 = 0;\n");
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      for (int j = 0; j < %s; ++j) {\n", AT[i].size2);
                fprintf(out, "          P_sum1 += %s[i][j];\n", name);
                fprintf(out, "      }\n");
                fprintf(out, "  }\n");
            }
            break;
        }
    }

    fclose(out);
}

void handle_aver(const char *name) {
    FILE *out = fopen("expanded.c", "a");

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, name) == 0) {
            if (AT[i].dim == 1) {
                fprintf(out, "  int P_sum = 0;\n");
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      P_sum += %s[i];\n", name);
                fprintf(out, "  }\n");
                fprintf(out, "  int P_aver = P_sum / %s;\n", AT[i].size1);
            } else if (AT[i].dim == 2) {
                fprintf(out, "  int P_sum = 0;\n");
                fprintf(out, "  for (int i = 0; i < %s; ++i) {\n", AT[i].size1);
                fprintf(out, "      for (int j = 0; j < %s; ++j) {\n", AT[i].size2);
                fprintf(out, "          P_sum += %s[i][j];\n", name);
                fprintf(out, "      }\n");
                fprintf(out, "  }\n");
                fprintf(out, "  int P_aver = P_sum / (%s * %s);\n", AT[i].size1, AT[i].size2);
            }
            break;
        }
    }

    fclose(out);
}

// Matrix and Vector calculations
void handle_dotp
(const char* C, const char* A, const char* B) {
    FILE* out = fopen("expanded.c", "a");

    int A_i = -1; 
    int B_i = -1;

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, A) == 0) {
            A_i = i;
        }
    }
    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, B) == 0) {
            B_i = i;
        }
    }
    fprintf(out, "int P_dot = 0;\n");
    fprintf(out, "for(int i = 0; i < %s; ++i){\n", AT[A_i].size1);
    fprintf(out, "  P_dot += %s[i] * %s[i];\n", A, B);
    fprintf(out, "}\n");

    fclose(out);

}


void handle_add(const char* C, const char* A, const char* B) {//assigns the result to an array
    FILE* out = fopen("expanded.c", "a");

    int A_i = -1, B_i = -1, C_i = -1;

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, A) == 0) {
            A_i = i;
        }
        if (strcmp(AT[i].name, B) == 0) {
            B_i = i;
        }
        if (strcmp(AT[i].name, C) == 0) {
            C_i = i;
        }
    }

    if (AT[A_i].dim == 1) {
        fprintf(out, "  for(int i = 0; i < %s; ++i){\n", AT[A_i].size1);
        fprintf(out, "    %s[i] = %s[i] + %s[i];\n", C, A, B);
        fprintf(out, "  }\n");
    }
    else if (AT[A_i].dim == 2) {
        fprintf(out, "  for(int i = 0; i < %s; ++i){\n", AT[A_i].size1);
        fprintf(out, "    for(int j = 0; j < %s; ++j){\n", AT[A_i].size2);
        fprintf(out, "        %s[i][j] = %s[i][j] + %s[i][j];\n", C, A, B);
        fprintf(out, "    }\n");
        fprintf(out, "  }\n");
    }
    fclose(out);
}

void handle_mmult(const char* C, const char* A, const char* B) {//only for 2D arrays
    FILE* out = fopen("expanded.c", "a");

    int A_i = -1, B_i = -1, C_i = -1;

    for (int i = 0; i < array_count; ++i) {
        if (strcmp(AT[i].name, A) == 0) {
            A_i = i;
        }
        if (strcmp(AT[i].name, B) == 0) {
            B_i = i;
        }
        if (strcmp(AT[i].name, C) == 0) {
            C_i = i;
        }
    }

    fprintf(out, "  for(int i = 0; i < %s; ++i){\n", AT[A_i].size1);
    fprintf(out, "    for(int j = 0; j < %s; ++i){\n ", AT[A_i].size2);
    fprintf(out, "        %s[i][j] = 0;\n ", C);
    fprintf(out, "        for(int n = 0; n < %s; ++n){\n ", AT[A_i].size2);
    fprintf(out, "            %s[i][j] += %s[i][k] * %s[k][j];\n  ", C, A, B);
    fprintf(out, "        }\n");
    fprintf(out, "    }\n");
    fprintf(out, "  }\n");

    fclose(out);

}

// Parsing the directives
void parse_line(const char *line) {
    while (*line != '\0' && isspace(*line)) {
        line++;
    }
    if (*line == '@') {
        char oper[MAX_OP_LEN], lhs[MAX_NAME_LEN], rhs1[MAX_NAME_LEN];
        char rhs2[MAX_NAME_LEN] = "";
        
        sscanf(line, "@%s", oper);
    
        strcpy(PT.oper, oper);
        
        if (strcmp(oper, "int") == 0) {
            //@int X(10) or @int X(10)(20)
            int n = sscanf(line, "@int %[^ (](%[^)])(%[^)])", lhs, rhs1, rhs2);//lhs=x rhs1 = size(for dim1)  rhs2 = size(for dim2)
            strcpy(PT.lhs,lhs);
            strcpy(PT.rhs1,rhs1);
            if (n == 4)
            {
                strcpy(PT.rhs2, rhs2);
            }       
            handle_int(lhs, rhs1, rhs2);

        } else if (strcmp(oper, "init") == 0) {
            //@init X = 0
            sscanf(line, "@init %s = %s", lhs, rhs1);
            handle_init(lhs,rhs1);

        } else if (strcmp(oper, "print") == 0) {
            //@print X 
            sscanf(line, "@print %s", lhs);
            handle_print(lhs);

        } 
        else if (strcmp(oper, "read") == 0) {
            //@read B < f1
            sscanf(line, "@read %s < %s",lhs,rhs1);
            handle_read(lhs,rhs1);
        } else if (strcmp(oper, "copy") == 0) {
            //@copy C = A
            sscanf(line, "@copy %s = %s", lhs, rhs1);
            handle_copy(lhs ,rhs1);
        }

        //Reducation operation
         else if (strcmp(oper, "sum") == 0) {
            //@sum a
            sscanf(line, "@sum %s ", lhs);
            handle_sum(lhs);
        } else if (strcmp(oper, "aver") == 0) {
            //@aver a
            sscanf(line, "@aver %s ", lhs);
            handle_aver(lhs);
        }

        //Matrix and Vector calculations
         else if (strcmp(oper, "dotp") == 0) {
            //@dotp C = A . B
            sscanf(line, "@dotp %s = %s . %s", lhs, rhs1, rhs2);
            handle_dotp(lhs ,rhs1,rhs2);
        }else if (strcmp(oper, "add") == 0) {
            //@add C = A + B 
            sscanf(line, "@add %s = %s + %s", lhs, rhs1, rhs2);
            handle_add(lhs ,rhs1 ,rhs2);
        }else if (strcmp(oper, "mmult") == 0) {
            //@mmult C = A * B
            sscanf(line, "@mmult %s = %s * %s", lhs, rhs1, rhs2);
            handle_mmult(lhs ,rhs1 ,rhs2);
        }


    } else {
        FILE *out = fopen("expanded.c", "a");
        fprintf(out, "%s", line);
        fclose(out);
    }
}
