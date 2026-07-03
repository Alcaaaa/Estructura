#include <iostream>
#include <fstream>
#include <string>
#include "estructura.h"
#include "funciones.h"

using namespace std;

// -- Inicializadores --

void inicializarDiccionario(Diccionario* dic) {
    dic->primero = nullptr;
    dic->ultimo  = nullptr;
    dic->curr    = nullptr;
    dic->totalPalabras = 0;
}

void inicializarColeccion(ColeccionPosts* col) {
    col->primero = nullptr;
    col->ultimo  = nullptr;
    col->curr    = nullptr;
    col->total   = 0;
}

void inicializarListaPostRef(ListaPostRef* lista) {
    lista->primero = nullptr;
    lista->ultimo  = nullptr;
    lista->curr    = nullptr;
    lista->numElem = 0;
}

void inicializarListaLikes(ListaLikes* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->curr    = nullptr;
    l->total   = 0;
}

void inicializarListaStopwords(ListaStopwords* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->curr    = nullptr;
    l->total   = 0;
}

// -- Stopwords --

bool estaEnListaStopwords(ListaStopwords* lista, const string& palabra) {
    if (lista == nullptr) return false;
    lista->curr = lista->primero;
    while (lista->curr != nullptr) {
        if (lista->curr->palabra == palabra) return true;
        lista->curr = lista->curr->sig;
    }
    return false;
}

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

void cargarStopwordsDesdeArchivo(ListaStopwords* lista, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cout << "Aviso: no se pudo abrir " << nombreArchivo << endl;
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
    cout << leidas << " stopwords cargadas. Total en lista: " << lista->total << endl;
}

void liberarListaStopwords(ListaStopwords* lista) {
    if (lista == nullptr) return;
    nodoStopword* tmp = lista->primero;
    while (tmp != nullptr) {
        nodoStopword* sig = tmp->sig;
        delete tmp;
        tmp = sig;
    }
    lista->primero = nullptr;
    lista->ultimo  = nullptr;
    lista->curr    = nullptr;
    lista->total   = 0;
}

bool esStopword(const string& p, ListaStopwords* lista) {
    if (p.length() <= 1) return true;
    return estaEnListaStopwords(lista, p);
}

// Convierte a minusculas y descarta todo lo que no sea a-z
string normalizarPalabra(const string& palabra) {
    string out = "";
    for (int i = 0; i < (int)palabra.length(); i++) {
        char c = palabra[i];
        if (c >= 'A' && c <= 'Z') c = c + 32;
        if (c >= 'a' && c <= 'z') out += c;
    }
    return out;
}

// -- Posts --

nodoPost* crearPost(ColeccionPosts* col, const string& idPost, const string& autor,
                    const string& texto, int cantidadLikes) {
    nodoPost* nuevo = new nodoPost;
    nuevo->idPost = idPost;
    nuevo->autor  = autor;
    nuevo->textoOriginal = texto;
    nuevo->sig    = nullptr;

    nuevo->misLikes = new ListaLikes;
    inicializarListaLikes(nuevo->misLikes);
    // el dataset solo trae el conteo, no los nombres; se guarda como total
    nuevo->misLikes->total = cantidadLikes;

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

// -- Busquedas internas --

nodoVocabulario* buscarPalabra(Diccionario* dic, const string& palabra) {
    dic->curr = dic->primero;
    while (dic->curr != nullptr) {
        if (dic->curr->palabra == palabra) return dic->curr;
        dic->curr = dic->curr->sig;
    }
    return nullptr;
}

bool existeRefEnLista(ListaPostRef* lista, nodoPost* postBuscado) {
    lista->curr = lista->primero;
    while (lista->curr != nullptr) {
        if (lista->curr->post == postBuscado) return true;
        lista->curr = lista->curr->sig;
    }
    return false;
}

nodoPost* buscarPostPorId(ColeccionPosts* col, const string& idPost) {
    col->curr = col->primero;
    while (col->curr != nullptr) {
        if (col->curr->idPost == idPost) return col->curr;
        col->curr = col->curr->sig;
    }
    return nullptr;
}

// Agrega la palabra al indice; si no existe la crea, luego añade referencia al post
void agregarPalabraAlIndice(Diccionario* dic, const string& palabra, nodoPost* post) {
    nodoVocabulario* nodo = buscarPalabra(dic, palabra);

    if (nodo == nullptr) {
        nodo = new nodoVocabulario;
        nodo->palabra = palabra;
        nodo->sig = nullptr;
        nodo->listaPost = new ListaPostRef;
        inicializarListaPostRef(nodo->listaPost);

        if (dic->primero == nullptr) {
            dic->primero = nodo;
            dic->ultimo  = nodo;
        } else {
            dic->ultimo->sig = nodo;
            dic->ultimo = nodo;
        }
        dic->totalPalabras++;
    }

    if (existeRefEnLista(nodo->listaPost, post)) return;

    nodoPostRef* ref = new nodoPostRef;
    ref->post = post;
    ref->sig  = nullptr;

    if (nodo->listaPost->primero == nullptr) {
        nodo->listaPost->primero = ref;
        nodo->listaPost->ultimo  = ref;
    } else {
        nodo->listaPost->ultimo->sig = ref;
        nodo->listaPost->ultimo = ref;
    }
    nodo->listaPost->numElem++;
}

// Recorre el texto, extrae palabras, filtra stopwords y agrega al indice
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

// Libera el diccionario sin tocar los posts (se usa antes de re-indexar)
void liberarDiccionario(Diccionario* dic) {
    nodoVocabulario* v = dic->primero;
    while (v != nullptr) {
        nodoPostRef* r = v->listaPost->primero;
        while (r != nullptr) {
            nodoPostRef* tmp = r->sig;
            delete r;
            r = tmp;
        }
        delete v->listaPost;
        nodoVocabulario* tmp = v->sig;
        delete v;
        v = tmp;
    }
    dic->primero = nullptr;
    dic->ultimo  = nullptr;
    dic->curr    = nullptr;
    dic->totalPalabras = 0;
}

void reindexarPosts(Diccionario* dic, ColeccionPosts* col, ListaStopwords* lista) {
    liberarDiccionario(dic);

    nodoPost* p = col->primero;
    int total = 0;
    while (p != nullptr) {
        indexarPost(dic, p, lista);
        p = p->sig;
        total++;
    }
    cout << "Re-indexado " << total << " posts. Palabras unicas: "
         << dic->totalPalabras << endl;
}

// -- Parser CSV (delimitador ';', campos entre comillas, salta BOM en encabezado) --

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
        cout << "Error: no se pudo abrir " << nombreArchivo << endl;
        return;
    }

    string encabezado;
    getline(archivo, encabezado);

    string campos[4];   // 0: UserName, 1: Embedded_text, 2: Likes, 3: ID
    int idxCampo = 0;
    bool enComillas = false;
    int postsCargados = 0;

    char c;
    while (archivo.get(c)) {
        if (enComillas) {
            if (c == '"') {
                if (archivo.peek() == '"') {
                    archivo.get(c);
                    campos[idxCampo] += '"';
                } else {
                    enComillas = false;
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
                if (idxCampo == 3 && campos[3].length() > 0) {
                    int likes = convertirALikes(campos[2]);
                    nodoPost* p = crearPost(col, campos[3], campos[0], campos[1], likes);
                    indexarPost(dic, p, lista);
                    postsCargados++;
                }
                for (int i = 0; i < 4; i++) campos[i] = "";
                idxCampo = 0;
            } else {
                campos[idxCampo] += c;
            }
        }
    }

    // ultima fila si el archivo no termina en salto de linea
    if (idxCampo == 3 && campos[3].length() > 0) {
        int likes = convertirALikes(campos[2]);
        nodoPost* p = crearPost(col, campos[3], campos[0], campos[1], likes);
        indexarPost(dic, p, lista);
        postsCargados++;
    }

    archivo.close();
    cout << "Posts cargados: " << postsCargados
         << " | Palabras unicas: " << dic->totalPalabras << endl;
}

// -- Likes --

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
    post->misLikes->total++;
}

// -- Busqueda en el indice --

void buscarYMostrar(Diccionario* dic, const string& palabraOriginal, ListaStopwords* lista) {
    string p = normalizarPalabra(palabraOriginal);
    if (p.length() == 0) {
        cout << "Aviso: la palabra ingresada no tiene letras validas." << endl;
        return;
    }
    if (esStopword(p, lista)) {
        cout << "'" << p << "' es stopword, no esta indexada." << endl;
        return;
    }

    nodoVocabulario* nodo = buscarPalabra(dic, p);
    if (nodo == nullptr) {
        cout << "'" << p << "' no aparece en el indice." << endl;
        return;
    }

    cout << "'" << p << "' aparece en "
         << nodo->listaPost->numElem << " post(s):" << endl;

    nodo->listaPost->curr = nodo->listaPost->primero;
    int mostrados = 0;
    while (nodo->listaPost->curr != nullptr && mostrados < 10) {
        nodoPost* post = nodo->listaPost->curr->post;
        cout << "   [" << post->idPost << "] " << post->autor
             << " (likes: " << post->misLikes->total << ")" << endl;
        nodo->listaPost->curr = nodo->listaPost->curr->sig;
        mostrados++;
    }
    if (nodo->listaPost->numElem > 10) {
        cout << "   ... y " << (nodo->listaPost->numElem - 10) << " mas." << endl;
    }
}

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

// -- Estadisticas --

void mostrarEstadisticas(Diccionario* dic, ColeccionPosts* col, IndiceUsuarios* idxU) {
    cout << "\n--- ESTADISTICAS ---" << endl;
    cout << "Posts cargados:          " << col->total << endl;
    cout << "Palabras unicas:         " << dic->totalPalabras << endl;
    cout << "Usuarios en la red:      " << idxU->total << endl;
}

void listarPrimerasPalabras(Diccionario* dic, int cuantas) {
    cout << "\n--- Primeras " << cuantas << " palabras del vocabulario ---" << endl;
    dic->curr = dic->primero;
    int i = 0;
    while (dic->curr != nullptr && i < cuantas) {
        cout << "  " << (i + 1) << ". " << dic->curr->palabra
             << "  (" << dic->curr->listaPost->numElem << " posts)" << endl;
        dic->curr = dic->curr->sig;
        i++;
    }
}

void listarPrimerosUsuarios(IndiceUsuarios* idxU, int cuantos) {
    cout << "\n--- Primeros " << cuantos << " usuarios ---" << endl;
    idxU->curr = idxU->primero;
    int i = 0;
    while (idxU->curr != nullptr && i < cuantos) {
        cout << "  " << (i + 1) << ". " << idxU->curr->nombre
             << "  (amigos: " << idxU->curr->amigos->total << ")" << endl;
        idxU->curr = idxU->curr->sig;
        i++;
    }
}

// -- Indice de usuarios --

void inicializarIndiceUsuarios(IndiceUsuarios* idx) {
    idx->primero = nullptr;
    idx->ultimo  = nullptr;
    idx->curr    = nullptr;
    idx->total   = 0;
}

void inicializarListaAmigos(ListaAmigos* l) {
    l->primero = nullptr;
    l->ultimo  = nullptr;
    l->curr    = nullptr;
    l->total   = 0;
}

nodoUsuario* buscarUsuario(IndiceUsuarios* idx, const string& nombre) {
    idx->curr = idx->primero;
    while (idx->curr != nullptr) {
        if (idx->curr->nombre == nombre) return idx->curr;
        idx->curr = idx->curr->sig;
    }
    return nullptr;
}

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
    lista->curr = lista->primero;
    while (lista->curr != nullptr) {
        if (lista->curr->nombreAmigo == nombre) return true;
        lista->curr = lista->curr->sig;
    }
    return false;
}

void agregarAmistadMutua(IndiceUsuarios* idx, const string& a, const string& b) {
    if (a == b) return;
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

// Detecta menciones @usuario en el texto y registra amistad autor <-> mencionado
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
            if (mencion.length() > 1) {
                agregarAmistadMutua(idx, autor, mencion);
            }
            i = j - 1;
        }
    }
}

void construirRedDesdePosts(IndiceUsuarios* idx, ColeccionPosts* col) {
    nodoPost* p = col->primero;
    while (p != nullptr) {
        registrarUsuario(idx, p->autor);
        extraerMencionesYConectar(idx, p->autor, p->textoOriginal);
        p = p->sig;
    }
    cout << "Red social construida. Usuarios totales: " << idx->total << endl;
}

void mostrarAmigosDe(IndiceUsuarios* idx, const string& nombreOriginal) {
    string nombre = nombreOriginal;
    if (nombre.length() > 0 && nombre[0] != '@') nombre = "@" + nombre;

    nodoUsuario* u = buscarUsuario(idx, nombre);
    if (u == nullptr) {
        cout << "Usuario '" << nombre << "' no encontrado en la red." << endl;
        return;
    }

    cout << nombre << " tiene " << u->amigos->total << " amigo(s):" << endl;

    u->amigos->curr = u->amigos->primero;
    int mostrados = 0;
    while (u->amigos->curr != nullptr && mostrados < 20) {
        cout << "   - " << u->amigos->curr->nombreAmigo << endl;
        u->amigos->curr = u->amigos->curr->sig;
        mostrados++;
    }
    if (u->amigos->total > 20) {
        cout << "   ... y " << (u->amigos->total - 20) << " mas." << endl;
    }
}

// Genera likes usando nombres reales del indice de usuarios (demo, el dataset no trae identidad)
void generarLikesSinteticos(nodoPost* post, IndiceUsuarios* idx, int cantidad) {
    if (post == nullptr || idx == nullptr) return;
    idx->curr = idx->primero;
    int agregados = 0;
    while (idx->curr != nullptr && agregados < cantidad) {
        agregarLikeAPost(post, idx->curr->nombre);
        idx->curr = idx->curr->sig;
        agregados++;
    }
    cout << agregados << " likes agregados al post " << post->idPost << "." << endl;
}

// -- Liberacion de memoria --
// (usa variables locales para no corromper curr mientras se libera)

void liberarTodo(Diccionario* dic, ColeccionPosts* col,
                  IndiceUsuarios* idxU, ListaStopwords* listaSW) {
    liberarDiccionario(dic);

    nodoPost* p = col->primero;
    while (p != nullptr) {
        nodoLike* l = p->misLikes->primero;
        while (l != nullptr) {
            nodoLike* sig = l->sig;
            delete l;
            l = sig;
        }
        delete p->misLikes;
        nodoPost* sig = p->sig;
        delete p;
        p = sig;
    }
    col->primero = nullptr;
    col->ultimo  = nullptr;
    col->curr    = nullptr;
    col->total   = 0;

    nodoUsuario* u = idxU->primero;
    while (u != nullptr) {
        nodoAmigo* a = u->amigos->primero;
        while (a != nullptr) {
            nodoAmigo* sig = a->sig;
            delete a;
            a = sig;
        }
        delete u->amigos;
        nodoUsuario* sig = u->sig;
        delete u;
        u = sig;
    }
    idxU->primero = nullptr;
    idxU->ultimo  = nullptr;
    idxU->curr    = nullptr;
    idxU->total   = 0;

    liberarListaStopwords(listaSW);
}

// ===== Grafo No Dirigido (Entrega II) =====

void inicializarGrafo(Grafo* g) {
    g->primero = nullptr;
    g->ultimo = nullptr;
    g->totalVertices = 0;
}

nodoVertice* buscarVertice(Grafo* g, const string& nombre) {
    nodoVertice* v = g->primero;
    while (v != nullptr) {
        if (v->usuario->nombre == nombre) return v;
        v = v->sig;
    }
    return nullptr;
}

nodoVertice* registrarVertice(Grafo* g, nodoUsuario* usuario) {
    if (usuario == nullptr) return nullptr;
    nodoVertice* v = buscarVertice(g, usuario->nombre);
    if (v != nullptr) return v;

    v = new nodoVertice;
    v->usuario = usuario;
    v->sig = nullptr;
    v->adyacentes = new ListaAristas;
    v->adyacentes->primero = nullptr;
    v->adyacentes->ultimo = nullptr;
    v->adyacentes->total = 0;

    if (g->primero == nullptr) {
        g->primero = v;
        g->ultimo = v;
    } else {
        g->ultimo->sig = v;
        g->ultimo = v;
    }
    g->totalVertices++;
    return v;
}

bool existeArista(ListaAristas* lista, nodoVertice* dest) {
    nodoArista* a = lista->primero;
    while (a != nullptr) {
        if (a->destino == dest) return true;
        a = a->sig;
    }
    return false;
}

void agregarAristaDirecta(nodoVertice* origen, nodoVertice* destino) {
    if (origen == nullptr || destino == nullptr || origen == destino) return;
    if (existeArista(origen->adyacentes, destino)) return;

    nodoArista* nuevo = new nodoArista;
    nuevo->destino = destino;
    nuevo->sig = nullptr;

    if (origen->adyacentes->primero == nullptr) {
        origen->adyacentes->primero = nuevo;
        origen->adyacentes->ultimo = nuevo;
    } else {
        origen->adyacentes->ultimo->sig = nuevo;
        origen->adyacentes->ultimo = nuevo;
    }
    origen->adyacentes->total++;
}

void construirGrafo(Grafo* g, IndiceUsuarios* idxU) {
    inicializarGrafo(g);

    // 1. Crear todos los vertices
    nodoUsuario* u = idxU->primero;
    while (u != nullptr) {
        registrarVertice(g, u);
        u = u->sig;
    }

    // 2. Crear las aristas a partir de ListaAmigos de cada usuario
    u = idxU->primero;
    while (u != nullptr) {
        nodoVertice* origen = buscarVertice(g, u->nombre);
        if (origen != nullptr) {
            nodoAmigo* amigo = u->amigos->primero;
            while (amigo != nullptr) {
                nodoVertice* destino = buscarVertice(g, amigo->nombreAmigo);
                if (destino != nullptr && destino != origen) {
                    // El grafo es no dirigido: conectamos en ambos sentidos y evitamos duplicados
                    agregarAristaDirecta(origen, destino);
                    agregarAristaDirecta(destino, origen);
                }
                amigo = amigo->sig;
            }
        }
        u = u->sig;
    }
    cout << "[Grafo] Construido con " << g->totalVertices << " vertices." << endl;
}

void validarSimetriaGrafo(Grafo* g) {
    cout << "[Grafo] Validando simetria..." << endl;
    nodoVertice* v = g->primero;
    int errores = 0;
    while (v != nullptr) {
        nodoArista* a = v->adyacentes->primero;
        while (a != nullptr) {
            if (!existeArista(a->destino->adyacentes, v)) {
                errores++;
            }
            a = a->sig;
        }
        v = v->sig;
    }
    if (errores == 0) {
        cout << "[Grafo] Validacion exitosa: todas las relaciones son simetricas (no dirigidas)." << endl;
    } else {
        cout << "[Grafo] Alerta: se detectaron " << errores << " aristas asimetricas." << endl;
    }
}

void liberarGrafo(Grafo* g) {
    nodoVertice* v = g->primero;
    while (v != nullptr) {
        nodoArista* a = v->adyacentes->primero;
        while (a != nullptr) {
            nodoArista* tmp = a->sig;
            delete a;
            a = tmp;
        }
        delete v->adyacentes;
        nodoVertice* tmp = v->sig;
        delete v;
        v = tmp;
    }
    g->primero = nullptr;
    g->ultimo = nullptr;
    g->totalVertices = 0;
}

// ===== Algoritmo BFS y Grados de Conexión =====

void inicializarListaContactos(ListaContactos* l) {
    l->primero = nullptr;
    l->ultimo = nullptr;
    l->total = 0;
}

void agregarContacto(ListaContactos* l, nodoVertice* v) {
    nodoContacto* nuevo = new nodoContacto;
    nuevo->vertice = v;
    nuevo->sig = nullptr;
    if (l->primero == nullptr) {
        l->primero = nuevo;
        l->ultimo = nuevo;
    } else {
        l->ultimo->sig = nuevo;
        l->ultimo = nuevo;
    }
    l->total++;
}

void liberarListaContactos(ListaContactos* l) {
    nodoContacto* c = l->primero;
    while (c != nullptr) {
        nodoContacto* tmp = c->sig;
        delete c;
        c = tmp;
    }
    l->primero = nullptr;
    l->ultimo = nullptr;
    l->total = 0;
}

// Estructuras y funciones auxiliares para Cola personalizada (BFS propio)
struct nodoQueue {
    nodoVertice* vertice;
    nodoQueue* sig;
};

struct ColaVertices {
    nodoQueue* primero;
    nodoQueue* ultimo;
};

void inicializarCola(ColaVertices* c) {
    c->primero = nullptr;
    c->ultimo = nullptr;
}

void encolar(ColaVertices* c, nodoVertice* v) {
    nodoQueue* nuevo = new nodoQueue;
    nuevo->vertice = v;
    nuevo->sig = nullptr;
    if (c->primero == nullptr) {
        c->primero = nuevo;
        c->ultimo = nuevo;
    } else {
        c->ultimo->sig = nuevo;
        c->ultimo = nuevo;
    }
}

nodoVertice* desencolar(ColaVertices* c) {
    if (c->primero == nullptr) return nullptr;
    nodoQueue* temp = c->primero;
    nodoVertice* v = temp->vertice;
    c->primero = c->primero->sig;
    if (c->primero == nullptr) {
        c->ultimo = nullptr;
    }
    delete temp;
    return v;
}

bool colaVacia(ColaVertices* c) {
    return c->primero == nullptr;
}

int tamanoCola(ColaVertices* c) {
    int cont = 0;
    nodoQueue* curr = c->primero;
    while (curr != nullptr) {
        cont++;
        curr = curr->sig;
    }
    return cont;
}

// Limpiar estados de visitado en el grafo para el BFS propio
void limpiarVisitasGrafo(Grafo* g) {
    nodoVertice* v = g->primero;
    while (v != nullptr) {
        v->visitado = false;
        v->nivelBfs = -1;
        v = v->sig;
    }
}

ResultadoBFS obtenerGradosConexion(Grafo* g, const string& nombreRaiz) {
    ResultadoBFS res;
    inicializarListaContactos(&res.grado1);
    inicializarListaContactos(&res.grado2);
    inicializarListaContactos(&res.grado3);

    string raiz = nombreRaiz;
    if (raiz.length() > 0 && raiz[0] != '@') {
        raiz = "@" + raiz;
    }

    nodoVertice* start = buscarVertice(g, raiz);
    if (start == nullptr) {
        return res;
    }

    // Inicializar visitas del grafo antes de correr el BFS
    limpiarVisitasGrafo(g);

    ColaVertices q;
    inicializarCola(&q);

    start->visitado = true;
    start->nivelBfs = 0;
    encolar(&q, start);

    int nivel = 0;
    while (!colaVacia(&q) && nivel < 3) {
        int tamNivel = tamanoCola(&q);
        for (int i = 0; i < tamNivel; i++) {
            nodoVertice* actual = desencolar(&q);

            nodoArista* a = actual->adyacentes->primero;
            while (a != nullptr) {
                nodoVertice* vecino = a->destino;
                if (!vecino->visitado) {
                    vecino->visitado = true;
                    vecino->nivelBfs = nivel + 1;
                    encolar(&q, vecino);

                    if (nivel == 0) {
                        agregarContacto(&res.grado1, vecino);
                    } else if (nivel == 1) {
                        agregarContacto(&res.grado2, vecino);
                    } else if (nivel == 2) {
                        agregarContacto(&res.grado3, vecino);
                    }
                }
                a = a->sig;
            }
        }
        nivel++;
    }

    // Limpieza de seguridad de la cola si quedo algo
    while (!colaVacia(&q)) {
        desencolar(&q);
    }

    return res;
}

void mostrarGradosConexion(Grafo* g, const string& nombreRaiz) {
    string raiz = nombreRaiz;
    if (raiz.length() > 0 && raiz[0] != '@') {
        raiz = "@" + raiz;
    }

    nodoVertice* start = buscarVertice(g, raiz);
    if (start == nullptr) {
        cout << "[Error] El usuario '" << raiz << "' no existe en la red." << endl;
        return;
    }

    ResultadoBFS res = obtenerGradosConexion(g, raiz);

    cout << "\n=========================================" << endl;
    cout << "   GRADOS DE CONEXION PARA: " << raiz << endl;
    cout << "=========================================" << endl;

    // Mostrar Grado 1
    cout << "-> Contactos de 1° Grado (Amigos directos) [" << res.grado1.total << "]:" << endl;
    nodoContacto* c = res.grado1.primero;
    int mostrados = 0;
    while (c != nullptr) {
        if (mostrados < 15) {
            cout << "   - " << c->vertice->usuario->nombre << endl;
        }
        mostrados++;
        c = c->sig;
    }
    if (res.grado1.total > 15) cout << "   ... y " << (res.grado1.total - 15) << " mas." << endl;

    // Mostrar Grado 2
    cout << "\n-> Contactos de 2° Grado (Amigos de amigos) [" << res.grado2.total << "]:" << endl;
    c = res.grado2.primero;
    mostrados = 0;
    while (c != nullptr) {
        if (mostrados < 15) {
            cout << "   - " << c->vertice->usuario->nombre << endl;
        }
        mostrados++;
        c = c->sig;
    }
    if (res.grado2.total > 15) cout << "   ... y " << (res.grado2.total - 15) << " mas." << endl;

    // Mostrar Grado 3
    cout << "\n-> Contactos de 3° Grado (Amigos de 2° grado) [" << res.grado3.total << "]:" << endl;
    c = res.grado3.primero;
    mostrados = 0;
    while (c != nullptr) {
        if (mostrados < 15) {
            cout << "   - " << c->vertice->usuario->nombre << endl;
        }
        mostrados++;
        c = c->sig;
    }
    if (res.grado3.total > 15) cout << "   ... y " << (res.grado3.total - 15) << " mas." << endl;

    liberarListaContactos(&res.grado1);
    liberarListaContactos(&res.grado2);
    liberarListaContactos(&res.grado3);
}

// ===== Tabla Hash (Daniel J. Bernstein djb2) =====

unsigned long djb2Hash(const string& str) {
    unsigned long hash = 5381;
    for (int i = 0; i < (int)str.length(); i++) {
        hash = ((hash << 5) + hash) + str[i]; // hash * 33 + c
    }
    return hash;
}

bool esPrimo(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int obtenerSiguientePrimo(int minValor) {
    int p = minValor;
    if (p % 2 == 0) p++;
    while (!esPrimo(p)) {
        p += 2;
    }
    return p;
}

void inicializarTablaHash(TablaHash* tabla, int M) {
    tabla->M = M;
    tabla->N = 0;
    tabla->celdas = new nodoHash*[M];
    for (int i = 0; i < M; i++) {
        tabla->celdas[i] = nullptr;
    }
}

void insertarTerminoHash(TablaHash* tabla, const string& termino, int cantidad) {
    if (termino.length() == 0) return;
    unsigned long h = djb2Hash(termino);
    int idx = h % tabla->M;

    // Buscar si ya existe el termino en la cadena
    nodoHash* curr = tabla->celdas[idx];
    while (curr != nullptr) {
        if (curr->termino == termino) {
            curr->frecuencia += cantidad;
            return;
        }
        curr = curr->sig;
    }

    // Si no existe, crear un nuevo nodo e insertarlo al inicio de la lista
    nodoHash* nuevo = new nodoHash;
    nuevo->termino = termino;
    nuevo->frecuencia = cantidad;
    nuevo->sig = tabla->celdas[idx];
    tabla->celdas[idx] = nuevo;
    tabla->N++;
}

void construirTablaHash(TablaHash* tabla, ColeccionPosts* col, ListaStopwords* listaSW, int N_vocabulario) {
    // 1. Calcular el valor de M (tamano de tabla)
    // N / M <= 0.67 => M >= N / 0.67
    int minM = (int)(N_vocabulario / 0.67) + 1;
    int M = obtenerSiguientePrimo(minM);

    inicializarTablaHash(tabla, M);

    // 2. Procesar todos los posts
    nodoPost* p = col->primero;
    while (p != nullptr) {
        const string& texto = p->textoOriginal;
        string palabra = "";
        for (int i = 0; i <= (int)texto.length(); i++) {
            char c = (i < (int)texto.length()) ? texto[i] : ' ';
            if (c >= 'A' && c <= 'Z') c = c + 32;

            if (c >= 'a' && c <= 'z') {
                palabra += c;
            } else {
                if (palabra.length() > 0) {
                    if (!esStopword(palabra, listaSW)) {
                        insertarTerminoHash(tabla, palabra, 1);
                    }
                    palabra = "";
                }
            }
        }
        p = p->sig;
    }
}

void reportarMetricasHash(TablaHash* tabla) {
    int ocupadas = 0;
    int colisionesTotal = 0;
    int maxCadena = 0;
    long long sumaCadenas = 0;

    for (int i = 0; i < tabla->M; i++) {
        nodoHash* curr = tabla->celdas[i];
        int cont = 0;
        while (curr != nullptr) {
            cont++;
            curr = curr->sig;
        }
        if (cont > 0) {
            ocupadas++;
            sumaCadenas += cont;
            colisionesTotal += (cont - 1);
            if (cont > maxCadena) {
                maxCadena = cont;
            }
        }
    }

    double loadFactor = (double)tabla->N / tabla->M;
    double promCadenaOcupada = ocupadas > 0 ? (double)sumaCadenas / ocupadas : 0.0;
    double promCadenaTotal = (double)sumaCadenas / tabla->M;

    cout << "\n=========================================" << endl;
    cout << "   METRICAS DE LA TABLA HASH (djb2)"       << endl;
    cout << "=========================================" << endl;
    cout << "Vocabulario (N - terminos unicos): " << tabla->N << endl;
    cout << "Tamano de Tabla (M - primo):       " << tabla->M << endl;
    cout << "Factor de carga (N / M):          " << loadFactor << "  (Limite: 0.67)" << endl;
    cout << "Celdas ocupadas:                  " << ocupadas << " (" << (double)ocupadas*100/tabla->M << "%)" << endl;
    cout << "Total de colisiones (N - celdas): " << colisionesTotal << endl;
    cout << "Largo maximo de colisiones:       " << maxCadena << endl;
    cout << "Largo promedio (celdas ocupadas):  " << promCadenaOcupada << endl;
    cout << "Largo promedio (todas las celdas): " << promCadenaTotal << endl;
    cout << "=========================================" << endl;
}

void swapHash(nodoHash** a, nodoHash** b) {
    nodoHash* t = *a;
    *a = *b;
    *b = t;
}

bool esMayorFreq(nodoHash* a, nodoHash* b) {
    if (a->frecuencia == b->frecuencia) {
        return a->termino < b->termino; // Alfabético en caso de empate
    }
    return a->frecuencia > b->frecuencia; // Mayor frecuencia primero
}

int particionHash(nodoHash** arr, int bajo, int alto) {
    nodoHash* pivote = arr[alto];
    int i = (bajo - 1);
    for (int j = bajo; j <= alto - 1; j++) {
        if (esMayorFreq(arr[j], pivote)) {
            i++;
            swapHash(&arr[i], &arr[j]);
        }
    }
    swapHash(&arr[i + 1], &arr[alto]);
    return (i + 1);
}

void quickSortHash(nodoHash** arr, int bajo, int alto) {
    if (bajo < alto) {
        int pi = particionHash(arr, bajo, alto);
        quickSortHash(arr, bajo, pi - 1);
        quickSortHash(arr, pi + 1, alto);
    }
}

void consultarTopN(TablaHash* tabla, int n) {
    int totalTerminos = tabla->N;
    if (totalTerminos == 0) {
        cout << "\n--- TOP-" << n << " TERMINOS MAS FRECUENTES ---" << endl;
        cout << "No hay terminos en la tabla." << endl;
        return;
    }

    // Reservar arreglo dinámico temporal
    nodoHash** arr = new nodoHash*[totalTerminos];

    // Copiar punteros
    int idx = 0;
    for (int i = 0; i < tabla->M; i++) {
        nodoHash* curr = tabla->celdas[i];
        while (curr != nullptr) {
            arr[idx++] = curr;
            curr = curr->sig;
        }
    }

    // Ordenar con QuickSort propio
    quickSortHash(arr, 0, totalTerminos - 1);

    cout << "\n--- TOP-" << n << " TERMINOS MAS FRECUENTES ---" << endl;
    int limite = n < totalTerminos ? n : totalTerminos;
    for (int i = 0; i < limite; i++) {
        cout << "  " << (i + 1) << ". \"" << arr[i]->termino 
             << "\" (frecuencia: " << arr[i]->frecuencia << ")" << endl;
    }

    // Liberar memoria del arreglo temporal
    delete[] arr;
}

void liberarTablaHash(TablaHash* tabla) {
    if (tabla->celdas == nullptr) return;
    for (int i = 0; i < tabla->M; i++) {
        nodoHash* curr = tabla->celdas[i];
        while (curr != nullptr) {
            nodoHash* tmp = curr->sig;
            delete curr;
            curr = tmp;
        }
    }
    delete[] tabla->celdas;
    tabla->celdas = nullptr;
    tabla->M = 0;
    tabla->N = 0;
}


