#include <iostream>
#include <mpi.h>

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
        // Создать случайное сообщение
        int message;
        if (rank == 0) {
            message = rand() % 100;
            std::cout << "Центральный процесс отправляет сообщение: " << message << std::endl;
        }

        // Распространить сообщение от процесса с рангом 0 всем остальным процессам
        MPI_Bcast(&message, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Получить сообщение от центрального процесса
        std::cout << "процесс " << rank << " получает сообщение от центрального процесса: " << message << std::endl;

        // Генерировать случайный ответ
        int response = rand() % 100;

        // Отправить ответ обратно в центральный процесс
        if (rank == 0) {
            // Получить ответы от всех остальных процессов
            for (int j = 1; j <= N; j++) {
                MPI_Recv(&response, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout << "Центральный процесс получает ответ от процесса " << j << ": " << response << std::endl;
            }
        } else {
            MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
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
