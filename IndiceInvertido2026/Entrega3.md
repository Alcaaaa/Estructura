# Documentación del Proyecto — Tabla Hash y Frecuencias (Entrega III)

*Curso:* Estructuras de Datos — CIN311/INF313/ICI313 (1/2026)  
*Fecha de entrega:* Tercera Fase  
*Equipo:*  
Jorge Bahamondes Amador  
Martín Araya Riquelme  
Bruno Díaz Fernández  

---

## 1. Puntos Claves de la Implementación de la Tabla Hash

El objetivo de esta fase es contabilizar la frecuencia absoluta de aparición de cada término en el dataset completo (excluyendo stopwords) utilizando una **Tabla Hash con direccionamiento por encadenamiento separado**.

*   **Función Hash**: Se utiliza obligatoriamente la función canónica **`djb2`** (Daniel J. Bernstein), que procesa los caracteres multiplicando recursivamente el acumulador por 33 (desplazamiento de bits `(hash << 5) + hash`) y sumando el valor del carácter actual.
*   **Colisiones**: Resueltas mediante listas enlazadas dinámicas simples (`nodoHash`). Cada celda de la tabla contiene un puntero al inicio de la lista de colisiones de esa celda.
*   **Manejo de Memoria**: Se implementa una función dedicada [liberarTablaHash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L941) para limpiar de la memoria dinámica todos los nodos colisionados y el arreglo de celdas antes de finalizar la ejecución del programa.

---

## 2. Dimensionamiento y Elección del Tamaño $M$

Para optimizar el uso de memoria y minimizar las colisiones, el tamaño $M$ de la tabla hash debe ser el menor número primo que cumpla con un factor de carga inferior o igual al 67%:
$$\frac{N}{M} \leq 0.67 \implies M \geq \frac{N}{0.67}$$

El programa realiza este cálculo de forma dinámica:
1.  Obtiene el tamaño del vocabulario $N = 11,961$ (cantidad de términos únicos sin stopwords en la colección).
2.  Calcula el límite inferior teórico: $M_{min} = \lceil 11,961 / 0.67 \rceil = 17,853$.
3.  Busca mediante algoritmos de primalidad (`esPrimo` / `obtenerSiguientePrimo`) el menor primo mayor o igual a $17,853$, que resulta ser **$M = 17,863$**.

**Métricas Obtenidas en Carga Real:**
*   **Vocabulario ($N$):** 11,961 palabras únicas.
*   **Tamaño de la Tabla ($M$):** 17,863 celdas (primo).
*   **Factor de Carga obtenido ($N/M$):** **`0.669596`** (aproximadamente $66.96\%$), cumpliendo con la restricción de diseño $\leq 0.67$.
*   **Justificación:** Al elegir el menor número primo que satisface el factor de carga límite de $0.67$, se aprovecha al máximo la memoria disponible sin superar el umbral donde las colisiones degradan significativamente el tiempo de búsqueda a $O(N)$ en el direccionamiento encadenado.

---

## 3. Estructuras de Datos Utilizadas (estructura.h)

```cpp
// Nodo para la lista enlazada de colisiones
struct nodoHash {
    string termino;     // Palabra clave normalizada
    int frecuencia;     // Contador de ocurrencias en el dataset
    nodoHash* sig;      // Enlace al siguiente nodo colisionado en la misma celda
};

// Estructura contenedora de la Tabla Hash
struct TablaHash {
    nodoHash** celdas;  // Arreglo dinámico de punteros a nodoHash
    int M;              // Tamaño físico de la tabla (primo)
    int N;              // Cantidad de términos únicos registrados
};
```

---

## 4. Métricas de Colisiones y Rendimiento Obtenidas

Al ejecutar la opción de menú **81** en la consola, se obtienen las siguientes métricas estadísticas de la distribución de la tabla:

*   **Celdas Ocupadas:** 8,667 celdas ocupadas de las 17,863 totales (representa un **`48.52%`** de ocupación física).
*   **Total de Colisiones:** **`3,294 colisiones`** (nodos secundarios en las listas de encadenamiento, calculado como $N - \text{Celdas Ocupadas}$).
*   **Largo Máximo de Colisión:** **5 nodos** en la cadena de colisión más larga.
*   **Largo Promedio (Celdas Ocupadas):** **1.38 nodos** por celda activa.
*   **Largo Promedio (Total de Celdas):** **0.67 nodos** (coincide con el factor de carga teórico $N/M$).

*Análisis:* Un largo máximo de colisiones de 5 sobre un vocabulario de casi 12,000 palabras demuestra una distribución muy uniforme del algoritmo `djb2`, asegurando que las operaciones de inserción y consulta se mantengan muy cercanas a $O(1)$ en promedio.

---

## 5. Caso de Prueba: Consulta Top-N (Menú Opción 82)

Para comprobar el correcto ordenamiento y conteo, realizamos una consulta de los **Top-10 términos más frecuentes** del dataset:

1.  **"climate"** (frecuencia: 3,150)
2.  **"change"** (frecuencia: 2,681)
3.  **"https"** (frecuencia: 353)
4.  **"replying"** (frecuencia: 318)
5.  **"tweet"** (frecuencia: 264)
6.  **"quote"** (frecuencia: 260)
7.  **"people"** (frecuencia: 219)
8.  **"global"** (frecuencia: 165)
9.  **"time"** (frecuencia: 165)
10. **"feb"** (frecuencia: 149)

*Nota:* Las palabras "climate" y "change" lideran con diferencia la frecuencia del dataset, lo cual es coherente con el origen del conjunto de datos centrado en publicaciones sobre cambio climático.

---

## 6. Funciones Principales Implementadas (funciones.h y listasFunciones.cpp)

*   [djb2Hash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L797): Calcula el código hash multiplicando por 33 de forma eficiente mediante desplazamientos de bits.
*   [obtenerSiguientePrimo](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L817): Determina dinámicamente el menor número primo adecuado a partir del factor de carga.
*   [inicializarTablaHash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L827): Reserva el arreglo dinámico de punteros de tamaño $M$ y pone las celdas en `nullptr`.
*   [insertarTerminoHash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L836): Busca la palabra en la lista encadenada correspondiente al índice hash; si existe, incrementa su frecuencia; si no, crea un nodo y lo enlaza.
*   [construirTablaHash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L860): Orquesta la carga de la tabla leyendo y limpiando secuencialmente cada palabra de los posts.
*   [reportarMetricasHash](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L891): Mide y calcula los promedios y máximos de las cadenas de colisión.
*   [consultarTopN](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L954): Recopila los punteros a todos los nodos del hash en un arreglo dinámico temporal, los ordena mediante un algoritmo **QuickSort propio** con criterio descendente de frecuencia (y orden alfabético en caso de empates) y muestra los primeros $N$ elementos.
