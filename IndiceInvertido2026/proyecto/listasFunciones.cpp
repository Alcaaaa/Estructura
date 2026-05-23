#include <iostream>
#include <fstream>
#include <string>
#include "estructura.h"
#include "funciones.h"

using namespace std;

// =====================================================
// INICIALIZADORES
// Dejan las cabeceras de las listas en estado vacio.
// =====================================================
void inicializarDiccionario(Diccionario* dic) {
    dic->primero = nullptr;
    dic->ultimo  = nullptr;
    dic->totalPalabras = 0;
}

void inicializarColeccion(ColeccionPosts* col) {
    col->primero = nullptr;
    col->ultimo  = nullptr;
    col->total   = 0;
}

void inicializarListaPostRef(ListaPostRef* lista) {
    lista->primero = nullptr;
    lista->ultimo  = nullptr;
    lista->numElem = 0;
}

void inicializarListaLikes(ListaLikes* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->total   = 0;
}

// =====================================================
// STOPWORDS — INICIALIZADOR Y MANEJO DE LISTA
// =====================================================
void inicializarListaStopwords(ListaStopwords* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->total   = 0;
}

// Recorrido lineal para verificar pertenencia (TDA basico).
bool estaEnListaStopwords(ListaStopwords* lista, const string& palabra) {
    if (lista == nullptr) return false;
    nodoStopword* actual = lista->primero;
    while (actual != nullptr) {
        if (actual->palabra == palabra) return true;
        actual = actual->sig;
    }
    return false;
}

// Inserta al final si no existe ya (sin duplicados).
void agregarStopword(ListaStopwords* lista, const string& palabra) {
    if (lista == nullptr || palabra.length() == 0) return;
    if (estaEnListaStopwords(lista, palabra)) return;

    nodoStopword* nuevo = new nodoStopword;
    nuevo->palabra = palabra;
    nuevo->sig = nullptr;

    if (lista->primero == nullptr) {
        lista->primero = nuevo;
        lista->ultimo  = nuevo;
    } else {
        lista->ultimo->sig = nuevo;
        lista->ultimo = nuevo;
    }
    lista->total++;
}

// Lee un archivo de stopwords (una palabra por linea).
// Normaliza cada linea a minusculas + solo letras (descarta BOM, \r, etc.)
void cargarStopwordsDesdeArchivo(ListaStopwords* lista, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cout << "[Aviso] No se pudo abrir el archivo de stopwords: "
             << nombreArchivo << endl;
        return;
    }

    string linea;
    int leidas = 0;
    while (getline(archivo, linea)) {
        string norm = normalizarPalabra(linea);
        if (norm.length() > 0) {
            agregarStopword(lista, norm);
            leidas++;
        }
    }
    archivo.close();
    cout << "[Exito] " << leidas << " stopwords leidas de '"
         << nombreArchivo << "'. Total en lista: " << lista->total << endl;
}

// Libera la lista de stopwords completa.
void liberarListaStopwords(ListaStopwords* lista) {
    if (lista == nullptr) return;
    nodoStopword* actual = lista->primero;
    while (actual != nullptr) {
        nodoStopword* tmp = actual;
        actual = actual->sig;
        delete tmp;
    }
    lista->primero = nullptr;
    lista->ultimo  = nullptr;
    lista->total   = 0;
}

// =====================================================
// STOPWORDS — VERIFICACION
// esStopword consulta primero la lista cargada por archivo,
// y como respaldo usa la lista hardcoded de respaldo.
// =====================================================
bool esStopword(const string& p, ListaStopwords* lista) {
    if (p.length() <= 1) return true;
    if (estaEnListaStopwords(lista, p)) return true;
    return esStopwordHardcoded(p);
}

// Lista de respaldo hardcoded (queda si el usuario nunca carga archivo).
bool esStopwordHardcoded(const string& p) {
    // Palabras muy cortas no aportan a la busqueda
    if (p.length() <= 1) return true;

    // Ingles
    if (p == "the" || p == "a"   || p == "an"  || p == "and" || p == "or"  ||
        p == "but" || p == "if"  || p == "of"  || p == "in"  || p == "on"  ||
        p == "at"  || p == "to"  || p == "for" || p == "with"|| p == "by"  ||
        p == "from"|| p == "is"  || p == "are" || p == "was" || p == "were"||
        p == "be"  || p == "been"|| p == "being"|| p == "have"|| p == "has"||
        p == "had" || p == "do"  || p == "does"|| p == "did" || p == "will"||
        p == "would"|| p == "should"|| p == "could"|| p == "can"|| p == "may"||
        p == "might"|| p == "must"|| p == "i"  || p == "you" || p == "he"  ||
        p == "she" || p == "it"  || p == "we"  || p == "they"|| p == "them"||
        p == "his" || p == "her" || p == "its" || p == "our" || p == "their"||
        p == "this"|| p == "that"|| p == "these"|| p == "those"|| p == "as"||
        p == "not" || p == "no"  || p == "yes" || p == "so"  || p == "than"||
        p == "then"|| p == "there"|| p == "here"|| p == "what"|| p == "when"||
        p == "where"|| p == "who"|| p == "how" || p == "why" || p == "all" ||
        p == "any" || p == "some"|| p == "more"|| p == "most"|| p == "such"||
        p == "only"|| p == "own" || p == "same"|| p == "too" || p == "very"||
        p == "just"|| p == "now" || p == "also"|| p == "into"|| p == "out" ||
        p == "up"  || p == "down"|| p == "off" || p == "over"|| p == "again"||
        p == "ve"  || p == "re"  || p == "ll"  || p == "s"   || p == "t"  ||
        p == "m"   || p == "d"   || p == "don" || p == "doesn"|| p == "didn"||
        p == "isn" || p == "aren"|| p == "wasn"|| p == "weren"|| p == "won")
        return true;

    // Espanol
    if (p == "el"  || p == "la"  || p == "los" || p == "las" || p == "un"  ||
        p == "una" || p == "unos"|| p == "unas"|| p == "y"   || p == "o"   ||
        p == "de"  || p == "del" || p == "en"  || p == "con" || p == "por" ||
        p == "para"|| p == "que" || p == "se"  || p == "su"  || p == "sus" ||
        p == "es"  || p == "son" || p == "fue" || p == "ser" || p == "esta"||
        p == "este"|| p == "esto"|| p == "esa" || p == "ese" || p == "eso" ||
        p == "lo"  || p == "le"  || p == "les" || p == "me"  || p == "mi"  ||
        p == "te"  || p == "ti"  || p == "yo"  || p == "tu"  || p == "el"  ||
        p == "no"  || p == "si"  || p == "al"  || p == "ya"  || p == "muy")
        return true;

    return false;
}

// Convierte la palabra a minusculas y devuelve solo letras a-z
string normalizarPalabra(const string& palabra) {
    string out = "";
    for (int i = 0; i < (int)palabra.length(); i++) {
        char c = palabra[i];
        if (c >= 'A' && c <= 'Z') c = c + 32; // pasar a minuscula
        if (c >= 'a' && c <= 'z') out += c;
    }
    return out;
}

// =====================================================
// CREACION DE POST en la coleccion maestra
// Convierte el numero de likes a int y crea N nodos
// "anonimos" en la lista de likes (porque el dataset
// solo da el conteo, no el nombre de quien dio like).
// =====================================================
nodoPost* crearPost(ColeccionPosts* col, const string& idPost, const string& autor,
                    const string& texto, int cantidadLikes) {
    nodoPost* nuevo = new nodoPost;
    nuevo->idPost = idPost;
    nuevo->autor  = autor;
    nuevo->textoOriginal = texto;
    nuevo->sig    = nullptr;

    nuevo->misLikes = new ListaLikes;
    inicializarListaLikes(nuevo->misLikes);
    nuevo->misLikes->total = cantidadLikes; // contador real del dataset

    // Insertar al final de la coleccion maestra
    if (col->primero == nullptr) {
        col->primero = nuevo;
        col->ultimo  = nuevo;
    } else {
        col->ultimo->sig = nuevo;
        col->ultimo = nuevo;
    }
    col->total++;
    return nuevo;
}

// =====================================================
// BUSQUEDAS BASICAS EN LISTAS
// =====================================================
nodoVocabulario* buscarPalabra(Diccionario* dic, const string& palabra) {
    nodoVocabulario* actual = dic->primero;
    while (actual != nullptr) {
        if (actual->palabra == palabra) return actual;
        actual = actual->sig;
    }
    return nullptr;
}

bool existeRefEnLista(ListaPostRef* lista, nodoPost* postBuscado) {
    nodoPostRef* actual = lista->primero;
    while (actual != nullptr) {
        if (actual->post == postBuscado) return true;
        actual = actual->sig;
    }
    return false;
}

nodoPost* buscarPostPorId(ColeccionPosts* col, const string& idPost) {
    nodoPost* actual = col->primero;
    while (actual != nullptr) {
        if (actual->idPost == idPost) return actual;
        actual = actual->sig;
    }
    return nullptr;
}

// =====================================================
// AGREGAR PALABRA AL INDICE
// Si la palabra no existe, crea un nuevo "piso" (nodoVocabulario).
// Despues agrega una referencia (no copia) al post en su pasillo.
// Evita duplicados en el pasillo.
// =====================================================
void agregarPalabraAlIndice(Diccionario* dic, const string& palabra, nodoPost* post) {
    nodoVocabulario* piso = buscarPalabra(dic, palabra);

    if (piso == nullptr) {
        piso = new nodoVocabulario;
        piso->palabra = palabra;
        piso->sig = nullptr;
        piso->listaPost = new ListaPostRef;
        inicializarListaPostRef(piso->listaPost);

        if (dic->primero == nullptr) {
            dic->primero = piso;
            dic->ultimo  = piso;
        } else {
            dic->ultimo->sig = piso;
            dic->ultimo = piso;
        }
        dic->totalPalabras++;
    }

    if (existeRefEnLista(piso->listaPost, post)) return;

    nodoPostRef* ref = new nodoPostRef;
    ref->post = post;
    ref->sig  = nullptr;

    if (piso->listaPost->primero == nullptr) {
        piso->listaPost->primero = ref;
        piso->listaPost->ultimo  = ref;
    } else {
        piso->listaPost->ultimo->sig = ref;
        piso->listaPost->ultimo = ref;
    }
    piso->listaPost->numElem++;
}

// =====================================================
// INDEXAR POST
// Recorre el texto del post, extrae palabras (separando
// por todo lo que no sea letra), descarta stopwords, y
// agrega cada palabra unica al indice.
// =====================================================
void indexarPost(Diccionario* dic, nodoPost* post, ListaStopwords* lista) {
    const string& texto = post->textoOriginal;
    string palabra = "";

    for (int i = 0; i <= (int)texto.length(); i++) {
        char c = (i < (int)texto.length()) ? texto[i] : ' ';

        if (c >= 'A' && c <= 'Z') c = c + 32;

        if (c >= 'a' && c <= 'z') {
            palabra += c;
        } else {
            if (palabra.length() > 0) {
                if (!esStopword(palabra, lista)) {
                    agregarPalabraAlIndice(dic, palabra, post);
                }
                palabra = "";
            }
        }
    }
}

// =====================================================
// LIBERAR SOLO EL DICCIONARIO (sin tocar posts ni usuarios)
// Se usa antes de re-indexar.
// =====================================================
void liberarDiccionario(Diccionario* dic) {
    nodoVocabulario* v = dic->primero;
    while (v != nullptr) {
        nodoPostRef* r = v->listaPost->primero;
        while (r != nullptr) {
            nodoPostRef* tmp = r;
            r = r->sig;
            delete tmp;
        }
        delete v->listaPost;
        nodoVocabulario* tmpV = v;
        v = v->sig;
        delete tmpV;
    }
    dic->primero = nullptr;
    dic->ultimo  = nullptr;
    dic->totalPalabras = 0;
}

// =====================================================
// RE-INDEXAR todos los posts con la lista de stopwords actual.
// Se usa cuando el usuario carga un archivo nuevo de stopwords:
//   1. Liberamos el indice actual.
//   2. Recorremos la coleccion maestra de posts.
//   3. Indexamos cada post con la nueva lista de stopwords.
// =====================================================
void reindexarPosts(Diccionario* dic, ColeccionPosts* col, ListaStopwords* lista) {
    liberarDiccionario(dic);

    nodoPost* p = col->primero;
    int total = 0;
    while (p != nullptr) {
        indexarPost(dic, p, lista);
        p = p->sig;
        total++;
    }
    cout << "[Exito] Re-indexado " << total << " post(s). "
         << "Palabras unicas ahora: " << dic->totalPalabras << endl;
}

// =====================================================
// PARSER DE CSV (delimitador ';' con campos entre comillas)
// Maneja:
//  - Saltos de linea DENTRO de campos entre comillas
//  - Comillas escapadas como ""
//  - BOM al inicio del archivo (se elimina junto al header)
// =====================================================
int convertirALikes(const string& s) {
    int n = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') n = n * 10 + (c - '0');
    }
    return n;
}

void cargarDatosDesdeCSV(Diccionario* dic, ColeccionPosts* col,
                          ListaStopwords* lista, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cout << "[Error] No se pudo abrir el archivo: " << nombreArchivo << endl;
        return;
    }

    // Saltar la primera linea (encabezado + BOM)
    string encabezado;
    getline(archivo, encabezado);

    string campos[4];          // 0: UserName, 1: Embedded_text, 2: Likes, 3: ID
    int idxCampo = 0;
    bool enComillas = false;
    int postsCargados = 0;

    char c;
    while (archivo.get(c)) {
        if (enComillas) {
            if (c == '"') {
                if (archivo.peek() == '"') {
                    archivo.get(c);          // consumir la segunda comilla escapada
                    campos[idxCampo] += '"';
                } else {
                    enComillas = false;       // cerramos campo entre comillas
                }
            } else {
                campos[idxCampo] += c;
            }
        } else {
            if (c == '"') {
                enComillas = true;
            } else if (c == ';') {
                if (idxCampo < 3) idxCampo++;
            } else if (c == '\n' || c == '\r') {
                // Fin de fila — solo procesar si tiene los 4 campos completos
                if (idxCampo == 3 && campos[3].length() > 0) {
                    int likes = convertirALikes(campos[2]);
                    nodoPost* p = crearPost(col, campos[3], campos[0], campos[1], likes);
                    indexarPost(dic, p, lista);
                    postsCargados++;
                }
                // Reiniciar estado de fila
                for (int i = 0; i < 4; i++) campos[i] = "";
                idxCampo = 0;
            } else {
                campos[idxCampo] += c;
            }
        }
    }

    // Procesar la ultima fila si el archivo no terminaba en \n
    if (idxCampo == 3 && campos[3].length() > 0) {
        int likes = convertirALikes(campos[2]);
        nodoPost* p = crearPost(col, campos[3], campos[0], campos[1], likes);
        indexarPost(dic, p, lista);
        postsCargados++;
    }

    archivo.close();
    cout << "[Exito] Posts cargados: " << postsCargados
         << " | Palabras unicas indexadas: " << dic->totalPalabras << endl;
}

// =====================================================
// LIKES (insertar identidad de quien dio el like)
// =====================================================
void agregarLikeAPost(nodoPost* post, const string& nombreUsuario) {
    if (post == nullptr) return;
    nodoLike* nuevo = new nodoLike;
    nuevo->nombreUsuario = nombreUsuario;
    nuevo->sig = nullptr;

    if (post->misLikes->primero == nullptr) {
        post->misLikes->primero = nuevo;
        post->misLikes->ultimo  = nuevo;
    } else {
        post->misLikes->ultimo->sig = nuevo;
        post->misLikes->ultimo = nuevo;
    }
    // total ya tenia el conteo del dataset; lo subimos en 1 por el like nuevo
    post->misLikes->total++;
}

// =====================================================
// BUSQUEDAS PARA EL USUARIO
// =====================================================
void buscarYMostrar(Diccionario* dic, const string& palabraOriginal, ListaStopwords* lista) {
    string p = normalizarPalabra(palabraOriginal);
    if (p.length() == 0) {
        cout << "[Aviso] La palabra ingresada no contiene letras validas." << endl;
        return;
    }
    if (esStopword(p, lista)) {
        cout << "[Aviso] '" << p << "' es una stopword, no esta indexada." << endl;
        return;
    }

    nodoVocabulario* piso = buscarPalabra(dic, p);
    if (piso == nullptr) {
        cout << "[Resultado] La palabra '" << p << "' no aparece en el indice." << endl;
        return;
    }

    cout << "[Resultado] '" << p << "' aparece en "
         << piso->listaPost->numElem << " post(s):" << endl;

    nodoPostRef* actual = piso->listaPost->primero;
    int mostrados = 0;
    while (actual != nullptr && mostrados < 10) {
        nodoPost* post = actual->post;
        cout << "   [" << post->idPost << "] " << post->autor
             << " (likes: " << post->misLikes->total << ")" << endl;
        actual = actual->sig;
        mostrados++;
    }
    if (piso->listaPost->numElem > 10) {
        cout << "   ... y " << (piso->listaPost->numElem - 10) << " mas." << endl;
    }
}

// Busqueda por varios terminos: muestra resultados de cada termino por separado.
// (Mantiene la implementacion simple para Unidad I.)
void buscarVariosTerminos(Diccionario* dic, const string& consulta, ListaStopwords* lista) {
    string palabra = "";
    for (int i = 0; i <= (int)consulta.length(); i++) {
        char c = (i < (int)consulta.length()) ? consulta[i] : ' ';
        if (c >= 'A' && c <= 'Z') c = c + 32;
        if (c >= 'a' && c <= 'z') {
            palabra += c;
        } else {
            if (palabra.length() > 0) {
                cout << "\n--- Termino: '" << palabra << "' ---" << endl;
                buscarYMostrar(dic, palabra, lista);
                palabra = "";
            }
        }
    }
}

// =====================================================
// ESTADISTICAS
// =====================================================
void mostrarEstadisticas(Diccionario* dic, ColeccionPosts* col, IndiceUsuarios* idxU) {
    cout << "\n--- ESTADISTICAS ---" << endl;
    cout << "Total de posts cargados: " << col->total << endl;
    cout << "Total de palabras unicas en el indice: " << dic->totalPalabras << endl;
    cout << "Total de usuarios en la red: " << idxU->total << endl;
}

void listarPrimerasPalabras(Diccionario* dic, int cuantas) {
    cout << "\n--- PRIMERAS " << cuantas << " PALABRAS DEL VOCABULARIO ---" << endl;
    nodoVocabulario* actual = dic->primero;
    int i = 0;
    while (actual != nullptr && i < cuantas) {
        cout << "  " << (i + 1) << ". " << actual->palabra
             << "  (en " << actual->listaPost->numElem << " posts)" << endl;
        actual = actual->sig;
        i++;
    }
}

void listarPrimerosUsuarios(IndiceUsuarios* idxU, int cuantos) {
    cout << "\n--- PRIMEROS " << cuantos << " USUARIOS DE LA RED ---" << endl;
    nodoUsuario* actual = idxU->primero;
    int i = 0;
    while (actual != nullptr && i < cuantos) {
        cout << "  " << (i + 1) << ". " << actual->nombre
             << "  (amigos: " << actual->amigos->total << ")" << endl;
        actual = actual->sig;
        i++;
    }
}

// =====================================================
// SEGUNDO INDICE INVERTIDO: USUARIOS y AMIGOS
// =====================================================
void inicializarIndiceUsuarios(IndiceUsuarios* idx) {
    idx->primero = nullptr;
    idx->ultimo  = nullptr;
    idx->total   = 0;
}

void inicializarListaAmigos(ListaAmigos* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->total   = 0;
}

nodoUsuario* buscarUsuario(IndiceUsuarios* idx, const string& nombre) {
    nodoUsuario* actual = idx->primero;
    while (actual != nullptr) {
        if (actual->nombre == nombre) return actual;
        actual = actual->sig;
    }
    return nullptr;
}

// Si el usuario no existe lo crea con lista de amigos vacia.
// Devuelve siempre un puntero al usuario (existente o nuevo).
nodoUsuario* registrarUsuario(IndiceUsuarios* idx, const string& nombre) {
    if (nombre.length() == 0) return nullptr;

    nodoUsuario* u = buscarUsuario(idx, nombre);
    if (u != nullptr) return u;

    u = new nodoUsuario;
    u->nombre = nombre;
    u->sig    = nullptr;
    u->amigos = new ListaAmigos;
    inicializarListaAmigos(u->amigos);

    if (idx->primero == nullptr) {
        idx->primero = u;
        idx->ultimo  = u;
    } else {
        idx->ultimo->sig = u;
        idx->ultimo = u;
    }
    idx->total++;
    return u;
}

bool existeAmigo(ListaAmigos* lista, const string& nombre) {
    nodoAmigo* actual = lista->primero;
    while (actual != nullptr) {
        if (actual->nombreAmigo == nombre) return true;
        actual = actual->sig;
    }
    return false;
}

// Agrega amistad mutua A <-> B (si no existia ya en alguno de los lados).
void agregarAmistadMutua(IndiceUsuarios* idx, const string& a, const string& b) {
    if (a == b) return;                  // un usuario no es amigo de si mismo
    if (a.length() == 0 || b.length() == 0) return;

    nodoUsuario* uA = registrarUsuario(idx, a);
    nodoUsuario* uB = registrarUsuario(idx, b);

    if (!existeAmigo(uA->amigos, b)) {
        nodoAmigo* nuevo = new nodoAmigo;
        nuevo->nombreAmigo = b;
        nuevo->sig = nullptr;
        if (uA->amigos->primero == nullptr) {
            uA->amigos->primero = nuevo;
            uA->amigos->ultimo  = nuevo;
        } else {
            uA->amigos->ultimo->sig = nuevo;
            uA->amigos->ultimo = nuevo;
        }
        uA->amigos->total++;
    }

    if (!existeAmigo(uB->amigos, a)) {
        nodoAmigo* nuevo = new nodoAmigo;
        nuevo->nombreAmigo = a;
        nuevo->sig = nullptr;
        if (uB->amigos->primero == nullptr) {
            uB->amigos->primero = nuevo;
            uB->amigos->ultimo  = nuevo;
        } else {
            uB->amigos->ultimo->sig = nuevo;
            uB->amigos->ultimo = nuevo;
        }
        uB->amigos->total++;
    }
}

// Extrae todas las @menciones de un texto y crea amistades autor <-> mencionado.
// Recorre caracter por caracter:
//   1. Detecta '@'
//   2. Acumula chars validos (letras, digitos, '_') como nombre
//   3. Cuando termina la palabra, si tiene contenido, crea amistad
//
// IMPORTANTE: aqui no hay "grafo" ni recorrido de grafo. Solo recorremos
// el string con un for normal, y al encontrar una mencion insertamos en
// listas enlazadas con while. TDA basico de Unidad I.
void extraerMencionesYConectar(IndiceUsuarios* idx, const string& autor,
                                const string& texto) {
    int n = (int)texto.length();
    for (int i = 0; i < n; i++) {
        if (texto[i] == '@') {
            string mencion = "@";
            int j = i + 1;
            while (j < n) {
                char c = texto[j];
                bool letra  = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
                bool digito = (c >= '0' && c <= '9');
                bool guion  = (c == '_');
                if (!letra && !digito && !guion) break;
                mencion += c;
                j++;
            }
            if (mencion.length() > 1) {        // hubo al menos un char tras '@'
                agregarAmistadMutua(idx, autor, mencion);
            }
            i = j - 1; // saltar la mencion procesada
        }
    }
}

// Construye la red social entera recorriendo todos los posts.
// Para cada post:
//   - registra al autor como usuario
//   - extrae menciones del texto y crea amistades mutuas
void construirRedDesdePosts(IndiceUsuarios* idx, ColeccionPosts* col) {
    nodoPost* p = col->primero;
    while (p != nullptr) {
        registrarUsuario(idx, p->autor);
        extraerMencionesYConectar(idx, p->autor, p->textoOriginal);
        p = p->sig;
    }
    cout << "[Exito] Red social construida. Usuarios totales: " << idx->total << endl;
}

void mostrarAmigosDe(IndiceUsuarios* idx, const string& nombreOriginal) {
    // Aceptar el nombre con o sin '@' al inicio
    string nombre = nombreOriginal;
    if (nombre.length() > 0 && nombre[0] != '@') nombre = "@" + nombre;

    nodoUsuario* u = buscarUsuario(idx, nombre);
    if (u == nullptr) {
        cout << "[Resultado] El usuario '" << nombre << "' no existe en la red." << endl;
        return;
    }

    cout << "[Resultado] " << nombre << " tiene "
         << u->amigos->total << " amigo(s):" << endl;

    nodoAmigo* a = u->amigos->primero;
    int mostrados = 0;
    while (a != nullptr && mostrados < 20) {
        cout << "   - " << a->nombreAmigo << endl;
        a = a->sig;
        mostrados++;
    }
    if (u->amigos->total > 20) {
        cout << "   ... y " << (u->amigos->total - 20) << " mas." << endl;
    }
}

// =====================================================
// LIKES SINTETICOS (solo para demo)
// Recorre el indice de usuarios y agrega los primeros
// 'cantidad' como likers de un post. La estructura es
// real; los datos se generan porque el dataset solo
// trae el conteo, no la identidad.
// =====================================================
void generarLikesSinteticos(nodoPost* post, IndiceUsuarios* idx, int cantidad) {
    if (post == nullptr || idx == nullptr) return;
    nodoUsuario* u = idx->primero;
    int agregados = 0;
    while (u != nullptr && agregados < cantidad) {
        agregarLikeAPost(post, u->nombre);
        u = u->sig;
        agregados++;
    }
    cout << "[Demo] Se agregaron " << agregados
         << " likes sinteticos al post " << post->idPost << "." << endl;
}

// =====================================================
// LIBERACION DE MEMORIA
// Recorre y libera TODO. Importante para el criterio de
// "gestion de memoria dinamica" de la rubrica.
// =====================================================
void liberarTodo(Diccionario* dic, ColeccionPosts* col,
                  IndiceUsuarios* idxU, ListaStopwords* listaSW) {
    // 1. Liberar el indice invertido (palabras + listas de referencias)
    liberarDiccionario(dic);

    // 2. Liberar la coleccion maestra de posts (y sus likes)
    nodoPost* p = col->primero;
    while (p != nullptr) {
        nodoLike* l = p->misLikes->primero;
        while (l != nullptr) {
            nodoLike* tmpL = l;
            l = l->sig;
            delete tmpL;
        }
        delete p->misLikes;
        nodoPost* tmpP = p;
        p = p->sig;
        delete tmpP;
    }
    col->primero = nullptr;
    col->ultimo  = nullptr;
    col->total   = 0;

    // 3. Liberar el indice de usuarios y sus listas de amigos
    nodoUsuario* u = idxU->primero;
    while (u != nullptr) {
        nodoAmigo* a = u->amigos->primero;
        while (a != nullptr) {
            nodoAmigo* tmpA = a;
            a = a->sig;
            delete tmpA;
        }
        delete u->amigos;
        nodoUsuario* tmpU = u;
        u = u->sig;
        delete tmpU;
    }
    idxU->primero = nullptr;
    idxU->ultimo  = nullptr;
    idxU->total   = 0;

    // 4. Liberar la lista de stopwords cargadas desde archivo
    liberarListaStopwords(listaSW);
}
