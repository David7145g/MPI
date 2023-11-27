#include <cstdio>
#include <mpi.h>

#define MAX_POLY_SIZE 100

typedef struct {
    int degree;
    double coefficients[MAX_POLY_SIZE];
} Polynomial;

int main(int argc, char* argv[]) {
    int rank, size;
    Polynomial polyA, polyB, result;
    MPI_Datatype polyType;
    MPI_Status status;

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Определение типа MPI_Datatype для полинома
    int blocklengths[2] = {1, MAX_POLY_SIZE};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint displacements[2] = {offsetof(Polynomial, degree), offsetof(Polynomial, coefficients)};
    MPI_Type_create_struct(2, blocklengths, displacements, types, &polyType);
    MPI_Type_commit(&polyType);

    if (rank == 0) {
        // Инициализация полиномов A и B на процессе с рангом 0
        polyA.degree = 3;
        polyA.coefficients[0] = 2.0;
        polyA.coefficients[1] = 3.0;
        polyA.coefficients[2] = 1.0;
        polyA.coefficients[3] = 4.0;

        polyB.degree = 2;
        polyB.coefficients[0] = 1.0;
        polyB.coefficients[1] = 2.0;
        polyB.coefficients[2] = 3.0;

        // Отправка полиномов A и B другим процессам
        for (int i = 1; i < size; i++) {
            MPI_Send(&polyA, 1, polyType, i, 0, MPI_COMM_WORLD);
            MPI_Send(&polyB, 1, polyType, i, 1, MPI_COMM_WORLD);
        }
    } else {
        // Получение полиномов A и B на остальных процессах
        MPI_Recv(&polyA, 1, polyType, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&polyB, 1, polyType, 0, 1, MPI_COMM_WORLD, &status);
    }

    // Вычисление результатов произведения полиномов
    result.degree = polyA.degree + polyB.degree;
    for (int i = 0; i <= result.degree; i++) {
        result.coefficients[i] = 0.0;
    }

    for (int i = 0; i <= polyA.degree; i++) {
        for (int j = 0; j <= polyB.degree; j++) {
            result.coefficients[i + j] += polyA.coefficients[i] * polyB.coefficients[j];
        }
    }

    if (rank == 0) {
        // Получение результатов от остальных процессов
        Polynomial temp;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&temp, 1, polyType, i, 2, MPI_COMM_WORLD, &status);
            for (int j = 0; j <= temp.degree; j++) {
                result.coefficients[j] += temp.coefficients[j];
            }
        }

        // Вывод результата
        printf("Результат умножения полиномов:\n");
        for (int i = 0; i <= result.degree; i++) {
            printf("%.2f ", result.coefficients[i]);
        }
        printf("\n");
    } else {
        // Отправка результата на процесс с рангом 0
        MPI_Send(&result, 1, polyType, 0, 2, MPI_COMM_WORLD);
    }

    // Освобождение ресурсов MPI
    MPI_Type_free(&polyType);
    MPI_Finalize();

    return 0;
}