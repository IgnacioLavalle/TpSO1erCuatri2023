import subprocess as sp
import numpy as np
import random

# configuracion de plots
from matplotlib import pyplot as plt
from matplotlib import rc, rcParams, axes
from matplotlib import axes as mplibAxes
import pandas as pd


def tprun(threads_lectura, threads_maximo, cantArchivos):
    iteraciones = 10
    ejecutable = "../build/ContarPalabras"
    comando = [ejecutable, str(threads_lectura), str(threads_maximo)]
    for i in range(0, cantArchivos):
        comando.append("../data/test-"+ str((i%3+1)))
    print(comando)
    promedio_archivos = []
    promedio_max = []
    for i in range(1,iteraciones):
        res = sp.check_output(comando, encoding="utf8")
        [tiempo_archivos, tiempo_max, _, _] = res.split()
        promedio_archivos.append(float(tiempo_archivos))
        promedio_max.append(float(tiempo_max))
    return [np.ma.median(promedio_archivos), np.ma.median(promedio_max)]

def scatterPlot(ejeY, ejeX, color, label):
    print("Plotting...")
    plt.figure(figsize=(12,12), dpi= 80)
    
    plt.scatter(ejeX, ejeY, s=30, alpha=1, color=color)

    
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.title(label, fontdict={'size':20})
    plt.xlabel('Cantidad de threads', fontsize=20)

    plt.ylabel("Tiempo (s)", fontsize=20)
    plt.grid(linestyle='--')
 
    plt.legend()
    plt.savefig("../Graficos/grafico_"+label+".png")
    plt.show()


def experimento_variando_threads():
    tiempos_archivos = []
    tiempos_max = []
    totalThreads = 6
    threadsLectura = 60
    for threads in range(1,threadsLectura):
        print("Corriendo con ", threads, " threads")
        [tiempo_archivos, tiempo_max] = tprun(threads, totalThreads, 30)
        tiempos_archivos.append(float(tiempo_archivos))
        tiempos_max.append(float(tiempo_max))
    df = pd.DataFrame(data={
        'CantThreads': range(1,threadsLectura),
        'Archivos': tiempos_archivos,
        'Maximo': tiempos_max
    })
    scatterPlot(df.Archivos, df.CantThreads, "red", "Tiempo de carga de archivos en paralelo - Archivos pequeños")

experimento_variando_threads()