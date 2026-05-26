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
