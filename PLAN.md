# Plan del Proyecto — Índice Invertido (Entrega I)

**Curso:** Estructuras de Datos — CIN311/INF313/ICI313 (1/2026)
**Fecha de entrega:** 12 - 13 de mayo 2026
**Equipo:** (rellenar con los integrantes)

---

## 1. Reglas inquebrantables (lo que NO se puede usar)

- ❌ `map`, `unordered_map`, `vector`, `set`, arreglos dinámicos avanzados.
- ❌ Árboles, hash maps, estructuras avanzadas no vistas en Unidad I.
- ❌ Generación de código por LLMs (lo dice la rúbrica).
- ✅ Permitido: punteros crudos, `string`, `ifstream`, `iostream`, listas enlazadas implementadas a mano.

**Verificado en el código actual:** cero `map`, cero `vector`, cero arreglos dinámicos. Todo punteros + listas enlazadas propias.

---

## 2. Modelo del "Edificio" (analogía acordada)

- **Ascensor (eje vertical):** la lista enlazada de palabras (`Diccionario` → `nodoVocabulario`).
- **Cada piso del edificio:** una palabra única del vocabulario.
- **Pasillos (eje horizontal):** la lista enlazada que sale de cada piso (`ListaPostRef` → `nodoPostRef`).
- **Las puertas del pasillo:** punteros a posts en la colección maestra (`ColeccionPosts`).

**Segundo edificio paralelo (red social):** `IndiceUsuarios` → `nodoUsuario` → `ListaAmigos` → `nodoAmigo`. Mismo patrón: clave (usuario) en la vertical, lista de amigos en la horizontal.

---

## 3. Mapeo Rúbrica → Código (chequeo punto por punto)

### 3.1 Modelamiento de datos requerido

| Requisito de la rúbrica | Implementación | Estado |
|---|---|---|
| Usuarios con datos básicos + lista de amigos | `nodoUsuario` con `nombre` + `ListaAmigos` | ✅ |
| Posts vinculados a su autor | `nodoPost.autor` (string) | ✅ |
| Posts con texto original | `nodoPost.textoOriginal` | ✅ |
| Estructura que registre likes Y la identidad de quien dio like | `ListaLikes` → `nodoLike` con `nombreUsuario` | ✅ (estructura) |
| Índice Invertido Usuarios → Contactos (lista enlazada propia) | `IndiceUsuarios` con `ListaAmigos` por usuario | ✅ |
| Índice Invertido Posts → Términos (lista enlazada propia) | `Diccionario` con `ListaPostRef` por palabra | ✅ |

> **Sobre los likes con identidad:** el dataset solo entrega el conteo numérico (columna `Likes`), no los nombres de quienes dieron like. Por eso, al cargar guardamos `total = conteo` y dejamos la lista vacía. La estructura está completa y se puebla de tres formas:
> - **Opción 5 del menú:** agregar un like real con nombre.
> - **Opción 6 del menú:** generar N likes sintéticos a partir del índice de usuarios (claramente etiquetado como "sintético").
> - **Carga manual de un post nuevo (Opción 4):** se ingresa el conteo y luego se agregan likes con identidad.

### 3.2 Requisitos funcionales mínimos

| Requisito | Función / archivo | Estado |
|---|---|---|
| (a) Carga de Dataset (CSV) | `cargarDatosDesdeCSV` | ✅ |
| (b) Filtrado de stopwords (al construir y al consultar) | `esStopword` (~100 palabras EN+ES) | ✅ |
| (c) Memoria dinámica con listas enlazadas propias | Todas las estructuras | ✅ |
| (d.i) Creación del índice | `indexarPost` + `agregarPalabraAlIndice` | ✅ |
| (d.ii) Inserción sin duplicidad | `existeRefEnLista`, `existeAmigo`, `buscarPostPorId` | ✅ |
| (d.iii.1) Búsqueda de amigos de un usuario | `mostrarAmigosDe` | ✅ |
| (d.iii.2) Búsqueda de posts por términos individuales | `buscarYMostrar`, `buscarVariosTerminos` | ✅ |

### 3.3 Restricciones técnicas

| Restricción | Cumplimiento |
|---|---|
| Lenguaje C/C++/Python/Java | C++11 ✅ |
| Clave del índice = nombre de usuario / vocabulario | ✅ |
| Valor = lista enlazada implementada por el estudiante | ✅ |
| No `map`, `vector`, árboles, hash maps | ✅ verificado |

---

## 4. Estado actual del código

| Archivo | Contenido |
|---|---|
| `proyecto/estructura.h` | TDA: Likes, Posts, ColeccionPosts, ListaPostRef, Vocabulario, IndiceUsuarios, ListaAmigos |
| `proyecto/funciones.h` | Declaraciones públicas |
| `proyecto/listasFunciones.cpp` | Implementaciones (≈480 líneas) |
| `proyecto/main.cpp` | Menú con 9 opciones |
| `proyecto/dataset.csv` | Mini-CSV de prueba en formato real |

**Verificación de compilación y ejecución:**
- Compila con `g++ -std=c++11 -Wall` sin warnings.
- Carga del dataset real arroja: **2008 posts**, **12687 palabras únicas**, **3133 usuarios** en la red.
- Búsqueda de palabra (`climate`) → 1999 posts ✅
- Búsqueda de amigos (`@GretaThunberg`) → 5 conexiones detectadas vía menciones ✅

---

## 5. Decisiones de diseño documentadas

### 5.1 Posts referenciados, no copiados
La rúbrica dice literalmente *"almacena las **referencias** a todos los objetos de tipo Post"*. Por eso:
- `ColeccionPosts` guarda cada `nodoPost` UNA sola vez.
- Las listas de posteo del índice usan `nodoPostRef` (puntero al post real), no copias.
- Beneficio: si se modifica un post (ej. agregar un like), el cambio es visible desde cualquier palabra que apunte a él.

### 5.2 Construcción del grafo de amigos desde @menciones
El dataset no incluye relaciones de amistad explícitas. La fuente más honesta de conexiones sociales es el patrón `@usuario` dentro de cada tweet:
- Si el autor `@A` escribe un tweet que menciona a `@B`, registramos amistad mutua A↔B.
- Es realista (las menciones SON una forma de interacción social en redes reales).
- Es determinístico (no inventa datos, los extrae del texto).

### 5.3 Stopwords inline
La lista de stopwords está hardcodeada en `esStopword()` con cadenas de comparaciones. Esto es ineficiente (`O(k)` por palabra) pero respeta Unidad I (sin `set`, sin hash). Para 12k palabras es perfectamente rápido.

---

## 6. Pendiente para Entrega I

### 6.1 Documentación obligatoria
- [ ] **README.md** técnico con:
  - Instrucciones de compilación
  - Descripción de cada estructura de `estructura.h`
  - Descripción de cada función pública
  - Decisiones de diseño (sección 5 de este doc puede ir ahí)
- [ ] Diagrama de listas enlazadas (a mano, draw.io, o tikz). Mostrar:
  - Diccionario → Vocabulario → ListaPostRef → ColeccionPosts
  - IndiceUsuarios → ListaAmigos
  - Posts ↔ Likes

### 6.2 Presentación (10 min máx)
- [ ] **Slide 1 (1 min):** integrantes + dataset elegido (Climate Change Twitter Dataset).
- [ ] **Slides 2-3 (2 min):** diagrama del Edificio (estructuras + vinculación Usuarios/Posts/Índice).
- [ ] **Slide 4 (2 min):** explicación del algoritmo de carga + parser CSV + filtrado de stopwords.
- [ ] **Video sin audio (4 min):** grabación de pantalla mostrando partes del código y la ejecución. **Se relata en vivo** durante la defensa.
- [ ] **Slide cierre (1 min):** gestión de memoria dinámica (`liberarTodo`) + conclusiones.

### 6.3 Trámites
- [ ] **Registrar dataset en la planilla compartida** (Climate Change Twitter Dataset).
- [ ] Confirmar día de defensa según los integrantes (12 o 13 mayo).

### 6.4 Mejoras opcionales (suman puntos en "calidad")
- [ ] Búsqueda AND (intersección): posts que contengan **todas** las palabras consultadas.
- [ ] Lista de stopwords ampliada a ~200 palabras.
- [ ] Validación más estricta de input en el menú (qué pasa si el usuario ingresa letras donde se espera número).
- [ ] Mostrar texto truncado del post en los resultados de búsqueda.

---

## 7. Cómo compilar y correr

```bash
cd proyecto
g++ -std=c++11 -Wall -o main.exe main.cpp listasFunciones.cpp
./main.exe
```

El ejecutable busca `../DataSET Procesado Final.csv` (carpeta padre). El CSV procesado debe estar separado por `;`, con texto entre comillas y BOM al inicio (lo que ya tiene).

---

## 8. Ruta crítica

| # | Tarea | Tiempo estimado |
|---|---|---|
| 1 | Probar todas las opciones del menú con casos límite | 1 h |
| 2 | Escribir README.md | 2 h |
| 3 | Hacer diagrama de estructuras | 1 h |
| 4 | Armar slides | 2 h |
| 5 | Grabar video sin audio (un solo tiro) | 1 h |
| 6 | Ensayar defensa cronometrando 10 min | 1 h |
| 7 | Registrar dataset en planilla | 15 min |

**Total estimado pendiente: ~8 horas.**

---

## 9. Criterios de evaluación (referencia)

| Criterio | Pts | Cobertura actual |
|---|---|---|
| Correctitud (carga, stopwords, índice) | 45 | ~45 (todos los requisitos funcionales presentes) |
| Estructuras (listas enlazadas propias) | 20 | 20 (cero estructuras avanzadas) |
| Calidad del código (modularización, comentarios, memoria) | 15 | ~14 (faltan algunos comentarios extra y validación de input) |
| Documentación + defensa | 20 | 0 (pendiente README, slides, video) |
| **Total** | **100** | **~79 + lo que sume la entrega de docs** |
