#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include <string>
using namespace std;

// Nodo de un like: guarda quien lo dio
struct nodoLike {
    string nombreUsuario;
    nodoLike* sig;
};

// first, last, curr, num_elem  (patron lista enlazada simple)
struct ListaLikes {
    nodoLike* primero;
    nodoLike* ultimo;
    nodoLike* curr;
    int total;
};

// Post: vive una sola vez en ColeccionPosts
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
    nodoPost* curr;
    int total;
};

// Referencia a un post (puntero, no copia)
struct nodoPostRef {
    nodoPost* post;
    nodoPostRef* sig;
};

struct ListaPostRef {
    nodoPostRef* primero;
    nodoPostRef* ultimo;
    nodoPostRef* curr;
    int numElem;
};

// Indice invertido: palabra -> lista de posts
struct nodoVocabulario {
    string palabra;
    ListaPostRef* listaPost;
    nodoVocabulario* sig;
};

struct Diccionario {
    nodoVocabulario* primero;
    nodoVocabulario* ultimo;
    nodoVocabulario* curr;
    int totalPalabras;
};

// Indice invertido: usuario -> lista de amigos
struct nodoAmigo {
    string nombreAmigo;
    nodoAmigo* sig;
};

struct ListaAmigos {
    nodoAmigo* primero;
    nodoAmigo* ultimo;
    nodoAmigo* curr;
    int total;
};

struct nodoUsuario {
    string nombre;
    ListaAmigos* amigos;
    nodoUsuario* sig;
};

struct IndiceUsuarios {
    nodoUsuario* primero;
    nodoUsuario* ultimo;
    nodoUsuario* curr;
    int total;
};

// Lista de stopwords cargable desde archivo
struct nodoStopword {
    string palabra;
    nodoStopword* sig;
};

struct ListaStopwords {
    nodoStopword* primero;
    nodoStopword* ultimo;
    nodoStopword* curr;
    int total;
};

#endif
