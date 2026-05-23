#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include <string>
using namespace std;

// =====================================================
// LIKES
// Cada post tiene su lista enlazada propia de likes.
// Guardamos el nombre del usuario que dio el like
// (tal como pide la rubrica: "identidad de los usuarios").
// =====================================================
struct nodoLike {
    string nombreUsuario;
    nodoLike* sig;
};

struct ListaLikes {
    nodoLike* primero;
    nodoLike* ultimo;
    int total;          // total de likes (puede ser mayor que la cantidad de nodos si los likes son anonimos)
};

// =====================================================
// POST
// Vive UNA SOLA VEZ dentro de la ColeccionPosts maestra.
// =====================================================
struct nodoPost {
    string idPost;
    string autor;
    string textoOriginal;
    ListaLikes* misLikes;
    nodoPost* sig;
};

struct ColeccionPosts {
    nodoPost* primero;
    nodoPost* ultimo;
    int total;
};

// =====================================================
// REFERENCIA A POST (los "pasillos" del Edificio)
// El indice invertido NO duplica posts: guarda punteros.
// =====================================================
struct nodoPostRef {
    nodoPost* post;     // puntero al post real (vive en ColeccionPosts)
    nodoPostRef* sig;
};

struct ListaPostRef {
    nodoPostRef* primero;
    nodoPostRef* ultimo;
    int numElem;
};

// =====================================================
// VOCABULARIO (el "Ascensor" del Edificio)
// Cada piso = una palabra unica + su pasillo de posts
// =====================================================
struct nodoVocabulario {
    string palabra;
    ListaPostRef* listaPost;
    nodoVocabulario* sig;
};

struct Diccionario {
    nodoVocabulario* primero;
    nodoVocabulario* ultimo;
    int totalPalabras;
};

// =====================================================
// USUARIOS y AMIGOS  (segundo Indice Invertido)
//
// Clave  = nombre de usuario (ej. "@laurenboebert")
// Valor  = lista enlazada de sus amigos/contactos
//
// Construccion: a partir de las @menciones dentro de
// cada post. Si el autor A escribe un tweet mencionando
// a @B, registramos amistad mutua A <-> B.
// =====================================================
struct nodoAmigo {
    string nombreAmigo;     // nombre del amigo (ej. "@Climatemorgan")
    nodoAmigo* sig;
};

struct ListaAmigos {
    nodoAmigo* primero;
    nodoAmigo* ultimo;
    int total;
};

struct nodoUsuario {
    string nombre;           // ej. "@laurenboebert"
    ListaAmigos* amigos;
    nodoUsuario* sig;
};

struct IndiceUsuarios {
    nodoUsuario* primero;
    nodoUsuario* ultimo;
    int total;
};

// =====================================================
// LISTA DE STOPWORDS (cargable desde archivo)
// Lista enlazada simple: nodo con la palabra + siguiente.
// Se compara linealmente al filtrar (TDA basico).
// =====================================================
struct nodoStopword {
    string palabra;
    nodoStopword* sig;
};

struct ListaStopwords {
    nodoStopword* primero;
    nodoStopword* ultimo;
    int total;
};

#endif
