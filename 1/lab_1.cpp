#include <iostream>
#include <mpi.h>

void star(int N, int M) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < N+1) {
        if (rank == 0) {
            printf("Ошибка: количество процессов должно быть не меньше N+1\n");
        }
        MPI_Finalize();
        exit(1);
    }

    int central_rank = 0;
    int message = 0;

    for (int i = 0; i < M; i++) {
        // Центральный процесс отправляет сообщение всем остальным процессам
        if (rank == central_rank) {
            for (int j = 1; j <= N; j++) {
                MPI_Send(&message, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
                printf("Процесс %d отправил сообщение процессу %d\n", rank, j);
            }
        }
            // Процессы-крайние принимают сообщения от центрального процесса
        else if (rank <= N) {
            MPI_Recv(&message, 1, MPI_INT, central_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Процесс %d принял сообщение от центрального процесса\n", rank);
        }

        // Дожидаемся, пока все процессы завершат свою работу
        MPI_Barrier(MPI_COMM_WORLD);
    }

    printf("Процесс %d завершил свою работу\n", rank);
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

    star(M, N);

    return 0;
}