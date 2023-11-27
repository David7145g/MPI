#include <cstdio>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создание группы с одномерной декартовой топологией (кольцо)
    MPI_Comm ring_comm;
    int ring_dims[1] = {size};
    int ring_periods[1] = {1};
    int ring_reorder = 0;
    MPI_Cart_create(MPI_COMM_WORLD, 1, ring_dims, ring_periods, ring_reorder, &ring_comm);

    // Обмен информацией в группе с одномерной декартовой топологией (кольцо)
    int ring_rank, ring_left, ring_right;
    MPI_Comm_rank(ring_comm, &ring_rank);
    MPI_Cart_shift(ring_comm, 0, 1, &ring_left, &ring_right);

    int ring_sendbuf = rank;
    int ring_recvbuf;
    MPI_Sendrecv(&ring_sendbuf, 1, MPI_INT, ring_right, 0, &ring_recvbuf, 1, MPI_INT, ring_left, 0, ring_comm, MPI_STATUS_IGNORE);

    printf("Ранг кольца %d: отправка %d на ранг %d\n", rank, ring_sendbuf, ring_right);
    printf("Ранг кольца %d: получен %d от ранга %d\n", rank, ring_recvbuf, ring_left);

    // Создание группы с топологией граф (звезда)
    MPI_Comm graph_comm;
    int graph_dims[1] = {size};
    int graph_periods[1] = {1};
    int graph_reorder = 0;
    int graph_sources[1] = {0};
    int graph_destinations[size-1];
    int graph_sourceweights[size-1]; // Предполагая равные веса на данный момент
    int graph_destweights[1]; // Предполагая равные веса на данный момент
    for (int i = 0; i < size - 1; i++) {
        graph_destinations[i] = i + 1;
        graph_sourceweights[i] = 1;
    }
    graph_destweights[0] = size - 1;

    MPI_Graph_create(MPI_COMM_WORLD, 1, graph_dims, graph_periods, graph_reorder, &graph_comm);
    MPI_Dist_graph_create(MPI_COMM_WORLD, 1, graph_sources, graph_sourceweights, graph_destinations, graph_destweights, MPI_INFO_NULL, 1, &graph_comm);

    // Обмен информацией в группе с топологией граф (звезда)
    int graph_rank, graph_indegree, graph_outdegree;
    int graph_sources_count;
    int graph_sources_recv;

    MPI_Comm_rank(graph_comm, &graph_rank);
    MPI_Dist_graph_neighbors_count(graph_comm, &graph_indegree, &graph_outdegree, &graph_sources_count);

    printf("Ранг графа %d: отправка %d соседям\n", graph_rank, graph_rank);
    for (int i = 0; i < graph_sources_count; i++) {
        MPI_Sendrecv(&graph_rank, 1, MPI_INT, graph_sources[i], 0, &graph_sources_recv, 1, MPI_INT, graph_sources[i], 0, graph_comm, MPI_STATUS_IGNORE);
        printf("Ранг графа %d: получено %d от соседей\n", graph_rank, graph_sources_recv);
    }

    MPI_Finalize();

    return 0;
}