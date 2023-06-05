#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>
#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);

    std::vector<std::string> claves();
    
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    
    hashMapPair maximoParalelo(unsigned int cantThreads);

    void buscarMaximoThread(std::atomic<int>* actual, hashMapPair *maximoActual, std::mutex *mtx_maximo);

    hashMapPair maximoLista(int indice) ;

 private:

    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

    std::mutex mutexes[HashMapConcurrente::cantLetras];

    static unsigned int hashIndex(std::string clave);

    void incrementarEnLista(ListaAtomica<hashMapPair> *lista, std::string clave);
};

#endif  /* HMC_HPP */