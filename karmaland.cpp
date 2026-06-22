#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>}
#include <allegro5/allegro_primitives.h>
#include <string>
#include <vector>
#include <Windows.h>

#define AN_P 2000
#define AL_P 1500

using namespace std;

typedef struct { 
	int x; 
	int y; 
	int size; 
	int pos_x; 
	int pos_y; 
	int velocidad;
} creature;

// Tipos y estado para pantallas secundarias



// Función que abre la pantalla de dibujo/modal y se queda en su propio bucle
// Pantalla secundaria: dibuja fondo y mueve al jugador
// Movemos el cuadrado por pasos de `jug.velocidad` en cada eje cuando
void fondo1(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, ALLEGRO_KEYBOARD_STATE& estado_teclado, creature& jug) {
	bool running = true;
	ALLEGRO_EVENT ev;

	// Bucle principal de la pantalla secundaria
	while (running) {
		// Espera un evento (mouse, teclado, timer...)
		al_wait_for_event(queue, &ev);

		// Actualizar el estado del teclado cada iteración
		al_get_keyboard_state(&estado_teclado);

		// Mover el jugador por pasos de `velocidad` según flechas.
		// Sumamos al offset para que la posición se acumule.
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_UP))    jug.pos_y -= jug.velocidad; // subir
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_DOWN))  jug.pos_y += jug.velocidad; // bajar
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT))  jug.pos_x -= jug.velocidad; // izquierda
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) jug.pos_x += jug.velocidad; // derecha

		// Dibujar fondo y jugador (sin manejo de colisiones por ahora)
		al_clear_to_color(al_map_rgb(0,0,0));
		al_draw_filled_rectangle(jug.x + jug.pos_x, jug.y + jug.pos_y,
								 jug.x + jug.pos_x + jug.size, jug.y + jug.pos_y + jug.size,
								 al_map_rgb(255, 0, 0));

		// Botón/salida simple
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 50, 50, 0, "Salir con ESC");
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			running = false;

		al_draw_filled_rectangle(0, 0, AN_P / 100, AL_P, al_map_rgb(100,100,100));
		al_draw_filled_rectangle(0, 0, AN_P, AL_P / 100, al_map_rgb(100,100,100));
		al_draw_filled_rectangle(0, AL_P - AL_P /100,AN_P, AL_P, al_map_rgb(100, 100, 100));
		al_draw_filled_rectangle(AN_P - AN_P / 100, 0, AN_P, AL_P, al_map_rgb(100, 100, 100));

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
	ALLEGRO_KEYBOARD_STATE estado_teclado;
	al_get_keyboard_state(&estado_teclado);

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
										// Inicializar jugador     
					creature jug;
					jug.size = 100;
					jug.x = AN_P / 2 - jug.size / 2;
					jug.y = AL_P / 2 - jug.size / 2;
					jug.pos_x = 0;
					jug.pos_y = 0;
					jug.velocidad = 10;

					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el primer rectangulo |" << endl;
					fondo1(ventana, queue, fuente, estado_teclado, jug);


				}
			}

			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				// Handle click within the third rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					cout << "| Click en el segundo rectangulo |" << endl;
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

int inicio() {
	// Aquí podrías colocar un menú de inicio o una pantalla de bienvenida
	// Por ejemplo, podrías mostrar el título del juego y esperar a que el usuario presione una tecla para continuar

	if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		x = evento.mouse.x;
		y = evento.mouse.y;

		al_clear_to_color(negro);


		al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, blanco);
		al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, blanco);
		
		if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT / 2 && y <= VRT * 4.5) {
			al_draw_filled_rectangle(HRZ / 2, VRT / 2, HRZ * 4.5, VRT * 4.5, gris);
			if (evento.mouse.button & 1) {
				cout << "| Click en el primer rectangulo |" << endl;
			 
			}
		}

		if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
			al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
			if (evento.mouse.button & 1) {
				cout << "| Click en el segundo rectangulo |" << endl;
			}
		}
	return 0;
}