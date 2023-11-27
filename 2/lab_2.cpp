#include <iostream>
#include <mpi.h>

/**
 * @param N - Кол-во процессов
 * @param M - Кол-во итераций
 */
void star(int N, int M) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < N + 1) {
        if (rank == 0) printf("Ошибка: количество процессов должно быть не меньше N+1\n");
        MPI_Finalize();
        exit(1);
    }

    for (int i = 0; i < M; i++) {
        // Создать случайное сообщение только на процессе с рангом 0
        int message;
        if (rank == 0) {
            message = rand() % 100;
            std::cout << "Центральный процесс отправляет сообщение: " << message << std::endl;
        }

        // Распределить сообщение от процесса с рангом 0 на все остальные процессы
        MPI_Scatter(&message, 1, MPI_INT, &message, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Получить сообщение от центрального процесса
        std::cout << "Процесс " << rank << " получает сообщение от центрального процесса: " << message << std::endl;

        // Генерировать случайный ответ
        int response = rand() % 100;

        // Собрать ответы от всех процессов на центральный процесс
        MPI_Gather(&response, 1, MPI_INT, &response, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Вывести ответы на центральном процессе
        if (rank == 0) {
            for (int j = 1; j <= N; j++) {
                std::cout << "Центральный процесс получает ответ от процесса " << j << ": " << response << std::endl;
            }
        }
    }

    MPI_Finalize();
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    // Количество процессов должно быть задано через командную строку
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " M N" << std::endl;
        MPI_Finalize();
        return 1;
    }
    //кол-во итераций
    int M = std::stoi(argv[1]);
    //кол-во процессов
    int N = std::stoi(argv[2]);

    star(N, M);

    return 0;
}
