#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
// #include <pthread.h>
#include <thread>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementarEnLista(ListaAtomica<hashMapPair>* lista, std::string clave) {
    for (auto element = lista->crearIt(); element.haySiguiente(); element.avanzar()) {
        if(element.siguiente().first == clave){ // Si la clave ya existe la incrementamos
            element.siguiente().second++;
            return;
        }
    }
    // Si no esta la clave se agrega
    lista->insertar(std::make_pair(clave, 1));
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int bucketIndex = hashIndex(clave);
    mutexes[bucketIndex].lock();
    incrementarEnLista(tabla[bucketIndex], clave);
    mutexes[bucketIndex].unlock();
}




std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> claves;
    for (auto bucket: tabla) {
         for(auto element = bucket->crearIt(); element.haySiguiente(); element.avanzar()){
             claves.push_back(element.siguiente().first);
         }
    }
    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    ListaAtomica<hashMapPair>* bucket = tabla[hashIndex(clave)]; // buscamos la lista asociada a esa clave
    for(auto element = bucket->crearIt(); element.haySiguiente(); element.avanzar()){
        if(element.siguiente().first == clave){
            return element.siguiente().second;
        }
    }
    return 0; // si no esta devolvemos 0
}

hashMapPair HashMapConcurrente::maximo() {

    for(auto& mtx: mutexes){
        mtx.lock();
    }

    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    for(auto& mtx: mutexes){
        mtx.unlock();
    }

    return *max;
}

hashMapPair HashMapConcurrente:: maximoLista(int indice) {

    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (auto it = tabla[indice]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().second > max->second) {
            max->first = it.siguiente().first;
            max->second = it.siguiente().second;
        }
    }

    

    return *max;
}


void HashMapConcurrente::buscarMaximoThread(std::atomic<int>* actual, hashMapPair *maximoActual, std::mutex *mtx_maximo){

     while(*actual < HashMapConcurrente::cantLetras){
        unsigned int indice =(*actual).fetch_add(1);
        if(indice >= HashMapConcurrente::cantLetras) break; // por las dudas que dos o mas incrementen en 24

        hashMapPair max = HashMapConcurrente::maximoLista(indice); // busca maximo de lista con indice actual

        mtx_maximo->lock();// actualiza maximo
        if(max.second > maximoActual->second){
            maximoActual->first = max.first;
            maximoActual->second = max.second;
        }
        mtx_maximo->unlock();
     }

}


hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {

    for(auto& mut: mutexes){
        mut.lock();
    }

    std::atomic<int> actual(0);
    hashMapPair *maximoActual = new hashMapPair();
    maximoActual->second = 0;
    std::mutex mtx_maximo;
    std::vector<std::thread> threads(cant_threads);

    for (auto& t: threads) {
        t = std::thread(&HashMapConcurrente::buscarMaximoThread, this , &actual, maximoActual, &mtx_maximo);
    }

    for (auto& t : threads) {
        t.join();
    }

    for(auto& mtx: mutexes){
        mtx.unlock();
    }

    return *maximoActual;
}


#endif
