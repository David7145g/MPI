#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Функция для сравнения элементов при сортировке
int cmp(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 100000; // Количество элементов в массиве
    const int blockSize = N / size; // Размер блока для каждого процесса
    const int remainder = N % size; // Остаток от деления

    // Вычисляем начальный и конечный индекс для каждого процесса
    int start = blockSize * rank + std::min(rank, remainder);
    int end = start + blockSize + (rank < remainder ? 1 : 0);

    // Генерируем случайные числа в промежутке от 0 до 100000
    srand(time(nullptr) + rank);
    int* data = new int[N];
    for (int i = 0; i < N; ++i) {
        data[i] = rand() % 100001;
    }

    // Выводим исходный массив на экран
    if (rank == 0) {
        std::cout << "Исходный массив: ";
        for (int i = 0; i < N; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    int* ranks = new int[size];
    for (int i = 0; i < size; ++i) {
        ranks[i] = i;
    }

    // Создаем коммуникатор для группы процессов
    MPI_Group worldGroup, newGroup;
    MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
    MPI_Group_incl(worldGroup, size, ranks,&newGroup);
    MPI_Comm newComm;
    MPI_Comm_create_group(MPI_COMM_WORLD, newGroup, 0, &newComm);

    // Сортируем блоки данных каждого процесса
    qsort(data + start, end - start, sizeof(int), cmp);

    // Собираем отсортированные блоки данных
    int* sortedData = nullptr;
    if (rank == 0) {
        sortedData = new int[N];
    }

    MPI_Gather(data + start, end - start, MPI_INT, sortedData, end - start, MPI_INT, 0, newComm);

    // Выводим отсортированный массив на экран
    if (rank == 0) {
        std::cout << "Отсортированный массив: ";
        for (int i = 0; i < N; ++i) {
            std::cout << sortedData[i] << " ";
        }
        std::cout << std::endl;
    }

    delete[] data;
    delete[] sortedData;

    MPI_Finalize();

    return 0;
}