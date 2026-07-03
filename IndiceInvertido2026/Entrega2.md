# Documentación del Proyecto — Red Social y Grafo (Entrega II)

*Curso:* Estructuras de Datos — CIN311/INF313/ICI313 (1/2026)  
*Equipo:*  
Jorge Bahamondes Amador  
Martín Araya Riquelme  
Bruno Díaz Fernández  

---

## 1. Puntos Claves de la Implementación del Grafo

El objetivo de esta fase es modelar la red social como un **Grafo No Dirigido $G = (V, E)$** a partir de las menciones (`@usuario`) detectadas en los posts cargados en la Entrega I.

*   **Vértices ($V$)**: Corresponden a los usuarios de la red social. Cada vértice almacena una referencia directa al objeto usuario correspondiente (`nodoUsuario*`).
*   **Aristas ($E$)**: Representan relaciones de amistad no dirigidas y mutuas. Si el usuario $A$ es amigo de $B$, entonces existe una arista no dirigida entre ambos.
*   **Restricciones de Integridad**:
    *   **Sin bucles**: Un usuario no puede tener una arista hacia sí mismo.
    *   **Sin aristas duplicadas**: Se previene explícitamente tener más de una arista entre los mismos dos vértices.
    *   **Simetría**: Al ser no dirigido, por cada relación registrada, se añaden las conexiones de ida y de vuelta de manera simétrica.

---

## 2. Estructuras de Datos Utilizadas (estructura.h)

El grafo se construyó sobre listas de adyacencia utilizando listas enlazadas dinámicas propias:

```cpp
struct nodoVertice; // Declaración adelantada para autoreferencia

// Estructura para cada arista del grafo (eje horizontal)
struct nodoArista {
    nodoVertice* destino; // Puntero al vértice destino
    nodoArista* sig;      // Enlace al siguiente nodo de la lista de adyacencia
};

// Estructura contenedora para la lista de adyacencia
struct ListaAristas {
    nodoArista* primero;
    nodoArista* ultimo;
    int total;
};

// Estructura para cada vértice de la red (eje vertical)
struct nodoVertice {
    nodoUsuario* usuario;     // Referencia al usuario maestro original de la Entrega I
    ListaAristas* adyacentes; // Lista de adyacencia con punteros a sus amigos
    nodoVertice* sig;         // Enlace al siguiente vértice del grafo
};

// Estructura maestra del Grafo
struct Grafo {
    nodoVertice* primero;
    nodoVertice* ultimo;
    int totalVertices;
};
```

Además, para retornar el resultado del recorrido BFS, se definieron listas de contactos dinámicas:
*   `ListaContactos`: Lista enlazada simple de punteros a vértices (`nodoContacto*`).
*   `ResultadoBFS`: Estructura que agrupa tres listas independientes: `grado1` (amigos directos), `grado2` (amigos de amigos) y `grado3` (amigos de 2° grado).

---

## 3. Descripción de las Funciones Principales (funciones.h y listasFunciones.cpp)

*   [inicializarGrafo](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L667): Inicializa los punteros del grafo maestro a `nullptr` y su contador a 0.
*   [registrarVertice](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L683): Crea dinámicamente un nodo vértice para un usuario si no existe ya en el grafo.
*   [agregarAristaDirecta](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L714): Añade una arista dirigida desde un vértice origen a un destino, controlando que no se creen bucles ni aristas duplicadas.
*   [construirGrafo](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L732): Popula el grafo leyendo todos los usuarios del `IndiceUsuarios`. Para cada usuario, mapea su lista de amigos textuales al conjunto de vértices de destino e invoca `agregarAristaDirecta` en ambos sentidos para garantizar la no direccionalidad.
*   [validarSimetriaGrafo](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L765): Recorre el grafo completo y verifica que para toda arista $A \to B$ exista una arista $B \to A$. Imprime un reporte del estado de simetría en consola.
*   [liberarGrafo](file:///Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L787): Libera recursivamente de la memoria dinámica todas las aristas y todos los vértices del grafo para prevenir fugas de memoria.
*   [obtenerGradosConexion](file:///E:/SD/Estruct/Estructura/IndiceInvertido2026/proyecto/listasFunciones.cpp#L836) **(Algoritmo BFS)**: Implementa el algoritmo de recorrido por anchura (BFS) con control de niveles.
    *   Utiliza una **estructura de Cola propia (`ColaVertices`)** implementada a mano.
    *   Utiliza un **marcado directo de visitados** (`visitado` y `nivelBfs`) dentro del struct `nodoVertice` para evitar el uso de `std::unordered_set`, garantizando que la exploración sea $O(V + E)$ y completamente libre de librerías.
    *   Mide el tamaño de la cola al inicio de cada nivel (`tamNivel`) para procesar únicamente los nodos de la distancia actual.
    *   Los vecinos no visitados se clasifican en las listas de retorno según el nivel: `nivel = 0` genera contactos de 1° grado, `nivel = 1` contactos de 2° grado y `nivel = 2` contactos de 3° grado.
    *   Al estar marcados como visitados, se garantiza que no existan duplicados inter-niveles y que el usuario raíz sea excluido.

---

## 4. Casos de Prueba Ejecutados

El dataset completo de la red social fue procesado, arrojando **3,133 usuarios (vértices)** en la red y **11,961 palabras únicas**.

### Caso de Prueba 1: Búsqueda Compuesta con Conexiones Densas
*   **Usuario Raíz:** `@GretaThunberg`
*   **Resultados Obtenidos:**
    *   **1° Grado (5 contactos):** `@AmanzWillieONE`, `@CarlHigbie`, `@BirdgirlUK`, `@charabia539`, `@arnestor`.
    *   **2° Grado (17 contactos):** `@Fridays4FutureU`, `@rolmou`, `@UNEP`, `@UNFCCC`, `@wef`, `@SDonziger`, `@POTUS`, `@Chevron`, `@COP26`, ... (y 8 más).
    *   **3° Grado (45 contactos):** `@planttreaty`, `@ssubory`, `@HarmSaeijs`, `@cooperhefner`, `@unfoundation`, ... (y 40 más).

### Caso de Prueba 2: Conexiones Lineales e Indirectas
*   **Usuario Raíz:** `@laurenboebert`
*   **Resultados Obtenidos:**
    *   **1° Grado (4 contactos):** `@UnionizeTesla`, `@Rob_roborob`, `@DiamataDonita`, `@JonnisTwoCents`.
    *   **2° Grado (1 contacto):** `@kleinbluegirl` (Amiga de `@DiamataDonita`).
    *   **3° Grado (0 contactos):** No hay usuarios a distancia de 3 enlaces desde esta rama.

### Caso de Prueba 3: Grafo Invertido (Nodos Puentes)
*   **Usuario Raíz:** `@kleinbluegirl`
*   **Resultados Obtenidos:**
    *   **1° Grado (1 contacto):** `@DiamataDonita`.
    *   **2° Grado (1 contacto):** `@laurenboebert` (Amigo de `@DiamataDonita`).
    *   **3° Grado (3 contactos):** `@UnionizeTesla`, `@Rob_roborob`, `@JonnisTwoCents` (Amigos de `@laurenboebert`).
    *   *Nota:* Se valida perfectamente la simetría y el no retorno del camino (evita duplicados).

---

## 5. Instrucciones de Compilación y Ejecución

Para compilar el proyecto con soporte para la Entrega II:

```bash
cd proyecto
g++ -std=c++11 -Wall -o main.exe main.cpp listasFunciones.cpp
./main.exe
```

Seleccione la **Opción 31** en el menú de consola para probar los grados de conexión ingresando cualquier nombre de usuario de la red social.
