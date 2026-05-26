#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "estructura.h"
#include <string>
using namespace std;

// ===== Inicializadores =====
void inicializarDiccionario(Diccionario* dic);
void inicializarColeccion(ColeccionPosts* col);
void inicializarListaPostRef(ListaPostRef* lista);
void inicializarListaLikes(ListaLikes* l);
void inicializarListaStopwords(ListaStopwords* l);

// ===== Filtrado / stopwords =====
bool estaEnListaStopwords(ListaStopwords* lista, const string& palabra);
bool esStopword(const string& palabra, ListaStopwords* lista);
string normalizarPalabra(const string& palabra);
void agregarStopword(ListaStopwords* lista, const string& palabra);
void cargarStopwordsDesdeArchivo(ListaStopwords* lista, const string& nombreArchivo);
void liberarListaStopwords(ListaStopwords* lista);

// ===== Construccion del indice =====
nodoPost* crearPost(ColeccionPosts* col, const string& idPost, const string& autor,
                    const string& texto, int cantidadLikes);
nodoVocabulario* buscarPalabra(Diccionario* dic, const string& palabra);
bool existeRefEnLista(ListaPostRef* lista, nodoPost* postBuscado);
void agregarPalabraAlIndice(Diccionario* dic, const string& palabra, nodoPost* post);
void indexarPost(Diccionario* dic, nodoPost* post, ListaStopwords* lista);
void liberarDiccionario(Diccionario* dic);
void reindexarPosts(Diccionario* dic, ColeccionPosts* col, ListaStopwords* lista);

// ===== Carga de dataset =====
void cargarDatosDesdeCSV(Diccionario* dic, ColeccionPosts* col,
                          ListaStopwords* lista, const string& nombreArchivo);

// ===== Busqueda =====
void buscarYMostrar(Diccionario* dic, const string& palabra, ListaStopwords* lista);
void buscarVariosTerminos(Diccionario* dic, const string& consulta, ListaStopwords* lista);

// ===== Likes (insertar identidad de quien dio el like) =====
void agregarLikeAPost(nodoPost* post, const string& nombreUsuario);
nodoPost* buscarPostPorId(ColeccionPosts* col, const string& idPost);

// ===== Estadisticas y debug =====
void mostrarEstadisticas(Diccionario* dic, ColeccionPosts* col, IndiceUsuarios* idxU);
void listarPrimerasPalabras(Diccionario* dic, int cuantas);
void listarPrimerosUsuarios(IndiceUsuarios* idxU, int cuantos);

// ===== Indice Invertido de USUARIOS y AMIGOS =====
void inicializarIndiceUsuarios(IndiceUsuarios* idx);
void inicializarListaAmigos(ListaAmigos* l);

nodoUsuario* buscarUsuario(IndiceUsuarios* idx, const string& nombre);
nodoUsuario* registrarUsuario(IndiceUsuarios* idx, const string& nombre);
bool existeAmigo(ListaAmigos* lista, const string& nombre);
void agregarAmistadMutua(IndiceUsuarios* idx, const string& a, const string& b);
void construirRedDesdePosts(IndiceUsuarios* idx, ColeccionPosts* col);
void extraerMencionesYConectar(IndiceUsuarios* idx, const string& autor, const string& texto);
void mostrarAmigosDe(IndiceUsuarios* idx, const string& nombre);

// ===== Likes sinteticos (demo) =====
void generarLikesSinteticos(nodoPost* post, IndiceUsuarios* idx, int cantidad);

// ===== Liberacion de memoria =====
void liberarTodo(Diccionario* dic, ColeccionPosts* col,
                  IndiceUsuarios* idxU, ListaStopwords* listaSW);

#endif
