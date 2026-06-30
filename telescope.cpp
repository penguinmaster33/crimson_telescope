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

#define AN_P 1500
#define AL_P 1500
#define DIV_HRZ 10
#define	DIV_VRT 10

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
void fondo1(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, ALLEGRO_KEYBOARD_STATE& estado_teclado, creature& jug, int mapa_total[20][20], int fstrow, int fstcol) {
	bool running = true;
	ALLEGRO_EVENT ev;

	//fstrow = primer row
	//fstcol = primer col

	// imprimir mapa una sola vez
	static bool printed = false;
	if (!printed) {
		for (int row = fstrow; row < fstrow+10; ++row)
			for (int col = fstcol; col < fstcol+10; ++col)	
				cout << "casilla r:" << row << " c: " << col << " valor: " << mapa_total[row][col] << endl;
		printed = true;
	}

	/*
		jug.x
	jug.y	........
			........
			........
			........< jug.x+jug.size
				   ^
			jug.y+jug.size
	
	
	
	*/


	// Bucle principal de la pantalla secundaria
	while (running) {
		// Espera un evento (mouse, teclado, timer...)
		al_wait_for_event(queue, &ev);

		// Actualizar el estado del teclado cada iteración
		al_get_keyboard_state(&estado_teclado);

		// Dimensiones de celda
		const int tileW = AN_P / DIV_HRZ;
		const int tileH = AL_P / DIV_VRT;

		// Comprobar colisiones sencillas por celda: calcula las celdas que ocuparía
		auto isFreeAt = [&](int candX, int candY) -> bool {
			int leftCell = candX / tileW;
			int rightCell = (candX + jug.size - 1) / tileW;
			int topCell = candY / tileH;
			int bottomCell = (candY + jug.size - 1) / tileH;

			if (leftCell < 0 || topCell < 0 || rightCell >= DIV_HRZ || bottomCell >= DIV_VRT)
				return false; // fuera de los límites -> no permitido

			for (int fstrow = topCell; fstrow <= bottomCell; ++fstrow)
				for (int fstcol = leftCell; fstcol <= rightCell; ++fstcol)
					if (mapa_total[fstrow][fstcol] == 1)
						return false;
			return true;
		};

		// Mover el jugador por pasos de `velocidad` según flechas, con comprobación de colisión
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_UP)) {
			int ny = jug.pos_y - jug.velocidad;
			if (isFreeAt(jug.pos_x, ny)) jug.pos_y = ny;
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_DOWN)) {
			int ny = jug.pos_y + jug.velocidad;
			if (isFreeAt(jug.pos_x, ny)) jug.pos_y = ny;
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT)) {
			int nx = jug.pos_x - jug.velocidad;
			if (isFreeAt(nx, jug.pos_y)) jug.pos_x = nx;
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) {
			int nx = jug.pos_x + jug.velocidad;
			if (isFreeAt(nx, jug.pos_y)) jug.pos_x = nx;
		}

		// Dibujar fondo y jugador
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_filled_rectangle(
			jug.pos_x,
			jug.pos_y,
			jug.pos_x + jug.size,
			jug.pos_y + jug.size,
			al_map_rgb(255, 0, 0));

		// Dibujar las celdas bloqueadas según la matriz casilla[row][col]
		for (int row = fstrow; row < DIV_VRT+fstrow; ++row) {
			for (int col = fstcol; col < DIV_HRZ+fstcol; ++col) {
				if (mapa_total[row][col] == 1) {
					al_draw_filled_rectangle(col * tileW, row * tileH, col * tileW + tileW, row * tileH + tileH, al_map_rgb(255, 255, 255));
					
				}
			}
		}

		// Botón/salida simple
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 50, 50, 0, "Salir con ESC");
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
			running = false;
			al_clear_to_color(al_map_rgb
			(0, 0, 0));
		}
		/*al_draw_filled_rectangle(0, 0, AN_P / 100, AL_P, al_map_rgb(100, 100, 100));
		al_draw_filled_rectangle(0, 0, AN_P, AL_P / 100, al_map_rgb(100, 100, 100));
		al_draw_filled_rectangle(0, AL_P - AL_P / 100, AN_P, AL_P, al_map_rgb(100, 100, 100));
		al_draw_filled_rectangle(AN_P - AN_P / 100, 0, AN_P, AL_P, al_map_rgb(100, 100, 100));*/

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
	ALLEGRO_TIMER* FPS = al_create_timer(1.0 / 200);

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
	int HRZ = AN_P / DIV_HRZ;
	int VRT = AL_P / DIV_VRT;
	int fstrow, fstcol;
	
	int casilla[10][10]{
	1,	1,	1,	1,	1,	1,  1,  1,	1,	1,
	1,	1,	0,	0,	0,	0,	0,	0,	1,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	0,	0,	1,	1,	1,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	1,	0,	0,	0,
	1,	0,	0,	0,	0,	0,	1,	0,	0,	0,
	1,	0,	0,	1,	1,	1,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	0,	0,	1,	1,	1,	1,	0,	0,	1,
	};

	int mapa_total[20][20]{ 
	1,	1,	1,	1,	1,	1,  1,  1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	0,	0,	0,	0,	2,	0,	1,	1,	0,	0,	0,	0,	2,	0,	0,	9,	9,	1,
	1,	0,	0,	0,	0,	0,	2,	0,	0,	1,	0,	0,	0,	0,	2,	0,	0,	9,	9,	1,
	1,	0,	0,	1,	1,	1,	1,	0,	0,	1,	0,	0,	1,	1,	1,	1,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	1,	0,	0,	1,	0,	0,	1,	0,	0,	0,	1,	0,	0,	1,	
	1,	0,	0,	0,	0,	0,	1,	0,	0,	1,	0,	0,	1,	0,	0,	0,	1,	2,	2,	1,
	1,	0,	0,	1,	2,	2,	1,	0,	0,	1,	0,	0,	1,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	9,	9,	1,	0,	0,	2,	0,	0,	1,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	1,	9,	9,	1,	0,	0,	2,	0,	0,	1,	0,	0,	0,	1,	0,	0,	1,
	1,	2,	2,	1,	1,	1,	1,	0,	0,	1,	1,	1,	1,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	0,	0,	0,	0,	0,	0,	2,	0,	0,	1,	
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	0,	0,	0,	0,	0,	0,	2,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	1,	1,	1,	0,	1,	1,	0,	1,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,  1,	1,	1,	1,  1,	1,	1,	1,
	};
	

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
					jug.pos_x = AN_P / 2 - jug.size / 2;
					jug.pos_y = AL_P / 2 - jug.size / 2;
					jug.x = 0;
					jug.y = 0;
					jug.velocidad = 5;

					fstrow = 0;
					fstcol = 0;

					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el primer rectangulo |" << endl;
					fondo1(ventana, queue, fuente, estado_teclado, jug, mapa_total, fstrow, fstcol);


				}
			}



			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					creature jug;
					jug.size = 100;
					jug.pos_x = AN_P / 2 - jug.size / 2;
					jug.pos_y = AL_P / 2 - jug.size / 2;
					jug.x = 0;
					jug.y = 0;
					jug.velocidad = 5;

					fstrow = 10;
					fstcol = 0;

					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el segundo rectangulo |" << endl;
					fondo1(ventana, queue, fuente, estado_teclado, jug, mapa_total, fstrow, fstcol);
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

	/*
		main(){
		int j=1;
		cambio (j)
		printf("%d", j); // NO IMPRIME j=5
		}

		void cambio(int j) {
		j=5
		}



		main(){
		struct jug
		jug j {};
		cambio(&j)
		printf("%d", j.x); // IMPRIME 5
		}

		void cambio(jug *j)	{
		*j.x=5
		}
		// puedes cambiar los valores dentro de un arreglo incluso sin usar & o *. arreglos YA SON PUNTEROS
		*/