#include <stdio.h>
#include <pthread.h>

struct thread_inputs {
    char* input_line;
    char* output_filename;    
};

void clean_file(const char* filename); 

pthread_t write_line_to_file_async(char* filename, char* line);

void* write_line_to_file(void *args);

int main(int argc, char* argv[]) {
    char* source_filename = argv[1];
    char* first_out_filename = argv[2];
    char* second_out_filename = argv[3];
    
    clean_file(first_out_filename);
    clean_file(second_out_filename);
    int status;
    int status1;
    pthread_t thread1 = write_line_to_file_async(first_out_filename, "line\n");
    pthread_t thread2 = write_line_to_file_async(second_out_filename, "line2\n");
    pthread_join(thread1, (void**)&status);
    pthread_join(thread2, (void**)&status1);
    return 0;
}

pthread_t write_line_to_file_async(char* filename, char* line) {
    pthread_t thread;
    struct thread_inputs in = { line, filename };
    printf(" { input_line = %s, output_filename =  %s }\n ", in.input_line, in.output_filename);
    pthread_create(&thread, NULL, write_line_to_file, &in);
    return thread;
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
