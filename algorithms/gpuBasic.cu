#include "../gameLogic/GameInstance.h"
#include "../tools/Timer.h"
#include <cuda_runtime.h>

__global__ void gameKernel(bool* gameArea, bool* nextIteration, int columns, int rows) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    //sprawdzenie, czy wątek nie wyszedł za plansze
    if (x >= columns || y >= rows) return;

    int neighbors = 0;
    for (int dy=-1; dy<=1; dy++) {
        for (int dx=-1; dx<=1; dx++) {
            if(dx==0 && dy==0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if(nx>=0 && nx<columns && ny>=0 && ny<rows) //sprawdzenie, czy sąsiad mieści się na planszy
                neighbors += gameArea[ny*columns + nx];
        }
    }

    nextIteration[y*columns + x] = (neighbors == 3 || (neighbors == 2 && gameArea[y*columns + x]));
}

void gpuBasic(GameInstance& game) {
    int columns = game.getColumns();
    int rows = game.getRows();

    // Alokacja pamięci GPU
    bool* d_gameArea;
    bool* d_nextIteration;
    size_t size = columns * rows * sizeof(bool);
    cudaMalloc(&d_gameArea, size);
    cudaMalloc(&d_nextIteration, size);

    // kopiowanie danych z 2D na 1D
    bool* flat = new bool[columns*rows];
    for(int y=0;y<rows;y++)
        for(int x=0;x<columns;x++)
            flat[y*columns + x] = game.getGameArea()[y][x];

    // kopiowanie planszy z CPU na GPU
    cudaMemcpy(d_gameArea, flat, size, cudaMemcpyHostToDevice);

    dim3 blockSize(16,16);
    dim3 gridSize((columns+15)/16, (rows+15)/16);
    gameKernel<<<gridSize, blockSize>>>(d_gameArea, d_nextIteration, columns, rows);
    cudaDeviceSynchronize(); //czekanie aż wszystkie wątki zakończą działanie kernela

    // Zapis wyników z powrotem do GameInstance
    cudaMemcpy(flat, d_nextIteration, size, cudaMemcpyDeviceToHost);
    for(int y=0;y<rows;y++)
        for(int x=0;x<columns;x++)
            game.getGameArea()[y][x] = flat[y*columns + x];

    delete[] flat;
    cudaFree(d_gameArea);
    cudaFree(d_nextIteration);
}

double gpuBasic(GameInstance& game, int iterations) {
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
    for(int y=0; y<rows; y++)
        for(int x=0; x<columns; x++)
            flat[y*columns + x] = game.getGameArea()[y][x];

    cudaMemcpy(d_gameArea, flat, size, cudaMemcpyHostToDevice);

    dim3 blockSize(16,16);
    dim3 gridSize((columns+15)/16, (rows+15)/16);

    Timer timer;
    timer.Start();

    //uruchomienie kernela dla kazdej iteracji
    for(int i=0; i<iterations; i++) {
        gameKernel<<<gridSize, blockSize>>>(d_gameArea, d_nextIteration, columns, rows);
        cudaDeviceSynchronize();

        // Zamiana wskaźników zamiast kopiowania w każdej iteracji
        std::swap(d_gameArea, d_nextIteration);
    }

    cudaMemcpy(flat, d_gameArea, size, cudaMemcpyDeviceToHost);

    // Zapis wyników z powrotem do GameInstance
    for(int y=0; y<rows; y++)
        for(int x=0; x<columns; x++)
            game.getGameArea()[y][x] = flat[y*columns + x];

    const double output = timer.Stop();

    delete[] flat;
    cudaFree(d_gameArea);
    cudaFree(d_nextIteration);

    return output;
}
