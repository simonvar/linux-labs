#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct thread_inputs {
    char* input_line;
    char* output_filename;    
};

void clean_file(const char* filename); 

void write_line_to_file_async(pthread_t* thread, char* filename, char* line);

void* write_line_to_file(void *args);

char* read_line_from_opened_file(FILE* file);

int main(int argc, char* argv[]) {
    char* source_filename = argv[1];
    char* first_out_filename = argv[2];
    char* second_out_filename = argv[3]; 
    int status;

    clean_file(first_out_filename);
    clean_file(second_out_filename);

    FILE* source_file = fopen(source_filename, "r");
    char* line;
    while ((line = read_line_from_opened_file(source_file)) != NULL) {
        pthread_t* thread1 = malloc(sizeof(pthread_t));
        write_line_to_file_async(thread1, first_out_filename, line);
        char* line2 = read_line_from_opened_file(source_file);
        if(line2 == NULL) {
            pthread_join(*thread1, (void**)&status);
            break;
        }
        pthread_t* thread2 = malloc(sizeof(pthread_t));
        write_line_to_file_async(thread2, second_out_filename, line2);
        pthread_join(*thread1, (void**)&status);
        pthread_join(*thread2, (void**)&status);
        free(thread1);
        free(thread2);
        free(line);
        free(line2);
    }
    return 0;
}

char* read_line_from_opened_file(FILE* file) {
    int buffer_size = 255;
    char* result = malloc(buffer_size);    
    if(fgets(result, buffer_size, file) == NULL) {
	return NULL;    
    }
    return result;
}

void write_line_to_file_async(pthread_t* thread, char* filename, char* line) {
    struct thread_inputs* in = malloc(sizeof(struct thread_inputs));
    in->input_line = line;
    in->output_filename = filename;    
    pthread_create(thread, NULL, write_line_to_file, in);
}

void clean_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    fclose(file);
}

void* write_line_to_file(void *args) {
    struct thread_inputs* in = (struct thread_inputs*)args;
    printf("%s: input_line: %s\n", in->output_filename, in->input_line);
    FILE* file = fopen(in->output_filename, "a");
    fprintf(file, "%s", in->input_line);
    fclose(file);
    free(args);
    return 0;
}

