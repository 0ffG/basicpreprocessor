#include <stdio.h>
#include <stdlib.h>
#include "preprocessor.h"

void process_file(const char *filename) {
    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    char line[256];
    while (fgets(line, sizeof(line), input)) {
        parse_line(line);
    }
    
    fclose(input);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    process_file(argv[1]);
    return 0;
}
