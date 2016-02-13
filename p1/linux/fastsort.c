#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#define     MAX_LINE_LENGTH 128

typedef struct line {
    char *word;
    char *line;
} line_t;

typedef struct darray {
    int end;
    int max;
    line_t **values;
} dynarray;

void cleanup(int);

dynarray *array;
FILE *fp;

#define die(M, ...) fprintf(stderr, M, ##__VA_ARGS__); exit(1);
#define error(M, ...) fprintf(stderr, M, ##__VA_ARGS__); cleanup(1);
#define check(A, M, ...) if(!(A)) { error(M, ##__VA_ARGS__) }
#define check_mem(A) check(A, "malloc failed\n")

void array_push(line_t *line) {
    array->values[array->end] = line;
    array->end++;

    if ((array->end) >= (array->max)) {
        array->max *= 2;
        void *values = realloc(array->values, array->max * sizeof(line_t *));
        check_mem(values);
        array->values = values;
    }
}

int cmp(const void * a, const void * b) {
    return strncmp((*(line_t**)a)->word, (*(line_t**)b)->word, 128); 
}

void cleanup(int ret) {
    if (array) {
        if (array->values) {
            int i;
            for (i = 0; i < array->end; i++) {
                free(array->values[i]->word);
                free(array->values[i]->line);
                free(array->values[i]);
            }
            free(array->values);
        }
        free(array);
    }

    exit(ret);
}

int main(int argc, char *argv[])
{
    int column = 1;
    char *filepath;

    if (argc == 2)
        filepath = argv[1];
    else if (argc == 3) {
        if (argv[1][0] != '-') {
            die("Error: Bad command line parameters\n");
        }
        
        char *end;
        column = strtol(&(argv[1][1]), &end, 10);
        if (*end) {
            die("Error: Bad command line parameters\n");
        }

        filepath = argv[2];
    }
    else {
        die("Error: Bad command line parameters\n");
    }
    array = NULL;

    fp = fopen(filepath, "r");
    check(fp, "Error: Cannot open file %s\n", filepath);

    array = malloc(sizeof(dynarray));
    check_mem(array);
    array->max = 500;
    array->end = 0;
    array->values = malloc(500 * sizeof(line_t *));

    char buffer[MAX_LINE_LENGTH+3];
    char *prev, *tok;
    int len, col;
    while (fgets(buffer, MAX_LINE_LENGTH+3, fp) != NULL) {
        len = strnlen(buffer, MAX_LINE_LENGTH+2);
        if (len > MAX_LINE_LENGTH+1) {
            error("Line too long\n");
        }

        line_t *newLine = malloc(sizeof(line_t));
        check_mem(newLine);

        newLine->line = malloc(len + 1);
        check_mem(newLine->line);

        strncpy(newLine->line, buffer, len+1);

        tok = strtok(buffer, " ");
        prev = NULL;
        col = 1;
        while (tok != NULL) {
            if (col == column) break;
            prev = tok;
            tok = strtok(NULL, " ");
            col++;
        }
        if (!tok) tok = prev;

        len = strnlen(tok, MAX_LINE_LENGTH+2);
        if (tok[len-1] == '\n') len--;
        newLine->word = malloc(len+1);
        check_mem(newLine->word);

        strncpy(newLine->word, tok, len+1);
        array_push(newLine);
    }
    fclose(fp);

    qsort(array->values, array->end, sizeof(line_t*), cmp);

    int i;
    for (i = 0; i < array->end; i++)
        printf("%s", (array->values[i])->line);

    cleanup(0);
    return 0;
}
