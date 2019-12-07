#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	if (argc < 3) {
		puts("Ошибка. Необходимо указать номер программы и количество строк для вывода");
		return 1;
	}

    // clear file  
    FILE* f = fopen("output.log", "w");
    fclose(f);

	int* array;
	int shmid;

	shmid = shmget(1, 5 * sizeof(int), 0666 | IPC_CREAT);

	printf("shmid: %d\n", shmid);

	if (shmid == -1) {
		puts("Ошибка. Не удалось создать разделяемый сегмент памяти");
		return 1;
	}

	array = (int*) shmat(shmid, NULL, 0);

	printf("lock: %p\n", array);

	if (array == (int*) (-1)) {
		puts("Ошибка. Не удалось отобразить разделяемую память в адресное пространство процесса");
		return 1;
	}

	int numProgram = atoi(argv[1]);
	int countStr = atoi(argv[2]);

	FILE* fout;

	for (int iterations = 0; iterations < countStr; iterations++) {

		array[numProgram] = 1;

		array[0] = numProgram;

		for (int i = 1; i < 4; i++) {
		     while (i != numProgram && array[i] == 1 && array[0] != numProgram);
        }

        fout = fopen("output.log", "a+");

        fprintf(fout, "PID: %d\n", getpid());
        printf("PID: %d\n", getpid());

        fclose(fout);

		sleep(1);

		array[numProgram] = 0;

	}

	shmdt(array);

	return 0;
}
