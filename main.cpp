#include <iostream>
#include <cstring>
using namespace std;

// Estructura para almacenar información del lote
struct LoteProduccion {
    int idLote;
    char nombreComponente[50];
    int cantidad;
    char fechaProduccion[11]; // formato: DD/MM/YYYY
};

// Variables globales para el sistema
LoteProduccion **almacen = nullptr;
int filas = 0, columnas = 0;

// Maestro de Lotes (vectores paralelos)
LoteProduccion *maestroLotes = nullptr;
bool *indicesDisponibles = nullptr;
int capacidadMaestro = 100;
int totalLotes = 0;

// Pila de Inspecciones
int *pilaIDLote = nullptr;
int *pilaResultado = nullptr;
int topPila = -1;
const int MAX_PILA = 10;

// Prototipos de funciones
void inicializarSistema();
void expandirMaestro();
void colocarLote();
void controlCalidad();
void deshacer();
void reportePorFila();
void buscarPorComponente();
void liberarMemoria();
void mostrarMenu();
int buscarIndiceDisponible();
void redimensionarAlmacen();

int main() {
    int opcion;
    
    cout << "=== Sistema de Gestion de Almacen ===" << endl;
    inicializarSistema();
    
    do {
        mostrarMenu();
        cout << "Seleccione opcion: ";
        cin >> opcion;
        cin.ignore();
        
        switch(opcion) {
            case 1:
                colocarLote();
                break;
            case 2:
                controlCalidad();
                break;
            case 3:
                deshacer();
                break;
            case 4:
                reportePorFila();
                break;
            case 5:
                buscarPorComponente();
                break;
            case 6:
                redimensionarAlmacen();
                break;
            case 0:
                cout << "Liberando memoria y finalizando..." << endl;
                liberarMemoria();
                break;
            default:
                cout << "Opcion no valida" << endl;
        }
    } while(opcion != 0);
    
    return 0;
}

void mostrarMenu() {
    cout << "\n--- MENU PRINCIPAL ---" << endl;
    cout << "1. Colocar Lote en Almacen" << endl;
    cout << "2. Control de Calidad (Registrar Inspeccion)" << endl;
    cout << "3. Deshacer (Pop de Pila)" << endl;
    cout << "4. Reporte por Fila" << endl;
    cout << "5. Buscar por Componente" << endl;
    cout << "6. Redimensionar Almacen" << endl;
    cout << "0. Salir" << endl;
}

void inicializarSistema() {
    cout << "Ingrese numero de filas del almacen: ";
    cin >> filas;
    cout << "Ingrese numero de columnas del almacen: ";
    cin >> columnas;
    
    // Asignar matriz 2D (almacen)
    almacen = new LoteProduccion*[filas];
    for(int i = 0; i < filas; i++) {
        almacen[i] = new LoteProduccion*[columnas];
        for(int j = 0; j < columnas; j++) {
            almacen[i][j] = nullptr;
        }
    }
    
    // Asignar vectores paralelos (maestro de lotes)
    maestroLotes = new LoteProduccion[capacidadMaestro];
    indicesDisponibles = new bool[capacidadMaestro];
    for(int i = 0; i < capacidadMaestro; i++) {
        indicesDisponibles[i] = true; // todos disponibles
    }
    
    // Asignar pila de inspecciones
    pilaIDLote = new int[MAX_PILA];
    pilaResultado = new int[MAX_PILA];
    
    cout << "Sistema inicializado correctamente." << endl;
    cout << "Almacen: " << filas << "x" << columnas << endl;
    cout << "Capacidad inicial maestro: " << capacidadMaestro << " lotes" << endl;
}

int buscarIndiceDisponible() {
    for(int i = 0; i < capacidadMaestro; i++) {
        if(indicesDisponibles[i]) {
            return i;
        }
    }
    return -1; // no hay espacio
}

void expandirMaestro() {
    cout << "Expandiendo capacidad del maestro de lotes..." << endl;
    
    int nuevaCapacidad = capacidadMaestro * 2;
    
    // Crear nuevos arreglos
    LoteProduccion *nuevoMaestro = new LoteProduccion[nuevaCapacidad];
    bool *nuevosIndices = new bool[nuevaCapacidad];
    
    // Copiar datos existentes
    for(int i = 0; i < capacidadMaestro; i++) {
        nuevoMaestro[i] = maestroLotes[i];
        nuevosIndices[i] = indicesDisponibles[i];
    }
    
    // Inicializar nuevos espacios
    for(int i = capacidadMaestro; i < nuevaCapacidad; i++) {
        nuevosIndices[i] = true;
    }
    
    // Liberar memoria anterior
    delete[] maestroLotes;
    delete[] indicesDisponibles;
    
    // Actualizar punteros
    maestroLotes = nuevoMaestro;
    indicesDisponibles = nuevosIndices;
    capacidadMaestro = nuevaCapacidad;
    
    cout << "Nueva capacidad: " << capacidadMaestro << " lotes" << endl;
}

void colocarLote() {
    int f, c;
    cout << "\n--- COLOCAR LOTE ---" << endl;
    cout << "Ingrese fila (0-" << filas-1 << "): ";
    cin >> f;
    cout << "Ingrese columna (0-" << columnas-1 << "): ";
    cin >> c;
    
    if(f < 0 || f >= filas || c < 0 || c >= columnas) {
        cout << "Posicion fuera de rango." << endl;
        return;
    }
    
    if(almacen[f][c] != nullptr) {
        cout << "La posicion ya esta ocupada por el lote ID: " 
             << almacen[f][c]->idLote << endl;
        return;
    }
    
    // Buscar indice disponible
    int indice = buscarIndiceDisponible();
    if(indice == -1) {
        expandirMaestro();
        indice = buscarIndiceDisponible();
    }
    
    // Solicitar datos del lote
    LoteProduccion nuevoLote;
    cout << "Ingrese ID del lote: ";
    cin >> nuevoLote.idLote;
    cin.ignore();
    cout << "Ingrese nombre del componente: ";
    cin.getline(nuevoLote.nombreComponente, 50);
    cout << "Ingrese cantidad: ";
    cin >> nuevoLote.cantidad;
    cin.ignore();
    cout << "Ingrese fecha de produccion (DD/MM/YYYY): ";
    cin.getline(nuevoLote.fechaProduccion, 11);
    
    // Guardar en maestro de lotes
    maestroLotes[indice] = nuevoLote;
    indicesDisponibles[indice] = false;
    totalLotes++;
    
    // Hacer que la celda del almacen apunte al maestro
    almacen[f][c] = &maestroLotes[indice];
    
    cout << "Lote colocado exitosamente en posicion [" << f << "][" << c << "]" << endl;
}

void controlCalidad() {
    int idBuscado, resultado;
    cout << "\n--- CONTROL DE CALIDAD ---" << endl;
    cout << "Ingrese ID del lote a inspeccionar: ";
    cin >> idBuscado;
    
    // Verificar si el lote existe
    bool encontrado = false;
    for(int i = 0; i < capacidadMaestro; i++) {
        if(!indicesDisponibles[i] && maestroLotes[i].idLote == idBuscado) {
            encontrado = true;
            break;
        }
    }
    
    if(!encontrado) {
        cout << "Lote no encontrado." << endl;
        return;
    }
    
    cout << "Ingrese resultado de inspeccion (1=Aprobado, 0=Rechazado): ";
    cin >> resultado;
    
    // Gestión de buffer rotatorio (máximo 10 elementos)
    if(topPila >= MAX_PILA - 1) {
        // Rotar elementos (eliminar el más antiguo)
        for(int i = 0; i < MAX_PILA - 1; i++) {
            pilaIDLote[i] = pilaIDLote[i + 1];
            pilaResultado[i] = pilaResultado[i + 1];
        }
        topPila = MAX_PILA - 2;
    }
    
    // Push a la pila
    topPila++;
    pilaIDLote[topPila] = idBuscado;
    pilaResultado[topPila] = resultado;
    
    cout << "Inspeccion registrada. Lote ID: " << idBuscado 
         << " - Resultado: " << (resultado ? "APROBADO" : "RECHAZADO") << endl;
}

void deshacer() {
    cout << "\n--- DESHACER INSPECCION ---" << endl;
    
    if(topPila < 0) {
        cout << "No hay inspecciones para deshacer." << endl;
        return;
    }
    
    // Pop de la pila
    int idRevertido = pilaIDLote[topPila];
    int resultadoRevertido = pilaResultado[topPila];
    topPila--;
    
    cout << "Se revirtio la inspeccion:" << endl;
    cout << "  Lote ID: " << idRevertido << endl;
    cout << "  Resultado: " << (resultadoRevertido ? "APROBADO" : "RECHAZADO") << endl;
}

void reportePorFila() {
    int fila;
    cout << "\n--- REPORTE POR FILA ---" << endl;
    cout << "Ingrese numero de fila (0-" << filas-1 << "): ";
    cin >> fila;
    
    if(fila < 0 || fila >= filas) {
        cout << "Fila fuera de rango." << endl;
        return;
    }
    
    cout << "\nLotes en la fila " << fila << ":" << endl;
    bool hayLotes = false;
    
    for(int j = 0; j < columnas; j++) {
        if(almacen[fila][j] != nullptr) {
            cout << "  Columna " << j << ": ID " << almacen[fila][j]->idLote 
                 << " - " << almacen[fila][j]->nombreComponente << endl;
            hayLotes = true;
        }
    }
    
    if(!hayLotes) {
        cout << "  No hay lotes en esta fila." << endl;
    }
}

void buscarPorComponente() {
    char componente[50];
    cout << "\n--- BUSCAR POR COMPONENTE ---" << endl;
    cout << "Ingrese nombre del componente: ";
    cin.ignore();
    cin.getline(componente, 50);
    
    bool encontrado = false;
    
    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            if(almacen[i][j] != nullptr) {
                if(strcmp(almacen[i][j]->nombreComponente, componente) == 0) {
                    cout << "Encontrado: Fila " << i << ", Columna " << j << endl;
                    cout << "  ID: " << almacen[i][j]->idLote << endl;
                    cout << "  Cantidad: " << almacen[i][j]->cantidad << endl;
                    cout << "  Fecha: " << almacen[i][j]->fechaProduccion << endl;
                    encontrado = true;
                }
            }
        }
    }
    
    if(!encontrado) {
        cout << "No se encontraron lotes con ese componente." << endl;
    }
}

void redimensionarAlmacen() {
    int nuevasFilas, nuevasColumnas;
    cout << "\n--- REDIMENSIONAR ALMACEN ---" << endl;
    cout << "Dimensiones actuales: " << filas << "x" << columnas << endl;
    cout << "Ingrese nuevas filas: ";
    cin >> nuevasFilas;
    cout << "Ingrese nuevas columnas: ";
    cin >> nuevasColumnas;
    
    // Crear nuevo almacén
    LoteProduccion ***nuevoAlmacen = new LoteProduccion**[nuevasFilas];
    for(int i = 0; i < nuevasFilas; i++) {
        nuevoAlmacen[i] = new LoteProduccion*[nuevasColumnas];
        for(int j = 0; j < nuevasColumnas; j++) {
            nuevoAlmacen[i][j] = nullptr;
        }
    }
    
    // Copiar datos existentes
    int minFilas = (filas < nuevasFilas) ? filas : nuevasFilas;
    int minCols = (columnas < nuevasColumnas) ? columnas : nuevasColumnas;
    
    for(int i = 0; i < minFilas; i++) {
        for(int j = 0; j < minCols; j++) {
            nuevoAlmacen[i][j] = almacen[i][j];
        }
    }
    
    // Liberar almacén anterior
    for(int i = 0; i < filas; i++) {
        delete[] almacen[i];
    }
    delete[] almacen;
    
    // Actualizar
    almacen = nuevoAlmacen;
    filas = nuevasFilas;
    columnas = nuevasColumnas;
    
    cout << "Almacen redimensionado a " << filas << "x" << columnas << endl;
}

void liberarMemoria() {
    // Liberar matriz 2D (almacen)
    if(almacen != nullptr) {
        for(int i = 0; i < filas; i++) {
            delete[] almacen[i];
        }
        delete[] almacen;
    }
    
    // Liberar vectores paralelos
    if(maestroLotes != nullptr) {
        delete[] maestroLotes;
    }
    if(indicesDisponibles != nullptr) {
        delete[] indicesDisponibles;
    }
    
    // Liberar pila
    if(pilaIDLote != nullptr) {
        delete[] pilaIDLote;
    }
    if(pilaResultado != nullptr) {
        delete[] pilaResultado;
    }
    
    cout << "Memoria liberada correctamente." << endl;
}