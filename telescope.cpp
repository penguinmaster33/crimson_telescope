#include <iostream>
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
#define AL_P 800

using namespace std;

typedef struct { int x; int y; int ancho; int alto; int vel_x; int vel_y; } Jugador;

// Tipos y estado para pantallas secundarias
enum ShapeType { SHAPE_TRIANGLE, SHAPE_CIRCLE, SHAPE_RECTANGLE };

typedef struct {
	ShapeType shape;
	ALLEGRO_COLOR bgColor;
	ALLEGRO_COLOR shapeColor;
	int size;
	int pos_x;
	int pos_y;
	bool filled;
} WindowState;

// Función que abre la pantalla de dibujo/modal y se queda en su propio bucle
void fondo1(WindowState& ws, ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente) {
	bool running = true;
	ALLEGRO_EVENT ev;

	// Redibuja en respuesta a eventos hasta que se salga (ESC o clic derecho)
	while (running) {
		al_wait_for_event(queue, &ev);

		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				// click izquierdo: cambiar forma (ejemplo)
				if (ws.shape == SHAPE_TRIANGLE) ws.shape = SHAPE_CIRCLE;
				else if (ws.shape == SHAPE_CIRCLE) ws.shape = SHAPE_RECTANGLE;
				else ws.shape = SHAPE_TRIANGLE;
			}

			if (ev.mouse.button & 2) {
				// click derecho: salir de la pantalla
				running = false;
			}
		}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				running = false;
			}
			// ejemplo: teclas para cambiar color/llenado
			if (ev.keyboard.keycode == ALLEGRO_KEY_F) ws.filled = !ws.filled;
		}

		// Siempre dibujamos después de procesar eventos (simple, sin timers adicionales)
		al_clear_to_color(ws.bgColor);

		// Dibujar un indicador de "salir" en la esquina
		ALLEGRO_COLOR backButton = al_map_rgb(80, 80, 80);
		al_draw_filled_rectangle(10, 10, 110, 40, backButton);
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 20, 15, 0, "Salir (click derecho / ESC)");

		// Dibujar la forma seleccionada
		ALLEGRO_COLOR sCol = ws.shapeColor;
		int cx = ws.pos_x;
		int cy = ws.pos_y;
		int s = ws.size;

		if (ws.shape == SHAPE_TRIANGLE) {
			if (ws.filled)
				al_draw_filled_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol);
			else
				al_draw_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol, 2.0);

		}
		else
			if (ws.shape == SHAPE_CIRCLE) {
				if (ws.filled)
					al_draw_filled_circle(cx, cy, s, sCol);
				else
					al_draw_circle(cx, cy, s, sCol, 2.0);
			}
			else
				if (ws.shape == SHAPE_RECTANGLE) {
					if (ws.filled)
						al_draw_filled_rectangle(cx - s, cy - s, cx + s, cy + s, sCol);
					else
						al_draw_rectangle(cx - s, cy - s, cx + s, cy + s, sCol, 2.0);
				}

		al_flip_display();
	}
}

// Función que abre la pantalla de dibujo/modal y se queda en su propio bucle
void fondo2(WindowState& ws, ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente) {
	bool running = true;
	ALLEGRO_EVENT ev;

	// Redibuja en respuesta a eventos hasta que se salga (ESC o clic derecho)
	while (running) {
		al_wait_for_event(queue, &ev);

		// ELIGE CLICK IZQ, O DER
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				// click izquierdo: cambiar forma (ejemplo)
				if (ws.shape == SHAPE_TRIANGLE) ws.shape = SHAPE_CIRCLE;
				else if (ws.shape == SHAPE_CIRCLE) ws.shape = SHAPE_RECTANGLE;
				else ws.shape = SHAPE_TRIANGLE;
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
			if (ev.keyboard.keycode == ALLEGRO_KEY_F) ws.filled = !ws.filled;
		}

		// Siempre dibujamos después de procesar eventos (simple, sin timers adicionales)
		al_clear_to_color(ws.bgColor);

		// Dibujar un indicador de "salir" en la esquina
		ALLEGRO_COLOR backButton = al_map_rgb(80, 80, 80);
		al_draw_filled_rectangle(10, 10, 110, 40, backButton);
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 20, 15, 0, "Salir (click derecho / ESC)");

		// Dibujar la forma seleccionada
		ALLEGRO_COLOR sCol = ws.shapeColor;
		int cx = ws.pos_x;
		int cy = ws.pos_y;
		int s = ws.size;

		if (ws.shape == SHAPE_TRIANGLE) {
			if (ws.filled) {
				al_draw_filled_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol);
			}
			else {
				al_draw_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol, 2.0);
			}
		}
		else if (ws.shape == SHAPE_CIRCLE) {
			if (ws.filled) al_draw_filled_circle(cx, cy, s, sCol);
			else al_draw_circle(cx, cy, s, sCol, 2.0);
		}
		else if (ws.shape == SHAPE_RECTANGLE) {
			if (ws.filled) al_draw_filled_rectangle(cx - s, cy - s, cx + s, cy + s, sCol);
			else al_draw_rectangle(cx - s, cy - s, cx + s, cy + s, sCol, 2.0);
		}

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
	ALLEGRO_COLOR negro = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR gris = al_map_rgb(100, 100, 100);

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

	//CICLO DEL DISPLAY
	int segundo = 0;
	//int countFPS = 0;
	int x = -1, y = -1;
	int HRZ = AN_P / 10;
	int VRT = AL_P / 10;

	while (true)
	{
		ALLEGRO_EVENT evento;
		al_wait_for_event(queue, &evento);
		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (evento.timer.source == segundoTimer) {
				segundo++;
				cout << "| Seg :" << segundo << " |" << endl;
			}
		}

		/*	if (evento.timer.source == FPS) {
			countFPS++;
			if (countFPS % 30 == 0) {
				cout << countFPS << endl;
			}
		}  */



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

					// Crear estado inicial para la pantalla de dibujo y abrirla
					WindowState ws;
					ws.shape = SHAPE_TRIANGLE; // ejemplo: empezar con triangulo
					ws.bgColor = negro; // limpiar a negro
					ws.shapeColor = blanco;
					ws.size = 80;
					ws.pos_x = AN_P / 2;
					ws.pos_y = AL_P / 2;
					ws.filled = true;

					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el primer rectangulo |" << endl;
					fondo1(ws, ventana, queue, fuente);


				}
			}

			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				// Handle click within the third rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					// Crear estado inicial para la pantalla de dibujo y abrirla
					WindowState ws;

					ws.shape = SHAPE_TRIANGLE; // ejemplo: empezar con triangulo
					ws.bgColor = negro; // limpiar a negro
					ws.shapeColor = blanco;
					ws.size = 80;
					ws.pos_x = AN_P / 2;
					ws.pos_y = AL_P / 2;
					ws.filled = true;

					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el segundo rectangulo |" << endl;
					fondo2(ws, ventana, queue, fuente);


				}
			}


			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the second rectangle
				al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {

					cout << "| Click en el tercer rectangulo |" << endl;
				}
			}


			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the fourth rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {

					cout << "| Click en el cuarto rectangulo |" << endl;
				}
			}

		}













		// FIGURAS:


		//al_draw_text(fuente, al_map_rgb(255, 255, 255), 200, 200, 0, ("Segundo: " + to_string(segundo).c_str())); // REPARAR FUENTES :((


		al_flip_display();
	}
	return 0;
}
