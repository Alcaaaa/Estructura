# Proyecto: Índice Invertido, Red Social y Tabla Hash

## Descripción general

Este proyecto implementa un sistema de búsqueda y análisis sobre un conjunto de posts, integrando cuatro componentes principales:
- un índice invertido de términos,
- un índice de usuarios y relaciones de amistad,
- un grafo no dirigido para explorar grados de conexión,
- y una tabla hash con función djb2 para almacenar frecuencias de palabras.

El programa permite cargar un dataset desde un archivo CSV, filtrar stopwords, indexar contenido, buscar posts por palabras, consultar amigos de usuarios, explorar conexiones por grados y visualizar métricas de la tabla hash.

## Requisitos del sistema

Para compilar y ejecutar este proyecto se necesita:
- un compilador de C++ compatible con C++11 o superior,
- un entorno de consola estándar (`g++` o `clang++`),
- y los archivos de datos incluidos en la raíz del proyecto:
  - `DataSET Procesado Final.csv`
  - `EN-Stopwords.txt`

No se requieren librerías externas ni paquetes adicionales.

## Estructura del proyecto

- `proyecto/main.cpp`: punto de entrada y menú principal.
- `proyecto/estructura.h`: definiciones de estructuras de datos.
- `proyecto/funciones.h`: declaraciones de funciones públicas.
- `proyecto/listasFunciones.cpp`: implementación de la lógica principal del sistema.
- `DataSET Procesado Final.csv`: dataset de entrada.
- `EN-Stopwords.txt`: lista de stopwords utilizada en el procesamiento del texto.

## Compilación

Desde la carpeta `IndiceInvertido2026/proyecto`, ejecute el siguiente comando:

```bash
g++ -std=c++11 -Wall -O2 -o main.exe main.cpp listasFunciones.cpp
```

Este comando genera el archivo ejecutable `main.exe`.

## Ejecución

Una vez compilado, ejecute:

```bash
./main.exe
```

El programa busca automáticamente los archivos de datos relativos a la carpeta padre del proyecto:
- `../DataSET Procesado Final.csv`
- `../EN-Stopwords.txt`

Por ello, es recomendable ejecutar el programa desde la carpeta `proyecto` manteniendo la estructura original del repositorio.

## Funcionalidades principales

- carga de posts desde un archivo CSV,
- filtrado de stopwords,
- indexación invertida de términos,
- búsqueda de posts por palabra o conjunto de términos,
- construcción de una red social a partir de menciones `@usuario`,
- exploración de conexiones por grados mediante BFS,
- métricas de la tabla hash y top-N de términos frecuentes.

## Casos de prueba

### 1. Carga inicial del dataset
- Ejecutar el programa sin modificar la configuración inicial.
- Verificar que se carguen los posts y se construya la red social.
- Resultado esperado: visualización de mensajes de carga del dataset y construcción del grafo.

### 2. Búsqueda de una palabra
- Seleccionar la opción `1`.
- Ingresar un término como `climate`.
- Resultado esperado: listado de posts que contienen la palabra.

### 3. Consulta de amigos de un usuario
- Seleccionar la opción `3`.
- Ingresar un usuario como `@GretaThunberg`.
- Resultado esperado: listado de amigos detectados mediante las menciones en los posts.

### 4. BFS por grados de conexión
- Seleccionar la opción `31`.
- Ingresar un usuario existente.
- Resultado esperado: visualización de contactos de 1°, 2° y 3° grado.

### 5. Inserción manual de un post
- Seleccionar la opción `4`.
- Ingresar un ID nuevo, autor, texto y cantidad inicial de likes.
- Resultado esperado: el nuevo post es agregado y queda disponible para búsquedas posteriores.

### 6. Métricas de la tabla hash
- Seleccionar la opción `81`.
- Resultado esperado: impresión de métricas como tamaño de tabla, factor de carga y colisiones.

### 7. Top-N de términos más frecuentes
- Seleccionar la opción `82`.
- Ingresar un valor como `10`.
- Resultado esperado: listado de los términos con mayor frecuencia.

## Verificación realizada

Se verificó que el proyecto compila y ejecuta correctamente con el siguiente comando:

```bash
g++ -std=c++11 -Wall -O2 -o main.exe main.cpp listasFunciones.cpp
```

Además, la ejecución del programa mostró correctamente el menú principal y permitió finalizar la sesión sin errores.
