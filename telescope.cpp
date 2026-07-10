#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>}
#include <allegro5/allegro_primitives.h>
#include <string>
#include <Windows.h>

#define AN_P 1000
#define AL_P 1000

typedef struct {
	int size;
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
void ciclo(ALLEGRO_DISPLAY * ventana, ALLEGRO_EVENT_QUEUE * queue, ALLEGRO_TIMER * segundoTimer, ALLEGRO_TIMER * FPS, ALLEGRO_FONT * fuente);
void dibujar(ALLEGRO_FONT* fuente, int mapa_total[20][20], int esquina);
void fondo1(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esquina);
void fondo2(WindowState	ws, ALLEGRO_DISPLAY *ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esquina);


void menu(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente) {
	int x = -1; 	
	int y = -1;
	int HRZ = AN_P / 10;
	int VRT = AL_P / 10;

	ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR gris = al_map_rgb(100, 100, 100);

	ALLEGRO_EVENT evento;
	al_wait_for_event(queue, &evento);

	al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 9.5, VRT * 9.5, blanco);

	int andar = 1;
	
	while (andar) {
		al_wait_for_event(queue, &evento);
		if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			x = evento.mouse.x;
			y = evento.mouse.y;
			printf("evento x: %d, y: %d\n", x, y);
			if (x >= HRZ * 2 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 9.5) {
				al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {
					printf("abrido ciclo");
					ciclo(ventana, queue, segundoTimer, FPS, fuente);
				}
			}

			if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
				if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					andar = false;
				}
			}
		}
	}
	if (andar == false)
		printf("programa cerrado");
}

void dibujo(ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq)
{
	int i, j;
	al_clear_to_color(al_map_rgb(0, 0, 0));

	// mapa_total es 20x20; calcular tamaño de celda según tamaño de ventana (AN_P x AL_P)
	int cols = 20;
	int rows = 20;
	int celAn = AN_P / cols; //float?
	int celAl = AL_P / rows; //float?
	int xplus = 0;
	int yplus = 0;

	if (esq==1){
		xplus = 0;
		yplus = 0;
	}
	else if (esq == 2) {

		xplus = AN_P / 2;
		yplus = 0;
	}
	else if (esq == 3) {

		xplus = 0;
		yplus = AL_P / 2;
	}
	else if (esq == 4) {

		xplus = AN_P / 2;
		yplus = AL_P / 2;
	}


	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (mapa_total[i][j] == 1) {

				int x1 = (j * celAn * 2) + xplus;
				int y1 = (i * celAl * 2) + yplus;
				int x2 = (x1 + celAn * 2) + xplus;
				int y2 = (y1 + celAl * 2) + yplus;
				al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(255, 255, 255));
			}
		}
	}
}



void ciclo(ALLEGRO_DISPLAY *ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER *segundoTimer, ALLEGRO_TIMER* FPS, ALLEGRO_FONT* fuente){

	//CICLO DEL DISPLAY
	int segundo = 0;
	//int countFPS = 0;
	int x = -1, y = -1;
	int HRZ = AN_P / 10; //100
	int VRT = AL_P / 10; //100

	int mapa_total[20][20] = { 
	1,	1,	1,	1,	1,	1,  1,  1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	0,	0,	0,	0,	0,	2,	0,	0,	1,	1,	0,	0,	0,	0,	0,	2,	9,	9,	1,
	1,	0,	0,	0,	0,	0,	2,	0,	0,	1,	1,	0,	0,	0,	0,	0,	2,	9,	9,	1,
	1,	0,	0,	1,	2,	2,	1,	0,	0,	1,	1,	0,	0,	1,	1,	1,	1,	2,	2,  1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	7,	7,	1,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	7,	7,	1,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	8,	8,	1,	0,	0,	2,	2,	0,	0,	2,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	8,	8,	1,	0,	0,	2,	2,	0,	0,	2,	0,	0,	2,	0,	0,	1,
	1,	2,	2,	1,	1,	1,	1,	1,	1,	1,  1,	1,	1,	1,	0,	0,	1,	1,	1,	1,
	1,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	1,	1,	1,	1,
	1,	0,	0,	1,	0,	0,	0,	0,	0,	2,	2,	0,	0,	1,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	0,	0,	0,	2,	2,	0,	0,	1,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	1,	0,	0,	6,	0,	0,	1,	1,	0,	0,	1,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	2,	0,	0,	0,	0,	0,	1,	1,	0,	0,	2,	0,	0,	1,  0,  0,  1,
	1,	0,	0,	2,	0,	0,	0,	0,  0,	1,	1,	0,	0,	2,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	2,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	2,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,  1,	1,	1,	1,  1,	1,	1,	1,
	};
	
	creature cuad;
	cuad.size = 20;
	cuad.velocidad = 5;

	// Si el vector global del mapa está vacío, copiar desde el array local
	

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

		cuad.pos_x = 2 * HRZ;
		cuad.pos_x = 2 * VRT;
		int esq;



		//MOUSE
		if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			x = evento.mouse.x;
			y = evento.mouse.y;

			al_clear_to_color(negro);


			al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, blanco);
			al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, blanco);
			al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, blanco);
			al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, blanco);



			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				// Handle click within the first rectangle
				al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					printf("Cuadrante 1\n");
					esq = 1;
					fondo1(ventana, queue, fuente, mapa_total, esq);
					
				}
			}

			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				// Handle click within the third rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					WindowState ws;
					ws.bgColor = blanco; // limpiar a blanco
					ws.shapeColor = negro;
					ws.size = 80;
					ws.pos_x = AN_P / 2;
					ws.pos_y = AL_P / 2;
					ws.filled = true;
					printf("Cuadrante 2\n");
					esq = 2;
					fondo2(ws, ventana, queue, fuente, mapa_total, esq);


				}
			}


			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the second rectangle
				al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {
					printf("Cuadrante 3\n");
				}
			}


			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the fourth rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {

					printf("Cuadrante 4\n");
				}
			}

		}

		/*if (cuad.pos_x < AN_P / 20) cuad.pos_x = 0;
		if (cuad.pos_y < AL_P/20) cuad.pos_y = 0;
		if (cuad.pos_x + cuad.size > AN_P) cuad.pos_x = AN_P - cuad.size;
		if (cuad.pos_y + cuad.size > AL_P) cuad.pos_y = AL_P - cuad.size;*/


		al_flip_display();
	}
}

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
	ALLEGRO_TIMER* FPS = al_create_timer(1.0 / 60);

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	al_register_event_source(queue, al_get_timer_event_source(segundoTimer));
	al_register_event_source(queue, al_get_timer_event_source(FPS));
	al_start_timer(segundoTimer);
	al_start_timer(FPS);

	//inicializar mouse
	al_install_mouse();
	al_register_event_source(queue, al_get_mouse_event_source());

	// teclado (para poder salir con ESC en la pantalla secundaria)
	al_install_keyboard();
	al_register_event_source(queue, al_get_keyboard_event_source());

	//inicializando primitivas (figuras)
	al_init_primitives_addon();

	menu(ventana, queue, segundoTimer, FPS, fuente);

	return 0;
}

void fondo1(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq) {
	bool running = true;
	ALLEGRO_EVENT ev;

	// Redibuja en respuesta a eventos hasta que se salga (ESC o clic derecho)
	while (running) {
		al_wait_for_event(queue, &ev);

		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				al_draw_filled_rectangle(AN_P / 2 - 50, AL_P / 2 - 50, AN_P / 2 + 50, AL_P / 2 + 50, al_map_rgb(255, 255, 255));
			}

			if (ev.mouse.button & 2) {
			}
		}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				running = false;
			}
			
					}

		// Siempre dibujamos después de procesar eventos (simple, sin timers adicionales)
		al_clear_to_color(al_map_rgb(0,0,0));

		dibujo(fuente, mapa_total, esq);

		
		ALLEGRO_COLOR backButton = al_map_rgb(80, 80, 80);
		al_draw_filled_rectangle(10, 10, 110, 40, backButton);
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 20, 15, 0, "Salir (click derecho / ESC\n");

		// Dibujar la forma seleccionada

		al_flip_display();
	}
}

void fondo2(WindowState ws, ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, int mapa_total[20][20], int esq) {
	bool running = true;
	ALLEGRO_EVENT ev;

	// Redibuja en respuesta a eventos hasta que se salga (ESC o clic derecho)
	while (running) {
		al_wait_for_event(queue, &ev);

		// ELIGE CLICK IZQ, O DER
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
			}

			if (ev.mouse.button & 2) {
				// click derecho: salir de la pantalla
				running = false;
			}
		}

		// AGARRA ESC.
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				running = false;
			}
			// ejemplo: teclas para cambiar color/llenado
			if (ev.keyboard.keycode == ALLEGRO_KEY_F)
				printf("f");
		}

		// Siempre dibujamos después de procesar eventos (simple, sin timers adicionales)
		al_clear_to_color(al_map_rgb(0,0,0));


		dibujo(fuente, mapa_total, esq);
		
		ALLEGRO_COLOR backButton = al_map_rgb(80, 80, 80);
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 20, 15, 0, "Salir (click derecho / ESC\n");

		al_flip_display();
	}
}