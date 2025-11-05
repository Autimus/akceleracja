#include "../gameLogic/GameInstance.h"
#include "../tools/Timer.h"
#include <cuda_runtime.h>
#include <algorithm>

__global__ void gameKernelOpt2(const bool* gameArea, bool* nextIteration, int columns, int rows) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    //sprawdzenie, czy wątek nie wyszedł za plansze
    if (x >= columns || y >= rows) return;

    int neighbors = 0;
    // [1] #pragma unroll - rozwinie pętlę, zmniejszając liczbę iteracji i zwiększając wydajność
    #pragma unroll
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < columns && ny >= 0 && ny < rows) //sprawdzenie, czy sąsiad mieści się na planszy
                neighbors += gameArea[ny * columns + nx];
        }
    }

    // [2] lokalna zmienna current zamiast wielokrotnego dostępu do globalnej pamięci
    bool current = gameArea[y * columns + x]; //new
    nextIteration[y * columns + x] = (neighbors == 3 || (neighbors == 2 && current));
}


void gpuOpt2(GameInstance& game) {
    int columns = game.getColumns();
    int rows = game.getRows();


    // Alokacja pamięci GPU
    bool* d_gameArea;
    bool* d_nextIteration;
    size_t size = columns * rows * sizeof(bool);
    cudaMalloc(&d_gameArea, size);
    cudaMalloc(&d_nextIteration, size);

    // kopiowanie danych z 2D na 1D
    bool* flat = new bool[columns * rows];
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
            flat[y * columns + x] = game.getGameArea()[y][x];

    // [3] new cuda Stream
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    // Asynchroniczna kopia danych CPU→GPU
    cudaMemcpyAsync(d_gameArea, flat, size, cudaMemcpyHostToDevice, stream);

    dim3 blockSize(16, 16);
    dim3 gridSize((columns + 15) / 16, (rows + 15) / 16);

    gameKernelOpt2<<<gridSize, blockSize, 0, stream>>>(d_gameArea, d_nextIteration, columns, rows);

    // Asynchroniczna kopia wyników GPU→CPU
    cudaMemcpyAsync(flat, d_nextIteration, size, cudaMemcpyDeviceToHost, stream);

    // Synchronizacja
    cudaStreamSynchronize(stream);

    // Zapis z powrotem do obiektu gry
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
            game.getGameArea()[y][x] = flat[y * columns + x];

    cudaStreamDestroy(stream);
    delete[] flat;
    cudaFree(d_gameArea);
    cudaFree(d_nextIteration);
}


double gpuOpt2(GameInstance& game, int iterations) {
    int columns = game.getColumns();
    int rows = game.getRows();

    // Alokacja pamięci GPU
    bool* d_gameArea;
    bool* d_nextIteration;
    size_t size = columns * rows * sizeof(bool);
    cudaMalloc(&d_gameArea, size);
    cudaMalloc(&d_nextIteration, size);

    // kopiowanie danych z 2D na 1D
    bool* flat = new bool[columns * rows];
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
            flat[y * columns + x] = game.getGameArea()[y][x];

    // new 2 linijki
    cudaStream_t stream;
    cudaStreamCreate(&stream);
    cudaMemcpyAsync(d_gameArea, flat, size, cudaMemcpyHostToDevice, stream);

    dim3 blockSize(16, 16);
    dim3 gridSize((columns + 15) / 16, (rows + 15) / 16);

    Timer timer;
    timer.Start();

    //uruchomienie kernela dla kazdej iteracji
    for (int i = 0; i < iterations; i++) {
        gameKernelOpt2<<<gridSize, blockSize, 0, stream>>>(d_gameArea, d_nextIteration, columns, rows);
        cudaStreamSynchronize(stream); // <- synchronizacja po każdej iteracji

        // [4] brak synchronize - największa różnica w czasie
        // Zamiana wskaźników zamiast kopiowania w każdej iteracji
        std::swap(d_gameArea, d_nextIteration);
    }

    cudaMemcpyAsync(flat, d_gameArea, size, cudaMemcpyDeviceToHost, stream);
    cudaStreamSynchronize(stream);



    // Zapis wyników z powrotem do GameInstance
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
            game.getGameArea()[y][x] = flat[y * columns + x];

    double elapsed = timer.Stop();

    cudaStreamDestroy(stream); // new
    delete[] flat;
    cudaFree(d_gameArea);
    cudaFree(d_nextIteration);

    return elapsed;
}
