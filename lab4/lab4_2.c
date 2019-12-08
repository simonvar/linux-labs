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

int main(int argc, char* argv[]) {
    char* source_filename = argv[1];
    char* first_out_filename = argv[2];
    char* second_out_filename = argv[3];
    
    clean_file(first_out_filename);
    clean_file(second_out_filename);
    int status;
    int status1;
    pthread_t* thread1 = malloc(sizeof(pthread_t));
    pthread_t* thread2 = malloc(sizeof(pthread_t));
    write_line_to_file_async(thread1, first_out_filename, "line\n");
    write_line_to_file_async(thread2, second_out_filename, "line2\n");
    pthread_join(thread1, (void**)&status);
    pthread_join(thread2, (void**)&status1);
    return 0;
}

void write_line_to_file_async(pthread_t* thread, char* filename, char* line) {
    struct thread_inputs* in = malloc(sizeof(struct thread_inputs));
    in->input_line = line;
    in->output_filename = filename;    
    printf(" { input_line = %s, output_filename =  %s }\n ", in->input_line, in->output_filename);
    pthread_create(thread, NULL, write_line_to_file, in);
}

void clean_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    fclose(file);
}

void* write_line_to_file(void *args) {
    struct thread_inputs* in = (struct thread_inputs*)args;
    printf(" { input_line = %s, output_filename =  %s }\n ", in->input_line, in->output_filename);
    return 0;
}
