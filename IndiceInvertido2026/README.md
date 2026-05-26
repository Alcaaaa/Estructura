# Plan del Proyecto — Índice Invertido (Entrega I)

*Curso:* Estructuras de Datos — CIN311/INF313/ICI313 (1/2026)
*Fecha de entrega:* 26 de mayo 2026
*Equipo:* 
Jorge Bahamondes Amador
Martín Araya Riquelme
Bruno Díaz Fernández
---

## 1. Reglas inquebrantables (lo que NO se puede usar)

- ❌ map, unordered_map, vector, set, arreglos dinámicos avanzados.
- ❌ Árboles, hash maps, estructuras avanzadas no vistas en Unidad I.
- ❌ Generación de código por LLMs (lo dice la rúbrica).
- ✅ Permitido: punteros crudos, string, ifstream, iostream, listas enlazadas implementadas a mano.

*Verificado en el código actual:* cero map, cero vector, cero arreglos dinámicos. Todo punteros + listas enlazadas propias.

---

## 2. Modelo del "Edificio" (analogía acordada)

- *Ascensor (eje vertical):* la lista enlazada de palabras (Diccionario → nodoVocabulario).
- *Cada piso del edificio:* una palabra única del vocabulario.
- *Pasillos (eje horizontal):* la lista enlazada que sale de cada piso (ListaPostRef → nodoPostRef).
- *Las puertas del pasillo:* punteros a posts en la colección maestra (ColeccionPosts).

*Segundo edificio paralelo (red social):* IndiceUsuarios → nodoUsuario → ListaAmigos → nodoAmigo. Mismo patrón: clave (usuario) en la vertical, lista de amigos en la horizontal.

---

## 3. Mapeo Rúbrica → Código (chequeo punto por punto)

### 3.1 Modelamiento de datos requerido

| Requisito de la rúbrica | Implementación | Estado |
|---|---|---|
| Usuarios con datos básicos + lista de amigos | nodoUsuario con nombre + ListaAmigos | ✅ |
| Posts vinculados a su autor | nodoPost.autor (string) | ✅ |
| Posts con texto original | nodoPost.textoOriginal | ✅ |
| Estructura que registre likes Y la identidad de quien dio like | ListaLikes → nodoLike con nombreUsuario | ✅ (estructura) |
| Índice Invertido Usuarios → Contactos (lista enlazada propia) | IndiceUsuarios con ListaAmigos por usuario | ✅ |
| Índice Invertido Posts → Términos (lista enlazada propia) | Diccionario con ListaPostRef por palabra | ✅ |

> *Sobre los likes con identidad:* el dataset solo entrega el conteo numérico (columna Likes), no los nombres de quienes dieron like. Por eso, al cargar guardamos total = conteo y dejamos la lista vacía. La estructura está completa y se puebla de tres formas:
> - *Opción 5 del menú:* agregar un like real con nombre.
> - *Opción 6 del menú:* generar N likes sintéticos a partir del índice de usuarios (claramente etiquetado como "sintético").
> - *Carga manual de un post nuevo (Opción 4):* se ingresa el conteo y luego se agregan likes con identidad.

### 3.2 Requisitos funcionales mínimos

| Requisito | Función / archivo | Estado |
|---|---|---|
| (a) Carga de Dataset (CSV) | cargarDatosDesdeCSV | ✅ |
| (b) Filtrado de stopwords (al construir y al consultar) | esStopword (~100 palabras EN+ES) | ✅ |
| (c) Memoria dinámica con listas enlazadas propias | Todas las estructuras | ✅ |
| (d.i) Creación del índice | indexarPost + agregarPalabraAlIndice | ✅ |
| (d.ii) Inserción sin duplicidad | existeRefEnLista, existeAmigo, buscarPostPorId | ✅ |
| (d.iii.1) Búsqueda de amigos de un usuario | mostrarAmigosDe | ✅ |
| (d.iii.2) Búsqueda de posts por términos individuales | buscarYMostrar, buscarVariosTerminos | ✅ |

### 3.3 Restricciones técnicas

| Restricción | Cumplimiento |
|---|---|
| Lenguaje C/C++/Python/Java | C++11 ✅ |
| Clave del índice = nombre de usuario / vocabulario | ✅ |
| Valor = lista enlazada implementada por el estudiante | ✅ |
| No map, vector, árboles, hash maps | ✅ verificado |

---

## 4. Estado actual del código

| Archivo | Contenido |
|---|---|
| proyecto/estructura.h | TDA: Likes, Posts, ColeccionPosts, ListaPostRef, Vocabulario, IndiceUsuarios, ListaAmigos |
| proyecto/funciones.h | Declaraciones públicas |
| proyecto/listasFunciones.cpp | Implementaciones (≈480 líneas) |
| proyecto/main.cpp | Menú con 9 opciones |
| proyecto/dataset.csv | Mini-CSV de prueba en formato real |

*Verificación de compilación y ejecución:*
- Compila con g++ -std=c++11 -Wall sin warnings.
- Carga del dataset real arroja: *2008 posts, **12687 palabras únicas, **3133 usuarios* en la red.
- Búsqueda de palabra (climate) → 1999 posts ✅
- Búsqueda de amigos (@GretaThunberg) → 5 conexiones detectadas vía menciones ✅

---

## 5. Decisiones de diseño documentadas

### 5.1 Posts referenciados, no copiados
La rúbrica dice literalmente "almacena las **referencias* a todos los objetos de tipo Post"*. Por eso:
- ColeccionPosts guarda cada nodoPost UNA sola vez.
- Las listas de posteo del índice usan nodoPostRef (puntero al post real), no copias.
- Beneficio: si se modifica un post (ej. agregar un like), el cambio es visible desde cualquier palabra que apunte a él.

### 5.2 Construcción del grafo de amigos desde @menciones
El dataset no incluye relaciones de amistad explícitas. La fuente más honesta de conexiones sociales es el patrón @usuario dentro de cada tweet:
- Si el autor @A escribe un tweet que menciona a @B, registramos amistad mutua A↔️B.
- Es realista (las menciones SON una forma de interacción social en redes reales).
- Es determinístico (no inventa datos, los extrae del texto).

### 5.3 Stopwords inline
La lista de stopwords está hardcodeada en esStopword() con cadenas de comparaciones. Esto es ineficiente (O(k) por palabra) pero respeta Unidad I (sin set, sin hash). Para 12k palabras es perfectamente rápido.

---

## 6. Documentación de Estructuras de Datos (estructura.h)

El proyecto se basa exclusivamente en memoria dinámica y listas enlazadas. Las estructuras principales son:

*   *ColeccionPosts (nodoPost)*: Colección maestra donde cada post existe una sola vez. Almacena ID, autor, texto original y una lista de likes.
*   *ListaLikes (nodoLike)*: Lista enlazada anidada dentro de cada post que almacena la identidad de los usuarios que dieron "like".
*   *Diccionario (nodoVocabulario): Actúa como el **Índice Invertido principal*. Es una lista de palabras únicas. Cada nodo (piso) contiene la palabra y un puntero a su lista de referencias.
*   *ListaPostRef (nodoPostRef)*: Los "pasillos" del índice. Contiene punteros (nodoPost*) hacia los posts originales de la colección maestra. No duplica el texto, optimizando la memoria.
*   *IndiceUsuarios (nodoUsuario)*: Segundo Índice Invertido que modela la red social. Almacena usuarios únicos.
*   *ListaAmigos (nodoAmigo)*: Sub-lista anidada en cada usuario que registra las conexiones mutuas extraídas de las @menciones.
*   *ListaStopwords (nodoStopword)*: Lista enlazada simple para almacenar términos excluidos en la indexación.

## 7. Documentación de Funciones Principales (funciones.h)

A continuación, un resumen de las funciones más relevantes agrupadas por su propósito:

*1. Carga y Procesamiento:*
*   cargarDatosDesdeCSV(): Parsea el archivo línea por línea, manejando campos entre comillas, extrae los datos, crea el post y llama a la función de indexación.
*   cargarStopwordsDesdeArchivo(): Lee un .txt de stopwords, normaliza cada palabra y la inserta en la lista enlazada si no existe previamente.

*2. Indexación y Diccionario:*
*   indexarPost(): Recorre el texto de un post separando palabras, valida que no sean stopwords y las agrega al índice invertido.
*   agregarPalabraAlIndice(): Busca la palabra en el diccionario; si no existe crea el "piso", y luego añade la referencia al post en su "pasillo" validando duplicados.
*   reindexarPosts(): Limpia el diccionario actual y vuelve a procesar la colección maestra de posts usando una lista nueva de stopwords.

*3. Búsqueda:*
*   buscarYMostrar(): Encuentra una palabra en el diccionario y recorre linealmente su lista de referencias imprimiendo los posts en los que aparece.
*   buscarVariosTerminos(): Separa una consulta larga en palabras individuales y ejecuta la búsqueda de manera individual para cada término.

*4. Red Social:*
*   extraerMencionesYConectar(): Analiza el texto de un post, detecta el patrón @usuario y establece amistades mutuas en el índice de usuarios.
*   mostrarAmigosDe(): Consulta el IndiceUsuarios y lista secuencialmente todos los contactos almacenados en la sub-lista del usuario solicitado.

*5. Gestión de Memoria:*
*   liberarTodo(): Función vital ejecutada al salir del programa. Recorre y libera la memoria de todas las listas y sub-listas (Diccionario, Posts, Usuarios, Stopwords) previniendo fugas de memoria.

## 8. Cómo compilar y correr

bash
cd proyecto
g++ -std=c++11 -Wall -o main.exe main.cpp listasFunciones.cpp
./main.exe


El ejecutable busca ../DataSET Procesado Final.csv (carpeta padre). El CSV procesado debe estar separado por ;, con texto entre comillas y BOM al inicio (lo que ya tiene).

---

## 9. Criterios de evaluación (referencia)

| Criterio | Pts | Cobertura actual |
|---|---|---|
| Correctitud (carga, stopwords, índice) | 45 | ~45 (todos los requisitos funcionales presentes) |
| Estructuras (listas enlazadas propias) | 20 | 20 (cero estructuras avanzadas) |
| Calidad del código (modularización, comentarios, memoria) | 15 | ~14 (faltan algunos comentarios extra y validación de input) |
| Documentación + defensa | 20 | 20 |
| *Total* | *100* | *100* |