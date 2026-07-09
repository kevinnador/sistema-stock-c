#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROD 1000 // Capacidad máxima definida como constante

// Estructura de Producto
typedef struct {
    int codigo;
    char nombre[50];
    char categoria[50];
    float precio_unitario;
    int stock_actual;
    int stock_minimo;
} Producto;

// Estructura para la Lista Dinámica de Movimientos
typedef struct NodoMovimiento {
    int codigo;
    char tipo; // 'E' para Entrada, 'V' para Venta
    int cantidad;
    struct NodoMovimiento* siguiente;
} NodoMovimiento;

// --- PROTOTIPOS ---
void cargarArchivo(Producto arr[], int *cant);
void guardarArchivo(Producto arr[], int cant);
int buscarPorCodigo(Producto arr[], int cant, int codigo);
void altaProducto(Producto arr[], int *cant);
void bajaProducto(Producto arr[], int *cant);
void modificarProducto(Producto arr[], int cant);
void consultarProducto(Producto arr[], int cant);
void registrarEntrada(Producto arr[], int cant, NodoMovimiento **lista);
void registrarVenta(Producto arr[], int cant, NodoMovimiento **lista);
void reporteStockBajo(Producto arr[], int cant);
void reporteCompleto(Producto arr[], int cant);
void registrarMovimiento(NodoMovimiento **lista, int codigo, char tipo, int cantidad);
void listarMovimientos(NodoMovimiento *lista);
void liberarMovimientos(NodoMovimiento *lista);
int leerEnteroSeguro(char mensaje[]);
float leerFlotanteSeguro(char mensaje[]);
void leerTextoSeguro(char mensaje [], char destino [], int tam_max);

// --- MAIN ---
int main() {
    Producto inventario[MAX_PROD];
    int cant_productos = 0;
    NodoMovimiento *lista_mov = NULL;
    int opcion;

    cargarArchivo(inventario, &cant_productos); // Persistencia al iniciar

    do {
        printf("\n--- GESTION DE STOCK ---\n");
        printf("1. Alta de producto\n2. Baja de producto\n3. Modificacion\n4. Consulta\n");
        printf("5. Registrar Entrada\n6. Registrar Venta\n7. Reporte Stock Bajo\n8. Reporte Completo\n");
        printf("9. Ultimos Movimientos\n0. Salir\nOpcion: ");
        if (scanf("%d", &opcion) != 1){
            printf("Error: Entrada invalida,Por favor Ingrese un numero: \n");
            while (getchar() != '\n');    
            opcion = -1;
        }

        switch(opcion) {
            case 1: altaProducto(inventario, &cant_productos); break;
            case 2: bajaProducto(inventario, &cant_productos); break;
            case 3: modificarProducto(inventario, cant_productos); break;
            case 4: consultarProducto(inventario, cant_productos); break;
            case 5: registrarEntrada(inventario, cant_productos, &lista_mov); break;
            case 6: registrarVenta(inventario, cant_productos, &lista_mov); break;
            case 7: reporteStockBajo(inventario, cant_productos); break;
            case 8: reporteCompleto(inventario, cant_productos); break;
            case 9: listarMovimientos(lista_mov); break;
            case 0: 
            guardarArchivo(inventario, cant_productos);
            liberarMovimientos(lista_mov);
            break; // Persistencia al salir
            default: printf("Opcion invalida.\n");
        }
    } while(opcion != 0);

    return 0;
}

// --- FUNCIONES DE ARCHIVO ---
void cargarArchivo(Producto arr[], int *cant) {
    FILE *arch = fopen("productos.dat", "rb");
    if (arch != NULL) {
        *cant = fread(arr, sizeof(Producto), MAX_PROD, arch);
        fclose(arch);
    }
}

void guardarArchivo(Producto arr[], int cant) {
    FILE *arch = fopen("productos.dat", "wb");
    if (arch != NULL) {
        fwrite(arr, sizeof(Producto), cant, arch);
        fclose(arch);
    }
}

// --- FUNCIONES DE BÚSQUEDA ---
int buscarPorCodigo(Producto arr[], int cant, int codigo) {
    for (int i = 0; i < cant; i++) {
        if (arr[i].codigo == codigo) return i;
    }
    return -1;
}

// --- ALTA DE PRODUCTO ---
void altaProducto(Producto arr[], int *cant) {
    if (*cant >= MAX_PROD) {
        printf("Error: Inventario lleno.\n");
        return;
    }
    
    int cod = leerEnteroSeguro("Codigo: ");
    
    if (buscarPorCodigo(arr, *cant, cod) != -1) {
        printf("Error: El codigo ya existe.\n");
        return;
    }
    
    arr[*cant].codigo = cod;
    while (getchar() != '\n');
    leerTextoSeguro("Nombre: ",arr[*cant].nombre, 50);
    leerTextoSeguro("Categoria: ",arr[*cant].categoria, 50);
    arr[*cant].precio_unitario = leerFlotanteSeguro("Precio Unitario: ");
    arr[*cant].stock_actual = leerEnteroSeguro("Stock Actual: ");
    arr[*cant].stock_minimo = leerEnteroSeguro("Stock Minimo: ");
    (*cant)++;
    printf("Producto agregado.\n");
}
// --BAJA DE PRODUCTO--
void bajaProducto(Producto arr[], int *cant) {
    int cod, pos;
    cod = leerEnteroSeguro("Codigo a dar de baja: ");
    pos = buscarPorCodigo(arr, *cant, cod);
    
    if (pos != -1) {
        arr[pos] = arr[*cant - 1]; // Reemplazo con el último para no iterar al vicio
        (*cant)--;
        printf("Producto eliminado.\n");
    } else {
        printf("Producto no encontrado.\n");
    }
}
// -- MODIFICAR PRODUCTO --
void modificarProducto(Producto arr[], int cant) {
    int cod, pos, op;
    cod = leerEnteroSeguro("Codigo a modificar: ");
    pos = buscarPorCodigo(arr, cant, cod);
    
    if (pos != -1) {
        op = leerEnteroSeguro("1. Precio | 2. Stock Minimo\n Opcion: ");
        if (op == 1) {
            arr[pos].precio_unitario = leerFlotanteSeguro("Nuevo Precio: ");
            printf("Modificacion Guardada. \n");
        } else if (op == 2) {
            arr[pos].stock_minimo = leerEnteroSeguro("Nuevo stock minimo: ");
            printf("Modificacion Guardada.\n ");
        }
    } else {
        printf("Opcion invalida. No se realizaron cambios. \n");
    }
}
// -- CONSULTAR PRODUCTO --
void consultarProducto(Producto arr[], int cant){
    int op;
    op = leerEnteroSeguro("1. Por Codigo | 2. Por Nombre\nOpcion: ");
    
    if (op == 1) {
        int cod, pos;
        cod = leerEnteroSeguro("Codigo: ");
        pos = buscarPorCodigo(arr, cant, cod);
        if (pos != -1) {
            printf("[%d] %s | Stock: %d | Precio: $%.2f\n", arr[pos].codigo, arr[pos].nombre, arr[pos].stock_actual, arr[pos].precio_unitario);
        } else {
            printf("No encontrado.\n");
        }
    } else if (op == 2) {
        char nom[50];
        int encontrado_exacto = 0;
        int encontrados_similares = 0;
        leerTextoSeguro("Nombre a buscar: ", nom, 50);
        for (int i = 0; i < cant; i++) {
            if (strcmp(arr[i].nombre, nom) == 0) {
                if (!encontrado_exacto){
                printf("[%d] %s | Stock: %d | Precio: $%.2f\n", arr[i].codigo, arr[i].nombre, arr[i].stock_actual, arr[i].precio_unitario);
                encontrado_exacto = 1;
            }
        }
        printf("\n---PRODUCTOS SIMILARES---\n");
        for (int i = 0; i < cant; i++){
            if(strstr(arr[i].nombre, nom) != NULL){
                if(strcmp(arr[i].nombre, nom) != 0){
                    printf("[%d] %s | CATEGORIA: %s | STOCK: %d | PRECIO: $%.2f\n",arr[1].codigo, arr[i].nombre, arr[i].categoria, arr[i].stock_actual, arr[1].precio_unitario);
                    encontrados_similares++;
                }
            }
        }
        if(!encontrado_exacto && encontrados_similares == 0){
            printf("No se encontraron productos que coincidan con '%s'.\n",nom);
        }else if (!encontrado_exacto){
            printf("\n[!] No se encontro el producto exacto, pero te mostramos %d sugerencias arriba.\n",encontrados_similares);
        }
    }
  }
}

// --- MOVIMIENTOS Y LISTA DINÁMICA ---
void registrarMovimiento(NodoMovimiento **lista, int codigo, char tipo, int cantidad) {
    NodoMovimiento *nuevo = (NodoMovimiento*)malloc(sizeof(NodoMovimiento));
    nuevo->codigo = codigo;
    nuevo->tipo = tipo;
    nuevo->cantidad = cantidad;
    nuevo->siguiente = *lista; // Se inserta al inicio. El primer nodo siempre es el más reciente.
    *lista = nuevo;
}

void registrarEntrada(Producto arr[], int cant, NodoMovimiento **lista) {
    int cod, pos, agregados;
    cod = leerEnteroSeguro("Codigo del producto: ");
    pos = buscarPorCodigo(arr, cant, cod);
    
    if (pos != -1) {
        agregados = leerEnteroSeguro("Cantidad a ingresar: ");
        arr[pos].stock_actual += agregados;
        registrarMovimiento(lista, cod, 'E', agregados);
        printf("Entrada registrada.\n");
    } else {
        printf("Producto no encontrado.\n");
    }
}

void registrarVenta(Producto arr[], int cant, NodoMovimiento **lista) {
    int cod, pos, vendidos;
    cod = leerEnteroSeguro("Codigo del producto: ");
    pos = buscarPorCodigo(arr, cant, cod);
    
    if (pos != -1) {
        vendidos = leerEnteroSeguro("Cantidad a vender: ");
        if (arr[pos].stock_actual >= vendidos) { // Validacion estricta de stock
            arr[pos].stock_actual -= vendidos;
            registrarMovimiento(lista, cod, 'V', vendidos);
            printf("Venta registrada.\n");
        } else {
            if (arr[pos].stock_actual > 0){
                printf("\n Stock insuficiente para cubrir el pedido(%d solicitados).\n", vendidos);
                printf("Solo quedan %d unidades disponibles de [%s]. \n", arr[pos].stock_actual, arr[pos].nombre);
                int op = leerEnteroSeguro("\nQue desea hacer?\n1. Vender las unidades remanentes disponibles\n2. Cancelar la venta\nOpcion: ");
                if (op == 1){
                    int cantidad_vendida = arr[pos].stock_actual;
                    arr[pos].stock_actual = 0;
                    registrarMovimiento(lista, cod, 'V', cantidad_vendida);
                    printf("Venta registrada por el remanente de %d unidades. El producto quedo en 0.\n", cantidad_vendida);
                } else {
                    printf("Venta cancelada por el usuario.\n");
                }
            } else {
                printf("Error absoluto: El producto [%s] no tiene stock disponible (0 unidades).\n", arr[pos].nombre);
            }
        }
    } else {
        printf("Producto no encontrado.\n");
    }
}

void listarMovimientos(NodoMovimiento *lista) {
    int n, i = 0;
    n = leerEnteroSeguro("Cantidad de ultimos movimientos a ver: ");
    NodoMovimiento *actual = lista;
    while (actual != NULL && i < n) {
        printf("Producto: %d | Tipo: %c | Cant: %d\n", actual->codigo, actual->tipo, actual->cantidad);
        actual = actual->siguiente;
        i++;
    }
}

// --- REPORTES ---
void reporteStockBajo(Producto arr[], int cant) {
    printf("\n--- PRODUCTOS CON STOCK BAJO ---\n");
    for (int i = 0; i < cant; i++) {
        if (arr[i].stock_actual <= arr[i].stock_minimo) {
            printf("Cod: %d | %s | Stock: %d (Min: %d)\n", arr[i].codigo, arr[i].nombre, arr[i].stock_actual, arr[i].stock_minimo);
        }
    }
}

void reporteCompleto(Producto arr[], int cant) {
    Producto temp[MAX_PROD];
    for (int i = 0; i < cant; i++) temp[i] = arr[i];
    
    for (int i = 0; i < cant - 1; i++) {
        for (int j = 0; j < cant - i - 1; j++) {
            if (strcmp(temp[j].categoria, temp[j+1].categoria) > 0) {
                Producto aux = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = aux;
            }
        }
    }
    
    printf("\n--- REPORTE COMPLETO (Por Categoria) ---\n");
    for (int i = 0; i < cant; i++) {
        printf("[%s] Cod: %d | %s | Stock: %d\n", temp[i].categoria, temp[i].codigo, temp[i].nombre, temp[i].stock_actual);
    }
}
// --
void liberarMovimientos(NodoMovimiento *lista){
    NodoMovimiento *actual = lista;
    NodoMovimiento *siguiente_nodo;
    while (actual != NULL){
        siguiente_nodo = actual->siguiente;
        free(actual);
        actual = siguiente_nodo;
    }
    printf("Memoria dinamica de movimientos liberada correctamente. \n");
}
// --
int leerEnteroSeguro(char mensaje[]){
    int numero;
    while(1){
        printf("%s",mensaje);
        if (scanf("%d", &numero) == 1){
            if (numero >= 0){
                return numero;
            } else {
               printf("Error: El numero no puede ser negatico. \n");
            }
        } else {
            printf("Error: Entrada invalida.Debe ingresar un numero entero.\n");
            while(getchar() != '\n');
        }
    }
}
// --
float leerFlotanteSeguro(char mensaje[]){
    float numero;
    while(1){
        printf("%s", mensaje);
        if(scanf("%f", &numero) ==1 ){
            if (numero >= 0.0){
                return numero;
            }else {
                printf("Error: El precio no puede ser negativo. \n");
            }
        }else{
            printf("Error: Entrada invalida. Debe ingresar un numero decimal. \n");
            while (getchar() != '\n');
        }
    }
}
// --
void leerTextoSeguro(char mensaje[], char destino [], int tam_max){
    int es_valido;
    int tiene_letras;
    char temp[200];
    while (1) {
        printf("%s", mensaje);
        char formato [20];
        sprintf(formato, "%%%d[^\n]", tam_max -1);
        if(scanf(formato, temp) == 1){
            es_valido = 1;
            tiene_letras = 0;
            for(int i = 0; temp[i] != '\0' ; i++){
                char c = temp[i];
                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (unsigned char)c == 164 || (unsigned char)c == 165){
                    tiene_letras = 1;
                }
                if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ' || c == '\'' || (unsigned char)c == 164 || (unsigned char)c == 165)){
                    es_valido = 0;
                    break;
                }
            }
            while (getchar() != '\n');
            if (es_valido && strlen(temp) > 0 && tiene_letras == 1){
                int i = 0;
                int j = 0;
                while(temp[i]== ' '){
                    i++;
                }
                int ultimo_fue_espacio = 0;
                while(temp[i] != '\0'){
                    if(temp[i] != ' '){
                        destino[j++] = temp[i];
                        ultimo_fue_espacio = 0;
                    }else if (!ultimo_fue_espacio){
                        destino[j++] = ' ';
                        ultimo_fue_espacio = 1;
                    }
                    i++;
                }
                if (j > 0 && destino[j - 1] == ' '){
                    j--;
                }
                destino[j] = '\0';
                if(strlen(destino) > 0){
                return;
                }
            } 
                printf("Error: entrada invalida, Debe comenzar con letras y no puede ser solo espacios.\n");
        }else{
            printf("Error: entrada invalida.\n");
            while(getchar() != '\n');
        }
    }
}