#include <iostream>
#include <mpi.h>

/**
Проблема в том, что внутри данного условия if (rank == 0) процесс с рангом 0 отправляет сообщение
 всем остальным процессам и затем ждет ответа от каждого из них. Однако, остальные процессы
 находятся в ожидании приема сообщения от процесса с рангом 0 и не могут отправить свой ответ,
 пока не получат сообщение.
Чтобы исправить этот код, можно использовать функцию MPI_Bcast, которая позволяет передавать
 сообщение от одного процесса всем остальным процессам. Таким образом, нет необходимости использовать
 отдельные MPI_Send и MPI_Recv для каждого процесса. -> lab_2
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
        if (rank == 0) {
            // Создать случайное сообщение
            int message = rand() % 100;
            std::cout << "Центральный процесс отправляет сообщение: " << message << std::endl;

            // Отправить сообщение всем остальным процессам
            for (int j = 1; j <= N; j++) {
                MPI_Send(&message, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            }

            // Получить ответ от всех остальных процессов
            int response;
            for (int j = 1; j <= N; j++) {
                MPI_Recv(&response, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout << "Центральный процесс получает ответ от процесса " << j << ": " << response << std::endl;
            }
        }
        else {
            // Получить сообщение от центрального процесса
            int message;
            MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "процесс " << rank << " получает сообщение от центрального процесса: " << message << std::endl;

            // Генерировать случайный ответ
            int response = rand() % 100;

            // Отправить ответ обратно в центральный процесс
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