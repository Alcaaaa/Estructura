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

// ===== Estructuras para Grafo No Dirigido (Entrega II) =====
struct nodoVertice; // Declaracion adelantada

struct nodoArista {
    nodoVertice* destino;
    nodoArista* sig;
};

struct ListaAristas {
    nodoArista* primero;
    nodoArista* ultimo;
    int total;
};

struct nodoVertice {
    nodoUsuario* usuario; // Referencia al usuario original
    ListaAristas* adyacentes; // Lista de adyacencia (aristas)
    bool visitado;            // Marcado de visitado para BFS propio
    int nivelBfs;             // Nivel de profundidad en el recorrido BFS
    nodoVertice* sig;
};

struct Grafo {
    nodoVertice* primero;
    nodoVertice* ultimo;
    int totalVertices;
};

// Estructuras de retorno para el recorrido BFS por niveles
struct nodoContacto {
    nodoVertice* vertice;
    nodoContacto* sig;
};

struct ListaContactos {
    nodoContacto* primero;
    nodoContacto* ultimo;
    int total;
};

struct ResultadoBFS {
    ListaContactos grado1;
    ListaContactos grado2;
    ListaContactos grado3;
};

// ===== Estructuras para Tabla Hash (Entrega III) =====
struct nodoHash {
    string termino;
    int frecuencia;
    nodoHash* sig;
};

struct TablaHash {
    nodoHash** celdas;
    int M; // Tamano de la tabla
    int N; // Cantidad de palabras unicas
};

#endif


