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
using namespace std;

typedef struct { int x; int y; int ancho; int alto; int vel_x; int vel_y; } Jugador;

int main() {


	if (!al_init()) {
		al_show_native_message_box(NULL, "ERROR CRITICO", "ERROR:404", "No se pudo cargar correctamente la libreria Allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//toma el ancho y alto de los pixeles de la pantalla
	int ancho = GetSystemMetrics(SM_CXSCREEN);
	int alto = GetSystemMetrics(SM_CYSCREEN);

	//crea la ventana 
	ALLEGRO_DISPLAY* ventana = al_create_display(1880, 960);
	al_set_window_title(ventana, "Videojuego :]");
	//posiciona la ventana al centro
	al_set_window_position(ventana, ancho / 2 - 1880 / 2, alto / 2 - 960 / 2);

	

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

	//inicializando el timer
	ALLEGRO_TIMER* segundoTimer = al_create_timer(1.0);
	ALLEGRO_TIMER* FPS = al_create_timer(1.0 / 30);
	
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	al_register_event_source(queue, al_get_timer_event_source(segundoTimer));
	al_register_event_source(queue, al_get_timer_event_source(FPS));
	al_start_timer(segundoTimer);
	al_start_timer(FPS);

	//inicializar mouse
	al_install_mouse();
	al_register_event_source(queue, al_get_mouse_event_source());

	//inicializando primitivas (figuras)
	al_init_primitives_addon();

	//CICLO DEL DISPLAY
	int segundo = 0;
	int countFPS = 0;
	int x = -1, y = -1;
	while (true)
	{
		ALLEGRO_EVENT evento;
		al_wait_for_event(queue, &evento);
		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (evento.timer.source == segundoTimer) {
				segundo++;
				cout << segundo << endl;
			}
		}
		if (evento.timer.source == FPS) {
			countFPS++;
			if (countFPS % 30 == 0)
			{
				cout << countFPS << endl;
			}
		}
		//POSICION DEL MOUSE
		if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
			x = evento.mouse.x;
			y = evento.mouse.y;
			cout << "x: " << x << "		y :" << y <<endl;
			if (evento.mouse.button & 1) {
				a
			}
		}


		//epilepsia
		//al_clear_to_color(al_map_rgb(segundo * 1 % 255, segundo * 1 % 255, segundo * 1 % 255));

		// FIGURAS:
		//al_draw_filled_rectangle(400, 400, 250, 150, blanco);
		al_draw_filled_circle(200, 600, 100, blanco);


		//al_draw_text(fuente, al_map_rgb(255, 255, 255), 200, 200, 0, ("Segundo: " + to_string(segundo).c_str())); // REPARAR FUENTES :((
		al_flip_display();
	}
	return 0;
}