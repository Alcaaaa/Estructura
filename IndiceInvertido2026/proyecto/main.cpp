#include <iostream>
#include <string>
#include "estructura.h"
#include "funciones.h"

using namespace std;

// Funcion auxiliar para limpiar el estado de error y vaciar el bufer de entrada de cin
void limpiarCin() {
    cin.clear();
    int c;
    while ((c = cin.get()) != '\n' && c != EOF);
}

int main() {
    Diccionario       miBuscador;       // palabra -> posts
    IndiceUsuarios    miRedSocial;      // usuario -> amigos
    ColeccionPosts    miColeccion;      // posts unicos
    ListaStopwords    misStopwords;     // stopwords cargadas desde archivo
    Grafo             miGrafo;          // Grafo no dirigido de la red social
    TablaHash         miTabla;          // Tabla Hash para frecuencias de palabras

    inicializarDiccionario(&miBuscador);
    inicializarIndiceUsuarios(&miRedSocial);
    inicializarColeccion(&miColeccion);
    inicializarListaStopwords(&misStopwords);
    inicializarGrafo(&miGrafo);

    // Intento cargar la lista de stopwords por defecto
    cout << "Cargando stopwords por defecto..." << endl;
    cargarStopwordsDesdeArchivo(&misStopwords, "../EN-Stopwords.txt");

    cout << "Cargando dataset..." << endl;
    cargarDatosDesdeCSV(&miBuscador, &miColeccion, &misStopwords,
                        "../DataSET Procesado Final.csv");

    cout << "Construyendo red social desde @menciones..." << endl;
    construirRedDesdePosts(&miRedSocial, &miColeccion);

    cout << "Construyendo grafo no dirigido de la red..." << endl;
    construirGrafo(&miGrafo, &miRedSocial);
    validarSimetriaGrafo(&miGrafo);

    cout << "Construyendo tabla hash de frecuencias de terminos..." << endl;
    construirTablaHash(&miTabla, &miColeccion, &misStopwords, miBuscador.totalPalabras);

    int opcion = -1;
    while (opcion != 9) {
        cout << "\n=========================================" << endl;
        cout << "   RED SOCIAL - INDICE INVERTIDO"            << endl;
        cout << "=========================================" << endl;
        cout << "  --- STOPWORDS ---"                         << endl;
        cout << "0. Cargar stopwords desde archivo (re-indexa)" << endl;
        cout << "  --- BUSQUEDA DE POSTS ---"                 << endl;
        cout << "1. Buscar una palabra"                       << endl;
        cout << "2. Buscar varios terminos"                   << endl;
        cout << "  --- BUSQUEDA DE USUARIOS ---"              << endl;
        cout << "3. Ver amigos de un usuario"                 << endl;
        cout << "31. Ver contactos de 1, 2 y 3 grado (BFS)"   << endl;
        cout << "  --- INSERCION ---"                         << endl;
        cout << "4. Insertar nuevo post manualmente"          << endl;
        cout << "5. Agregar like (con identidad) a un post"   << endl;
        cout << "6. Generar likes sinteticos para un post"    << endl;
        cout << "  --- INSPECCION ---"                        << endl;
        cout << "7. Listar primeras N palabras / usuarios"    << endl;
        cout << "8. Estadisticas"                             << endl;
        cout << "81. Metricas de la Tabla Hash (djb2)"        << endl;
        cout << "82. Consultar Top-N terminos mas frecuentes" << endl;
        cout << "9. Salir (libera memoria)"                   << endl;
        cout << "Seleccione una opcion: ";
        if (!(cin >> opcion)) {
            cout << "[Error] Entrada invalida. Por favor, ingrese un numero." << endl;
            limpiarCin();
            opcion = -1; // Fuerza reintento del menu
            continue;
        }
        cin.ignore();

        if (opcion == 0) {
            string archivo;
            cout << "-> Nombre del archivo (ENTER usa '../EN-Stopwords.txt'): ";
            getline(cin, archivo);
            if (archivo.length() == 0) archivo = "../EN-Stopwords.txt";

            cargarStopwordsDesdeArchivo(&misStopwords, archivo);

            cout << "Re-indexando posts con la nueva lista de stopwords..." << endl;
            reindexarPosts(&miBuscador, &miColeccion, &misStopwords);

            // Reconstruir la tabla hash con el nuevo vocabulario
            liberarTablaHash(&miTabla);
            construirTablaHash(&miTabla, &miColeccion, &misStopwords, miBuscador.totalPalabras);
        }
        else if (opcion == 1) {
            string palabra;
            cout << "-> Palabra a buscar: ";
            getline(cin, palabra);
            buscarYMostrar(&miBuscador, palabra, &misStopwords);
        }
        else if (opcion == 2) {
            string consulta;
            cout << "-> Terminos (separados por espacio): ";
            getline(cin, consulta);
            buscarVariosTerminos(&miBuscador, consulta, &misStopwords);
        }
        else if (opcion == 3) {
            string usuario;
            cout << "-> Nombre de usuario (con o sin '@'): ";
            getline(cin, usuario);
            mostrarAmigosDe(&miRedSocial, usuario);
        }
        else if (opcion == 31) {
            string usuario;
            cout << "-> Nombre de usuario raiz (con o sin '@'): ";
            getline(cin, usuario);
            mostrarGradosConexion(&miGrafo, usuario);
        }
        else if (opcion == 4) {
            string id, autor, texto, likesStr;
            cout << "-> ID del post (ej: post_99999): ";
            getline(cin, id);
            cout << "-> Autor (con '@'): ";
            getline(cin, autor);
            cout << "-> Texto: ";
            getline(cin, texto);
            cout << "-> Cantidad inicial de likes: ";
            getline(cin, likesStr);

            int n = 0;
            for (int i = 0; i < (int)likesStr.length(); i++) {
                if (likesStr[i] >= '0' && likesStr[i] <= '9')
                    n = n * 10 + (likesStr[i] - '0');
            }

            if (buscarPostPorId(&miColeccion, id) != nullptr) {
                cout << "[Error] Ya existe un post con ese ID." << endl;
            } else {
                nodoPost* nuevo = crearPost(&miColeccion, id, autor, texto, n);
                indexarPost(&miBuscador, nuevo, &misStopwords);
                registrarUsuario(&miRedSocial, autor);
                extraerMencionesYConectar(&miRedSocial, autor, texto);
                
                // Reconstruir el grafo para incorporar nuevas conexiones si las hay
                liberarGrafo(&miGrafo);
                construirGrafo(&miGrafo, &miRedSocial);

                // Reconstruir la tabla hash para incorporar nuevos terminos
                liberarTablaHash(&miTabla);
                construirTablaHash(&miTabla, &miColeccion, &misStopwords, miBuscador.totalPalabras);
                
                cout << "[Exito] Post agregado, indexado, grafo y tabla hash actualizados." << endl;
            }
        }
        else if (opcion == 5) {
            string idPost, usuario;
            cout << "-> ID del post: ";
            getline(cin, idPost);
            cout << "-> Nombre del usuario que da like: ";
            getline(cin, usuario);
            nodoPost* p = buscarPostPorId(&miColeccion, idPost);
            if (p == nullptr) {
                cout << "[Error] No existe ese post." << endl;
            } else {
                agregarLikeAPost(p, usuario);
                cout << "[Exito] Like registrado. Total ahora: "
                     << p->misLikes->total << endl;
            }
        }
        else if (opcion == 6) {
            string idPost, cantStr;
            cout << "-> ID del post: ";
            getline(cin, idPost);
            cout << "-> Cuantos likes sinteticos generar: ";
            getline(cin, cantStr);

            int cant = 0;
            for (int i = 0; i < (int)cantStr.length(); i++) {
                if (cantStr[i] >= '0' && cantStr[i] <= '9')
                    cant = cant * 10 + (cantStr[i] - '0');
            }

            nodoPost* p = buscarPostPorId(&miColeccion, idPost);
            if (p == nullptr) {
                cout << "[Error] No existe ese post." << endl;
            } else {
                generarLikesSinteticos(p, &miRedSocial, cant);
            }
        }
        else if (opcion == 7) {
            int n;
            cout << "-> Cuantas mostrar: ";
            if (!(cin >> n)) {
                cout << "[Error] Entrada invalida. Debe ingresar un numero." << endl;
                limpiarCin();
                continue;
            }
            cin.ignore();
            listarPrimerasPalabras(&miBuscador, n);
            listarPrimerosUsuarios(&miRedSocial, n);
        }
        else if (opcion == 8) {
            mostrarEstadisticas(&miBuscador, &miColeccion, &miRedSocial);
            cout << "Stopwords cargadas en lista: " << misStopwords.total << endl;
        }
        else if (opcion == 81) {
            reportarMetricasHash(&miTabla);
        }
        else if (opcion == 82) {
            int n;
            cout << "-> Cantidad de terminos mas frecuentes (Top-N): ";
            if (!(cin >> n)) {
                cout << "[Error] Entrada invalida." << endl;
                limpiarCin();
                continue;
            }
            cin.ignore();
            consultarTopN(&miTabla, n);
        }
        else if (opcion == 9) {
            cout << "Liberando memoria dinamica..." << endl;
            liberarTablaHash(&miTabla);
            liberarGrafo(&miGrafo);
            liberarTodo(&miBuscador, &miColeccion, &miRedSocial, &misStopwords);
            cout << "Listo. Hasta pronto." << endl;
        }
        else {
            cout << "[Aviso] Opcion no valida." << endl;
        }
    }

    return 0;
}
