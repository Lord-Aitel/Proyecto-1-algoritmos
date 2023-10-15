#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#define MAX_EVENTOS 100

typedef struct {
    char evento[100];  // Texto del evento
} RegistroEvento;

typedef struct {
    char nombreCarta[40];
    char tipo[20];
    int ataque;
    int defensa;
    int vida;
} Carta;

typedef struct {
    Carta* cartas[15];  
    int numCartas;      
} Mazo;

typedef struct {
    char nombreJugador[30];
    Carta* mano[3];
    Carta* mazo[12];
    int puntosVida;
} Jugador;

void llenarMazoInicial(Mazo* mazo, Carta* cartas) {
    mazo->numCartas = 15;
    int i;
    for (i = 0; i < 15; i++) {
        mazo->cartas[i] = &cartas[i];
    }
}

void barajarCartas(Carta* cartas, int numCartas) {
    int i, j;
    for (i = numCartas - 1; i > 0; i--) {
        j = rand() % (i + 1);

        // Intercambia las cartas en las posiciones i y j
        Carta temp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }
}

void elegirCartasParaMano(Mazo* mazo, Jugador* jugador) {
    int i;
    printf("Estas son tus cartas iniciales:\n");
    for (i = 0; i < mazo->numCartas; i++) {
        printf("%d. Nombre: %s, Tipo: %s, Ataque: %d, Defensa: %d, Vida: %d\n",
               i + 1,
               mazo->cartas[i]->nombreCarta,
               mazo->cartas[i]->tipo,
               mazo->cartas[i]->ataque,
               mazo->cartas[i]->defensa,
               mazo->cartas[i]->vida);
    }

    printf("Elige 3 cartas para tu mano (números separados por espacios): ");
    int seleccion[3];
    for (i = 0; i < 3; i++) {
        scanf("%d", &seleccion[i]);
        if (seleccion[i] < 1 || seleccion[i] > mazo->numCartas) {
            printf("Selección no válida. Ingresa un número válido.\n");
            i--;
        }
    }

    // Limpia la mano del jugador
    for (i = 0; i < 3; i++) {
        jugador->mano[i] = NULL;
    }

    // Mover las cartas seleccionadas a la mano del jugador y al mazo del jugador
    for (i = 0; i < 3; i++) {
        int cartaSeleccionada = seleccion[i] - 1;
        jugador->mano[i] = mazo->cartas[cartaSeleccionada];
        mazo->cartas[cartaSeleccionada] = NULL;
    }
    mazo->numCartas -= 3;

    // Copia las cartas restantes en el mazo del jugador para futuros turnos
    for (i = 0; i < mazo->numCartas; i++) {
        jugador->mazo[i] = mazo->cartas[i];
    }
}

void repartirCartas(Carta* cartas, Jugador* jugador, Jugador* oponente, int n) {
    int i;
    if (n < 15) {
        printf("No hay suficientes cartas para repartir.\n");
        return;
    }

    // Repartir 3 cartas a la mano del jugador y 3 cartas al oponente
    for (i = 0; i < 3; i++) {
        jugador->mano[i] = &cartas[i];
        oponente->mano[i] = &cartas[i + 3];
    }

    // Colocar las cartas restantes en el mazo del jugador y el oponente
    for (i = 6; i < n; i++) {
        jugador->mazo[i - 6] = &cartas[i];
        oponente->mazo[i - 6] = &cartas[i + 3];
    }

    jugador->puntosVida = 5;
    oponente->puntosVida = 5;
}

void atacarComputadora(Jugador* computadora, Jugador* jugador) {
    int cartasEnMano = 0;
    int i;

    for (i = 0; i < 3; i++) {
        if (computadora->mano[i]->nombreCarta[0] != '\0') {
            cartasEnMano++;
        }
    }

    if (cartasEnMano == 0) {
        printf("La mano de la computadora está vacía. No puede atacar.\n");
        return;
    }

    int cartaAtacante = rand() % 3;

    while (computadora->mano[cartaAtacante]->nombreCarta[0] == '\0') {
        cartaAtacante = rand() % 3;
    }

    int cartaDefensora = rand() % 3;

    Carta* atacanteCarta = computadora->mano[cartaAtacante];
    Carta* defensoraCarta = jugador->mano[cartaDefensora];

    int danoNeto = atacanteCarta->ataque - defensoraCarta->defensa;
    
    // Asegúrate de que el daño nunca sea negativo
    if (danoNeto < 0) {
        danoNeto = 0;
    }

    defensoraCarta->vida -= danoNeto;

    if (defensoraCarta->vida <= 0) {
        printf("La carta del jugador ha muerto!\n");
        defensoraCarta->vida = 0;
        jugador->mano[cartaDefensora]->nombreCarta[0] = '\0'; 
        jugador->puntosVida--;
    }

    printf("La computadora ha atacado con %s y causó %d puntos de daño.\n", atacanteCarta->nombreCarta, danoNeto);
    printf("Puntos de vida del jugador: %d\n", jugador->puntosVida);
}

void atacarOponente(Jugador *jugador, Jugador *oponente) {
    // Primero se muestra la mano del oponente para atacar
    mostrarCartasOponente(oponente);

    // Selecciona una carta del jugador para atacar
    int cartaAtacante;
    do {
        printf("Selecciona una carta de tu mano para atacar (1-3): ");
        scanf("%d", &cartaAtacante);
        if (cartaAtacante < 1 || cartaAtacante > 3 || jugador->mano[cartaAtacante - 1]->nombreCarta[0] == '\0') {
            printf("Opción no válida. Por favor, selecciona una carta válida.\n");
        }
    } while (cartaAtacante < 1 || cartaAtacante > 3 || jugador->mano[cartaAtacante - 1]->nombreCarta[0] == '\0');

    int cartaOponente;
    
    do {
        printf("Selecciona una carta del oponente para atacar (1-3): ");
        scanf("%d", &cartaOponente);
        if (cartaOponente < 1 || cartaOponente > 3 || oponente->mano[cartaOponente - 1]->nombreCarta[0] == '\0') {
            printf("Opción no válida. Por favor, selecciona una carta válida del oponente.\n");
        }
    } while (cartaOponente < 1 || cartaOponente > 3 || oponente->mano[cartaOponente - 1]->nombreCarta[0] == '\0');

    // Realiza el ataque
    Carta* atacanteCarta = jugador->mano[cartaAtacante - 1];
    Carta* oponenteCarta = oponente->mano[cartaOponente - 1];

    // El daño es igual al valor de ataque del jugador
    int danoNeto = atacanteCarta->ataque;

    // Asegúrate de que el daño nunca sea negativo
    if (danoNeto < 0) {
        danoNeto = 0;
    }

    // Resta vida a la carta del oponente
    oponenteCarta->vida -= danoNeto;

    // Elimina la carta del oponente si su vida llega a 0 o menos
    if (oponenteCarta->vida <= 0) {
        printf("La carta del oponente ha muerto!\n");
        oponenteCarta->vida = 0;
        oponente->mano[cartaOponente - 1]->nombreCarta[0] = '\0'; // Elimina la carta del oponente
        jugador->puntosVida--;  // Resta un punto de vida al jugador
        oponente->puntosVida--; // Resta un punto de vida al oponente
    }

    // Muestra el resultado del ataque
    printf("%s ha atacado al oponente con %s y causó %d puntos de daño.\n", jugador->nombreJugador, atacanteCarta->nombreCarta, danoNeto);
    printf("Puntos de vida del oponente: %d\n", oponente->puntosVida);
}

void sacarCartaDeMano(Jugador *jugador) {
    int cartasEnMano = 0;
    int i;

    for (i = 0; i < 3; i++) {
        if (jugador->mano[i]->nombreCarta[0] != '\0') {
            cartasEnMano++;
        }
    }

    if (cartasEnMano >= 3) {
        printf("La mano del jugador está llena. No se pueden sacar más cartas.\n");
        return;
    }

    for (i = 0; i < 3; i++) {
        if (jugador->mano[i] == NULL) {
            jugador->mano[i] = jugador->mazo[i];
            jugador->mazo[i] = NULL;
            break;
        }
    }
}

void mostrarCartasOponente(Jugador* oponente) {
    printf("Cartas del oponente:\n");
    int i;
    for (i = 0; i < 3; i++) {
        if (oponente->mano[i]->nombreCarta[0] != '\0') {
            printf("%d. Nombre: %s, Tipo: %s, Ataque: %d, Defensa: %d, Vida: %d\n",
                   i + 1,
                   oponente->mano[i]->nombreCarta,
                   oponente->mano[i]->tipo,
                   oponente->mano[i]->ataque,
                   oponente->mano[i]->defensa,
                   oponente->mano[i]->vida);
        }
    }
}

void turnoComputadora(Jugador *computadora, Jugador *jugador) {
    int accion = rand() % 2;

    if (accion == 0) {
        atacarComputadora(computadora, jugador);
    } else {
        sacarCartaDeMano(computadora);
    }
}

void turnoJugador(Jugador* jugador, Jugador* oponente) {
    int i;
    int cartasEnMano = 0;

    printf("Cartas en la mano de %s:\n", jugador->nombreJugador);

    for (i = 0; i < 3; i++) {
        if (jugador->mano[i]->nombreCarta != '\0') {
            printf("%d. Nombre: %s, Tipo: %s, Ataque: %d, Defensa: %d, Vida: %d\n",
                   i + 1,
                   jugador->mano[i]->nombreCarta,
                   jugador->mano[i]->tipo,
                   jugador->mano[i]->ataque,
                   jugador->mano[i]->defensa,
                   jugador->mano[i]->vida);
            cartasEnMano++;
        }
    }

    if (cartasEnMano == 0) {
        printf("No tienes cartas en tu mano.\n");
        return;
    }

    int opcion;
    printf("Escoje una opcion:\n");
    printf("1 para sacar una carta\n");
    printf("2 para atacar al oponente\n");
    printf("Ingresa tu elección: ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            sacarCartaDeMano(jugador);
            break;
        case 2:
            atacarOponente(jugador, oponente);
            break;
        default:
            printf("Opción no válida. Por favor, selecciona una opción válida.\n");
    }
}

void crearCarta(Carta cartas[], int *datos) {
    if (*datos >= 60) { 
        printf("La lista de cartas está llena. No se puede agregar más cartas.\n");
        return;
    }

    printf("Ingrese el nombre de la carta: ");
    scanf("%s", cartas[*datos].nombreCarta);

    int tipoValido = 0;
    int i;

    do {
        printf("Ingrese el tipo de la carta (Mago, Vikingo, Nigromante o Bestia): ");
        scanf("%s", cartas[*datos].tipo);

        for (i = 0; cartas[*datos].tipo[i]; i++) {
            cartas[*datos].tipo[i] = tolower(cartas[*datos].tipo[i]);
        }
        if (strcmp(cartas[*datos].tipo, "mago") == 0 ||
            strcmp(cartas[*datos].tipo, "vikingo") == 0 ||
            strcmp(cartas[*datos].tipo, "nigromante") == 0 ||
            strcmp(cartas[*datos].tipo, "bestia") == 0) {
            tipoValido = 1;
        } else {
            printf("Tipo de carta no válido. Por favor, ingrese un tipo válido.\n");
        }
    } while (!tipoValido);

    printf("Ingrese la vida de la carta: ");
    scanf("%d", &cartas[*datos].vida);
    printf("Ingrese el ataque de la carta: ");
    scanf("%d", &cartas[*datos].ataque);
    printf("Ingrese la defensa de la carta: ");
    scanf("%d", &cartas[*datos].defensa);
    
    (*datos)++;
    printf("La carta ha sido creada y agregada a la lista.\n");
}

// Función para mostrar el historial
void mostrarHistorial(RegistroEvento historial[], int numEventos) {
	int i;
    printf("Historial de eventos:\n");
    for (i = 0; i < numEventos; i++) {
        printf("%d. %s\n", i + 1, historial[i].evento);
    }
}

void agregarEvento(RegistroEvento historial[], int* numEventos, const char* evento) {
    if (*numEventos < MAX_EVENTOS) {
        strncpy(historial[*numEventos].evento, evento, sizeof(historial[*numEventos].evento));
        (*numEventos)++;
    } else {
        printf("El historial está lleno. No se pueden agregar más eventos.\n");
    }
}

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "");

	RegistroEvento historial[MAX_EVENTOS]; // Declarar un arreglo para el historial
    int numEventos = 0; // Inicializar el número de eventos

    FILE* file;
    file = fopen("cartas.txt", "r");

    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    Carta* cartasOriginales = (Carta*)malloc(60 * sizeof(Carta));

    if (cartasOriginales == NULL) {
        printf("Error al asignar memoria para cartasOriginales.\n");
        fclose(file);
        return 1;
    }

    int datos = 0;
    int i;
    Jugador *jugador = (Jugador*)malloc(sizeof(Jugador));
    Jugador *oponente = (Jugador*)malloc(sizeof(Jugador));

    if (jugador == NULL || oponente == NULL) {
        printf("Error al asignar memoria para jugador u oponente.\n");
        return 1;
    }

    jugador->puntosVida = 5;
    oponente->puntosVida = 5;

    while (datos < 60 && fscanf(file, " %39[^,],%19[^,],%d,%d,%d\n",
                       cartasOriginales[datos].nombreCarta,
                       cartasOriginales[datos].tipo,
                       &cartasOriginales[datos].vida,
                       &cartasOriginales[datos].ataque,
                       &cartasOriginales[datos].defensa) == 5) {
        datos++;
    }

    fclose(file);

    printf("%d datos leídos.\n\n", datos);
	barajarCartas(cartasOriginales, datos);

    printf("Bienvenido al juego de The Clash of the Guardians\n");
    printf("Ingresa tu nombre: ");
    scanf("%s", jugador->nombreJugador);



    for (i = 0; i < 12; i++) {
        jugador->mazo[i] = &cartasOriginales[i];
        oponente->mazo[i] = &cartasOriginales[i + 3];
    }
    while (1) {
        int turno = 1;
        int opcion;
        printf("Escoje una opcion:\n");
        printf("1 para crear una carta\n");
        printf("2 para jugar\n");
        printf("3 para mostrar historial\n");
        printf("4 para salir\n");
        printf("Ingresa tu elección: ");
        scanf("%d", &opcion);
		
        if (opcion == 1) {
            printf("Has seleccionado la Opción de crear una carta\n");
            crearCarta(cartasOriginales, &datos);
            agregarEvento(historial, &numEventos, "Se ingreso una carta nueva dentro de la tanda");
        } else if (opcion == 2) {
            printf("Has seleccionado la Opción 2\n");
            
            Mazo mazo;
            
    		llenarMazoInicial(&mazo, cartasOriginales);
    		
    		elegirCartasParaMano(&mazo, jugador);
    		
            repartirCartas(cartasOriginales, jugador, oponente, datos);
            
            while (jugador->puntosVida > 0 && oponente->puntosVida > 0) {
                if (turno == 1) {
                    turnoJugador(jugador, oponente);
                    turno = 2;
                    agregarEvento(historial, &numEventos, "Jugador ataco al oponente");
                } else {
                    turnoComputadora(oponente, jugador);
                    turno = 1;
                    agregarEvento(historial, &numEventos, "computadora ataco al oponente");
                }
            }
            if (jugador->puntosVida <= 0) {
			    printf("¡Has perdido! La Computadora ha ganado.\n");
			} else if (oponente->puntosVida <= 0) {
			    printf("¡Has ganado! Has derrotado a la Computadora.\n");
			}
            
        } else if (opcion == 3) {
            printf("Has seleccionado la Opción 3\n");
            mostrarHistorial(historial, numEventos);
        } else if (opcion == 4) {
            printf("Saliendo del programa\n");
            free(jugador);
            free(oponente);
            break;
        } else {
            printf("Opción no válida, por favor, selecciona una opción válida\n");
        }
    }
    return 0;
}
