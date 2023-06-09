import subprocess as sp
import numpy as np
import random

# configuracion de plots
from matplotlib import pyplot as plt
from matplotlib import rc, rcParams, axes
from matplotlib import axes as mplibAxes
import pandas as pd
import ast
from collections import Counter
import string


def runSourceCode(threads_lectura, threads_maximo, cantArchivos,archivos):
    ejecutable = "../build/Sandbox"
    comando = [ejecutable, str(threads_lectura), str(threads_maximo)]

    for i in range(0, cantArchivos):
         comando.append("../data/"+ archivos[i])
    print(comando)
    res = sp.check_output(comando, encoding="utf8")
    print("Resultado de c++:")
    print(res)
    tiemposMaximos = ast.literal_eval(res)
        
    return tiemposMaximos

def scatterPlot(ejeY, ejeX, color, label,totalThreads):
    print("Plotting...")
    plt.figure(figsize=(12,12), dpi= 80)
    
    plt.scatter(ejeX, ejeY, s=30, alpha=1, color=color)

    #plt.yticks(ejeY)
    plt.xticks(np.arange(0, totalThreads+1, 5.0),fontsize=15)
    plt.yticks(fontsize=15)
    plt.title(label, fontdict={'size':20})
    plt.xlabel('Cantidad de threads', fontsize=20)
    plt.ylabel("Tiempo (s)", fontsize=20)
    plt.grid(linestyle='--')
    plt.legend()
    plt.savefig("../Graficos/grafico_"+label+".png")
    plt.show()



def generarArchivo():
    palabras = []
    for i in range(0, 10): #1000
        palabras = palabras + list(string.ascii_lowercase)[3:10]
    
    archivo = "wordset-experimento2-chico.txt"
    with open("../data/"+archivo, 'w') as f:
        for p in palabras:
            f.write("%s\n" % p)

def generarExperimento2():
    palabras = []
    archivo = "wordset-experimento2.txt"
    with open("../data/"+archivo) as file:
        while (line := file.readline().rstrip()):
           palabras.append(line)

    data =  Counter(s[0] for s in  palabras)
    names = list(data.keys())
    values = list(data.values())
    plt.bar(range(len(data)), values, tick_label=names)
    plt.show()


def comparar():
   
    archivos = ["wordset-experimento2-grande.txt"]
    tiempos_max = runSourceCode(5,4,len(archivos), archivos)
    tiempoDatasetGrande = tiempos_max[2];  #tiempo con 3 threads
    
    archivos = ["wordset-experimento2-chico.txt"]
    tiempos_max = runSourceCode(5,4,len(archivos), archivos)
    tiempoDatasetChico= tiempos_max[0];  #tiempo con 1 threads

    values = [tiempoDatasetGrande,tiempoDatasetChico ]

    plt.bar(range(len(values)), values, tick_label=["Dataset Grande 3 threads","Dataset chico 1 thread"])
    plt.show()

def experimentar():

    totalThreads = 40 # Elegir el maximo de cantidad de threads
    archivos = ["igualCantidadDePalabrasXLetra.txt"] # elegir con que archivo se busca experimentar
    
    tiempos_max = runSourceCode(5,totalThreads,len(archivos), archivos) # devuelve una lista con los tiempos por cantidad de threads
    df = pd.DataFrame(data={
        'CantThreads': range(1,totalThreads+1), 
        'Maximo': tiempos_max
    })
    scatterPlot(df.Maximo, df.CantThreads, "red", "Tiempo de ejecucion de maximoParalelo",totalThreads)


experimentar()

#Auxiliares para plotear y generar archivos

comparar()
generarArchivo()  
generarExperimento2() 