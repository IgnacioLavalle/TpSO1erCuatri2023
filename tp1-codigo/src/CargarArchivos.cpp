#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <atomic>
#include <string>
#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


void cargarArchivoThread(
        HashMapConcurrente *hashMap,
        std::atomic<int>* indiceArchivo,
        std::vector<std::string> filePaths
) {

  while (*indiceArchivo < filePaths.size()) {
    int indice = (*indiceArchivo).fetch_add(1);
    if(indice >= filePaths.size()){
      break;
    }
    cargarArchivo(*hashMap, filePaths[indice]);
  }
}
void cargarMultiplesArchivos(
        HashMapConcurrente &hashMap,
        unsigned int cantThreads,
        std::vector<std::string> filePaths
) {
    std::atomic<int> indiceArchivo(0);
    std::vector<std::thread> threads(cantThreads);

    for (auto& t: threads) {
        t = std::thread(&cargarArchivoThread, &hashMap , &indiceArchivo, filePaths);
    }

    for (auto& t : threads) {
        t.join();
    }
}

#endif
