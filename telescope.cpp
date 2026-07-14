#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>}
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <string>
#include <Windows.h>

#define AN_P 1000
#define AL_P 1000

// PROBLEMAS MAYORES:
// 1. Al parecer, por haber dos ciclos, hay inputs que no llegan a recibirse y se saltan
// 2. Los clicks ocurren al tener el mouse en MOVIMIENTO. Si lo haces y no lo mueves, no se registra _nada_
// 3. Reparar menu

typedef struct {
	int size_x;
	int size_y;
	int pos_x;
	int pos_y;
	int velocidad;
} creature;

typedef struct {
	int blanco;
}color;

typedef struct {
	ALLEGRO_COLOR bgColor;
	ALLEGRO_COLOR shapeColor;
	int size;
	int pos_x;
	int pos_y;
	bool filled;
} WindowState;

void menu(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente);
void ciclo(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente);
void dibujo(ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq, creature cuad);
void fondo(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq, creature cuad);
void movimiento(int mov_x, int mov_y, creature cuad);


int main() {

	if (!al_init()) {
		al_show_native_message_box(NULL, "ERROR CRITICO", "ERROR:404", "No se pudo cargar correctamente la libreria Allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//toma el ancho y alto de los pixeles de la pantalla
	int ancho = GetSystemMetrics(SM_CXSCREEN);
	int alto = GetSystemMetrics(SM_CYSCREEN);

	//crea la ventana 
	ALLEGRO_DISPLAY* ventana = al_create_display(AN_P, AL_P);
	al_set_window_title(ventana, "Videojuego :]");
	//posiciona la ventana al centro
	al_set_window_position(ventana, ancho / 2 - AN_P / 2, alto / 2 - AL_P / 2);



	//inicializa las fuentes
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT* fuente = al_create_builtin_font();
	if (!fuente) {
		al_show_native_message_box(NULL, "ERROR CRITICO", "ERROR:404", "No se pudo cargar correctamente la fuente", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//inicializando colores


	//inicializando el timer
	ALLEGRO_TIMER* segundoTimer = al_create_timer(1.0);
	ALLEGRO_TIMER* FPS = al_create_timer(1.0 / 120);

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	al_register_event_source(queue, al_get_timer_event_source(segundoTimer));
	al_register_event_source(queue, al_get_timer_event_source(FPS));
	al_start_timer(segundoTimer);
	al_start_timer(FPS);

	//inicializar mouse
	al_install_mouse();
	al_register_event_source(queue, al_get_mouse_event_source());

	//teclado
	al_install_keyboard();
	al_register_event_source(queue, al_get_keyboard_event_source());

	//inicializando primitivas (figuras)
	al_init_primitives_addon();

	menu(ventana, queue, segundoTimer, FPS, fuente);

	return 0;
}


void menu(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente) {
	int x = -1;
	int y = -1;
	int HRZ = AN_P / 10;
	int VRT = AL_P / 10;

	ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR gris = al_map_rgb(100, 100, 100);

	ALLEGRO_EVENT evento;




	int andar = 1;

	while (andar) {
		al_wait_for_event(queue, &evento);

		al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 9.5, VRT * 9.5, blanco);
		if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			x = evento.mouse.x;
			y = evento.mouse.y;
			if (x >= HRZ * 2 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 9.5) {
				al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button == 1) {
					printf("abrido ciclo\n");
					ciclo(ventana, queue, segundoTimer, FPS, fuente);
				}
			}
		}
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				andar = false;
				break;
			}
		}
	}
	if (andar == false)
		printf("programa cerrado");
}

void ciclo(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente) {

	//CICLO DEL DISPLAY
	int segundo = 0;
	//int countFPS = 0;
	int x = -1, y = -1;
	int HRZ = AN_P / 10; //100
	int VRT = AL_P / 10; //100

	int mov_x=0, mov_y=0;

	int mapa_total[20][20] = {
	1,	1,	1,	1,	1,	1,  1,  1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	0,	0,	2,	0,	0,	2,	0,	0,	1,	1,	0,	0,	0,	0,	0,	0,	9,	9,	1,
	1,	0,	0,	2,	0,	0,	2,	0,	0,	1,	1,	0,	0,	0,	0,	0,	0,	9,	9,	1,
	1,	0,	0,	1,	0,  0,	1,	0,	0,	1,	1,	2,	2,	1,	1,	1,	1,	2,	2,  1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	2,	2,	0,	0,	2,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	2,	2,	0,	0,	2,	0,	0,	2,	0,	0,	1,
	1,	2,	2,	1,	1,	1,	1,	1,	1,	1,  1,	1,	1,	1,	0,	0,	1,	1,	1,	1,
	1,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	1,	1,	1,	1,
	1,	0,	0,	1,	0,	0,	0,	0,	0,	2,	2,	0,	0,	1,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	0,	0,	0,	2,	2,	0,	0,	1,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	2,	0,	0,	0,	0,	0,	1,	1,	0,	0,	2,	0,	0,	1,  0,  0,  1,
	1,	0,	0,	2,	0,	0,	0,	0,  0,	1,	1,	0,	0,	2,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	2,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	2,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,  1,	1,	1,	1,  1,	1,	1,	1,
	};

	int i = 0, j = 0;
	//int mapa_total2[20][20];
	/*for (i = 0; i < 20; i++)
		for (j = 0; j < 20; j++) {
			scanf("%d", &mapa_total);
			printf("%d", mapa_total[i][j]);
		}
	*/
	creature cuad;
	cuad.pos_x = 2 * HRZ;
	cuad.pos_y = 2 * VRT;
	cuad.size_x = AN_P / 10;
	cuad.size_y = AL_P / 10;
	cuad.velocidad = 5;

	int camara = 0, esq = 0;

	while (true)
	{
		ALLEGRO_COLOR negro = al_map_rgb(0, 0, 0);
		ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255);
		ALLEGRO_COLOR gris = al_map_rgb(100, 100, 100);

		ALLEGRO_EVENT evento;
		// Obtener siguiente evento de la cola antes de leerlo
		al_wait_for_event(queue, &evento);


		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (evento.timer.source == segundoTimer) {
				segundo++;
				printf("| Seg : %d |\n", segundo);
			}
		}


		if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && camara == 0) {
			x = evento.mouse.x;
			y = evento.mouse.y;


			al_clear_to_color(negro);

			al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, blanco);
			al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, blanco);
			al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, blanco);
			al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, blanco);


			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, gris);
				if (evento.mouse.button == 1) {
					esq = 1;
					printf("Cuadrante 1\n");
					camara = 1;
				}
			}

			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button == 1) {
					esq = 2;
					printf("Cuadrante 2\n");
					camara = 2;
				}
			}


			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, gris);
				if (evento.mouse.button == 1) {
					esq = 3;
					printf("Cuadrante 3\n");
					camara = 3;
				}
			}


			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button == 1) {
					esq = 4;
					printf("Cuadrante 4\n");
					camara = 4;
				}
			}
		}

		if (camara == 1)
			fondo(ventana, queue, fuente, mapa_total, esq, cuad);
		else if (camara == 2)
			fondo(ventana, queue, fuente, mapa_total, esq, cuad);
		else if (camara == 3)
			fondo(ventana, queue, fuente, mapa_total, esq, cuad);
		else if (camara == 4)
			fondo(ventana, queue, fuente, mapa_total, esq, cuad);


		if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {	// JUGADOR PRESIONO TECLA. CICLO FOR PARA REVISAR CADA BORDE PARA HACER COLISIONES.
			for (i = 0;i < 20;i++)
				for (j = 0;j < 20;j++)
					if (mapa_total[i][j] < cuad.pos_y) // QUIERO GUARDAR EL CUADRANTE EN EL QUE ESTÁ CADA BORDE DE ESTE INDIVIDUO PARA COMPARARLO CON EL BORDE DE ABAJO MAS FACILMENTE
						 ;
		}
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				mov_y = 1;
				movimiento(mov_x, mov_y, cuad);
				printf("tecla abajo presionada");
			}
			if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
				mov_y = -1;
				movimiento(mov_x, mov_y, cuad);
				printf("tecla arriba presionada");
			}
			if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				mov_x = -1;
				movimiento(mov_x, mov_y, cuad);
				printf("tecla izquierda presionada");
			}
			if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				mov_y = 1;
				movimiento(mov_x, mov_y, cuad);
				printf("tecla derecha presionada");
			}
		}




		/*if (mapa_total[][])
			cuad.pos_x = cuad.pos_x - cuad.velocidad;

		if (cuad.pos_y < AL_P / 20)
			cuad.pos_y = cuad.pos_y - cuad.velocidad;

		if (cuad.pos_x + cuad.size_x > AN_P)
			cuad.pos_x = AN_P - cuad.size_x;

		if (cuad.pos_y + cuad.size_y > AL_P)
			cuad.pos_y = AL_P - cuad.size_y;
			*/


		if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
			camara = 0;
			esq = 0;
			printf("ESCAPE PRESIONADO, SALIR DE 'fondo'\n");
		}
		al_flip_display();
	}
}

void fondo(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq, creature cuad) {
	bool running = true;
	ALLEGRO_EVENT ev;
 	al_wait_for_event(queue, &ev);

	int xplus = 0, yplus = 0;
	int i, j;
	int x, y;
	int limPuertaX, limPuertaY;
	int celAn = AN_P / 10; // un decimo de tamanho de pantalla
	int celAl = AL_P / 10;


	if (esq == 1) {
		xplus = 0;			yplus = 0;
	}
	else if (esq == 2) {
		xplus = 10;			yplus = 0;
	}
	else if (esq == 3) {
		xplus = 0;			yplus = 10;
	}
	else if (esq == 4) {
		xplus = 10;			yplus = 10;
	}



	if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		x = ev.mouse.x;
		y = ev.mouse.y;


		for (i = 0; i < 10; ++i) {
			for (j = 0; j < 10; ++j) {
				if (mapa_total[i + yplus][j + xplus] == 2 || mapa_total[i + yplus][j + xplus] == 3) {
					limPuertaX = j * celAn;
					limPuertaY = i * celAl;
					if (x >= limPuertaX && x <= limPuertaX + celAn && y >= limPuertaY && y <= limPuertaY + celAl) {

						if (ev.mouse.button == 1) {

							if (mapa_total[i + yplus][j + xplus] == 2) { // OKAY; AQUI ES PUERTA CERRADA. A chequear adyacentes!
								mapa_total[i + yplus][j + xplus] = 3;

								if (mapa_total[i + yplus - 1][j + xplus] == 2)
									mapa_total[i + yplus - 1][j + xplus] = 3;
								if (mapa_total[i + yplus + 1][j + xplus] == 2)
									mapa_total[i + yplus + 1][j + xplus] = 3;
								if (mapa_total[i + yplus][j + xplus - 1] == 2)
									mapa_total[i + yplus][j + xplus - 1] = 3;
								if (mapa_total[i + yplus][j + xplus + 1] == 2)
									mapa_total[i + yplus][j + xplus + 1] = 3;
								if (mapa_total[i + yplus - 1][j + xplus - 1] == 2)
									mapa_total[i + yplus - 1][j + xplus - 1] = 3;
								if (mapa_total[i + yplus - 1][j + xplus + 1] == 2)
									mapa_total[i + yplus - 1][j + xplus + 1] = 3;
								if (mapa_total[i + yplus + 1][j + xplus - 1] == 2)
									mapa_total[i + yplus + 1][j + xplus - 1] = 3;
								if (mapa_total[i + yplus + 1][j + xplus + 1] == 2)
									mapa_total[i + yplus + 1][j + xplus + 1] = 3;
							}

							else if (mapa_total[i + yplus][j + xplus] == 3) { // OKAY; AQUI ES PUERTA ABIERTA. A chequear adyacentes!
								mapa_total[i + yplus][j + xplus] = 2;

								if (mapa_total[i + yplus - 1][j + xplus] == 3)
									mapa_total[i + yplus - 1][j + xplus] = 2;
								if (mapa_total[i + yplus + 1][j + xplus] == 3)
									mapa_total[i + yplus + 1][j + xplus] = 2;
								if (mapa_total[i + yplus][j + xplus - 1] == 3)
									mapa_total[i + yplus][j + xplus - 1] = 2;
								if (mapa_total[i + yplus][j + xplus + 1] == 3)
									mapa_total[i + yplus][j + xplus + 1] = 2;
								if (mapa_total[i + yplus - 1][j + xplus - 1] == 3)
									mapa_total[i + yplus - 1][j + xplus - 1] = 2;
								if (mapa_total[i + yplus - 1][j + xplus + 1] == 3)
									mapa_total[i + yplus - 1][j + xplus +
									1] = 2;
								if (mapa_total[i + yplus + 1][j + xplus - 1] == 3)
									mapa_total[i + yplus + 1][j + xplus - 1] = 2;
								if (mapa_total[i + yplus + 1][j + xplus + 1] == 3)
									mapa_total[i + yplus + 1][j + xplus + 1] = 2;
							}

							printf("click en puerta\n");
						}
					}
				}
			}
		}
	}

	

	al_clear_to_color(al_map_rgb(0, 0, 0));
	dibujo(fuente, mapa_total, esq, cuad);
}

void dibujo(ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq, creature cuad)
{
	int i, j;
	al_clear_to_color(al_map_rgb(0, 0, 0));

	// mapa_total es 20x20; calcular tamaño de celda según tamaño de ventana (AN_P x AL_P)
	// DEBERIA QUITARSE EL *2 Y DEJAR EN 10x10, es lo q se muestra
	int cols = 20;
	int fils = 20;
	int celAn = AN_P / cols;
	int celAl = AL_P / fils; //float?
	int xplus, yplus;

	if (esq == 1) {
		xplus = 0;		yplus = 0;
	}
	else if (esq == 2) {
		xplus = 10;		yplus = 0;
	}
	else if (esq == 3) {
		xplus = 0;		yplus = 10;
	}
	else if (esq == 4) {
		xplus = 10;		yplus = 10;
	}

	for (i = 0; i < fils; ++i) {
		for (j = 0; j < cols; ++j) {
			if (mapa_total[i + yplus][j + xplus] == 1) {
				int x1 = (j * celAn * 2);
				int y1 = (i * celAl * 2);
				int x2 = (x1 + celAn * 2);
				int y2 = (y1 + celAl * 2);
				al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(255, 255, 255));
			}
			else if (mapa_total[i + yplus][j + xplus] == 2) {
				int x1 = (j * celAn * 2);
				int y1 = (i * celAl * 2);
				int x2 = (x1 + celAn * 2);
				int y2 = (y1 + celAl * 2);
				al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(0, 255, 0));
			}

			else if (mapa_total[i + yplus][j + xplus] == 3) {
				int x1 = (j * celAn * 2);
				int y1 = (i * celAl * 2);
				int x2 = (x1 + celAn * 2);
				int y2 = (y1 + celAl * 2);
				al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(0, 0, 255));
			}

			else if (mapa_total[i + yplus][j + xplus] == 9) {
				int x1 = (j * celAn * 2);
				int y1 = (i * celAl * 2);
				int x2 = (x1 + celAn * 2);
				int y2 = (y1 + celAl * 2);
				al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(255, 0, 0));
			}
		}
	}

	al_draw_filled_rectangle(cuad.pos_x, cuad.pos_y, cuad.size_x, cuad.size_y, al_map_rgb(255, 0, 255));


}

void movimiento(int mov_x, int mov_y, creature cuad) {
	if (mov_y == 1) 
		cuad.pos_y = cuad.pos_y + cuad.velocidad;
	else if (mov_y == -1) 
		cuad.pos_y = cuad.pos_y - cuad.velocidad;

	if (mov_x == 1)
		cuad.pos_x = cuad.pos_x + cuad.velocidad;
	else if (mov_y == -1)
		cuad.pos_x = cuad.pos_x - cuad.velocidad;



}
