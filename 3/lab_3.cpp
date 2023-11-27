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
    int* recv_buffer = new int[N]; // Буфер для приема сообщений

    for (int i = 0; i < M; i++) {
        if (rank == central_rank) {
            // Генерируем сообщение для отправки
            int* send_buffer = new int[N];
            for (int j = 0; j < N; j++) {
                send_buffer[j] = message;
            }
            // Отправляем сообщение всем остальным процессам
            MPI_Scatter(send_buffer, 1, MPI_INT, MPI_IN_PLACE, 1, MPI_INT, central_rank, MPI_COMM_WORLD);
            printf("Процесс %d отправил сообщение \n", rank);

            delete[] send_buffer;
        }
        else if (rank <= N) {
            // Процессы-крайние принимают сообщения от центрального процесса
            MPI_Scatter(nullptr, 1, MPI_INT, &recv_buffer[rank - 1], 1, MPI_INT, central_rank, MPI_COMM_WORLD);
            printf("Процесс %d принял сообщение от центрального процесса\n", rank);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    delete[] recv_buffer;

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

    star(N, M);

    return 0;
}