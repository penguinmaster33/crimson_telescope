#include <iostream>// no es c sacar 
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include <vector>
#include <Windows.h>

#define AN_P 1000
#define AL_P 1000
#define DIV_HRZ 10
#define	DIV_VRT 10

// Tamaño del mapa en celdas y píxeles (mapa_total es 20x20)
#define MAP_COLS 20
#define MAP_ROWS 20
const int MAP_W = (AN_P / DIV_HRZ) * MAP_COLS;
const int MAP_H = (AL_P / DIV_VRT) * MAP_ROWS;

using namespace std; //no es c

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
// Helper: alterna (toggle) componente conectada de puertas (2<->3) usando BFS 4-direccional
void toggleDoorComponent(int mapa[20][20], int startR, int startC) {
	int orig = mapa[startR][startC];
	if (orig != 2 && orig != 3) return; // no es puerta
	int target = (orig == 2) ? 3 : 2; // 2->3 (abrir), 3->2 (cerrar)

	// Cambiar a c
	std::vector<std::pair<int,int>> stack; //// No es C
	stack.push_back({startR,startC}); /// tamopo c
	mapa[startR][startC] = target;
	while (!stack.empty()) { //  stack.empty no es c
		auto p = stack.back(); stack.pop_back(); /// stack.back()tampoco c 
		int r = p.first, c = p.second;
		const int dr[4] = {-1,1,0,0};
		const int dc[4] = {0,0,-1,1};
		for (int i=0;i<4;++i) {
			int nr = r + dr[i];
			int nc = c + dc[i];
			if (nr<0||nr>=20||nc<0||nc>=20) continue;
			if (mapa[nr][nc] == orig) {
				mapa[nr][nc] = target;
				stack.push_back({nr,nc});
			}
		}
	}
}

void fondo1(ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente, ALLEGRO_KEYBOARD_STATE& estado_teclado, creature& jug /*asi no se hacen las referencias */ , int mapa_total[20][20], int fstrow, int fstcol) {
	bool running = true;
	ALLEGRO_EVENT ev;

	//fstrow = primer row
	//fstcol = primer col

	// imprimir mapa una sola vez
	static bool printed = false;
	if (!printed) {
		for (int row = fstrow; row < fstrow + 10; ++row)
			for (int col = fstcol; col < fstcol + 10; ++col)
				cout << "casilla r:" << row << " c: " << col << " valor: " << mapa_total[row][col] << endl; //usar printf
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
		auto isFreeAt = [&](int candX, int candY) -> bool { // nope
			int leftCell = candX / tileW;
			int rightCell = (candX + jug.size - 1) / tileW;
			int topCell = candY / tileH;
			int bottomCell = (candY + jug.size - 1) / tileH;

			// Comprobar límites respecto al mapa total (MAP_COLS x MAP_ROWS)
			if (leftCell < 0 || topCell < 0 || rightCell >= MAP_COLS || bottomCell >= MAP_ROWS)
				return false; // fuera de los límites -> no permitido

			for (int r = topCell; r <= bottomCell; ++r)
				for (int c = leftCell; c <= rightCell; ++c)
					// Bloquean: muros (1) y puertas cerradas (2)
					if (mapa_total[r][c] == 1 || mapa_total[r][c] == 2)
						return false;
			return true;
		};

		// Mover el jugador por pasos de `velocidad` según flechas, con comprobación de colisión
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_UP)) {
			int ny = jug.pos_y - jug.velocidad;
			if (isFreeAt(jug.pos_x, ny)) jug.pos_y = ny;  // deberia ser jug->pos_x no jug.pos_x
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

		// Limitar la posición del jugador al tamaño total del mapa en píxeles
		if (jug.pos_x < 0) jug.pos_x = 0;
		if (jug.pos_y < 0) jug.pos_y = 0;
		if (jug.pos_x + jug.size > MAP_W) jug.pos_x = MAP_W - jug.size;
		if (jug.pos_y + jug.size > MAP_H) jug.pos_y = MAP_H - jug.size;

		// Comprobar si el jugador ha pisado una celda con valor 9 (perder)
		auto playerOnLosingCell = [&]() -> bool { //????
			int leftCell = jug.pos_x / tileW;
			int rightCell = (jug.pos_x + jug.size - 1) / tileW;
			int topCell = jug.pos_y / tileH;
			int bottomCell = (jug.pos_y + jug.size - 1) / tileH;
			for (int r = topCell; r <= bottomCell; ++r)
				for (int c = leftCell; c <= rightCell; ++c)
					if (r>=0 && r<MAP_ROWS && c>=0 && c<MAP_COLS && mapa_total[r][c] == 9)
						return true;
			return false;
		};

		if (playerOnLosingCell()) {
			// Mostrar pantalla de "has perdido" y salir de la pantalla secundaria al pulsar una tecla
			al_clear_to_color(al_map_rgb(0,0,0));
			int rectW = AN_P/2; int rectH = AL_P/4;
			int rx = (AN_P - rectW)/2; int ry = (AL_P - rectH)/2;
			al_draw_filled_rectangle(rx, ry, rx+rectW, ry+rectH, al_map_rgb(50,50,50));
			al_draw_rectangle(rx, ry, rx+rectW, ry+rectH, al_map_rgb(200,0,0), 3);
			al_draw_text(fuente, al_map_rgb(255,255,255), rx + 20, ry + 20, 0, "HAS PERDIDO");
			al_flip_display();

			// esperar tecla para salir
			ALLEGRO_EVENT ev2;
			while (true) {
				al_wait_for_event(queue, &ev2);
				if (ev2.type == ALLEGRO_EVENT_KEY_DOWN) break;
			}
			running = false;
			break;
		}

		// Dibujar fondo y jugador (posiciones relativas a la cámara: fstrow/fstcol)
		al_clear_to_color(al_map_rgb(0, 0, 0));
		// Cámara en píxeles
		int camX = fstcol * tileW;
		int camY = fstrow * tileH;
		al_draw_filled_rectangle(
			jug.pos_x - camX,
			jug.pos_y - camY,
			jug.pos_x + jug.size - camX,
			jug.pos_y + jug.size - camY,
			al_map_rgb(255, 0, 0));

		// Dibujar las celdas según la matriz mapa_total (viewport)
		for (int row = fstrow; row < DIV_VRT + fstrow; ++row) {
			for (int col = fstcol; col < DIV_HRZ + fstcol; ++col) {
				int drawX = (col - fstcol) * tileW;
				int drawY = (row - fstrow) * tileH;
				int val = mapa_total[row][col];
				if (val == 1) {
					// muro
					al_draw_filled_rectangle(drawX, drawY, drawX + tileW, drawY + tileH, al_map_rgb(255, 255, 255));
				} else if (val == 2) {
					// puerta cerrada (marrón)
					al_draw_filled_rectangle(drawX, drawY, drawX + tileW, drawY + tileH, al_map_rgb(150, 75, 0));
				} else if (val == 3) {
					// puerta abierta (verde claro)
					al_draw_filled_rectangle(drawX, drawY, drawX + tileW, drawY + tileH, al_map_rgb(150, 200, 150));
				}
			}
		}

		// Botón/salida simple
		al_draw_text(fuente, al_map_rgb(255, 255, 255), 50, 50, 0, "Salir con ESC");
		// Manejar teclas de acción: ESC para salir, E para alternar puertas bajo el jugador (si está en viewport)
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				running = false;
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_E) {
				// Intentar alternar la puerta bajo el jugador; si no hay, probar celdas adyacentes
				int centerX = jug.pos_x + jug.size / 2;
				int centerY = jug.pos_y + jug.size / 2;
				int centerCol = centerX / tileW;
				int centerRow = centerY / tileH;
				int targetR = -1, targetC = -1;
				// Lista de celdas a probar: centro primero, luego 4-adyacentes
				const int drs[5] = {0, -1, 1, 0, 0};
				const int dcs[5] = {0, 0, 0, -1, 1};
				for (int i = 0; i < 5; ++i) {
					int r = centerRow + drs[i];
					int c = centerCol + dcs[i];
					if (r < fstrow || r >= fstrow + DIV_VRT || c < fstcol || c >= fstcol + DIV_HRZ) continue; // fuera viewport
					if (r < 0 || r >= 20 || c < 0 || c >= 20) continue;
					int val = mapa_total[r][c];
					if (val == 2 || val == 3) { targetR = r; targetC = c; break; }
				}
				if (targetR != -1) {
					toggleDoorComponent(mapa_total, targetR, targetC);
					cout << "Toggle puertas en r=" << targetR << " c=" << targetC << endl;  //prinntf
				}
			}
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

	int casilla[10][10]{  /// [][] = {}
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

	int mapa_total[20][20]{ /// [][] = {}
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


	// Inicializar jugador persistente (busca primera celda libre '0')
	creature player;
	player.size = 50;
	player.velocidad = 5;
	// buscar primera celda 0
	bool found = false;
	for (int r = 0; r < MAP_ROWS && !found; ++r) {
		for (int c = 0; c < MAP_COLS; ++c) {
			if (mapa_total[r][c] == 0) {
				player.pos_x = c * HRZ + (HRZ - player.size) / 2;
				player.pos_y = r * VRT + (VRT - player.size) / 2;
				found = true; break;
			}
		}
	}
	if (!found) { player.pos_x = 0; player.pos_y = 0; }

	while (true)
	{
		ALLEGRO_EVENT evento;
		al_wait_for_event(queue, &evento);
		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (evento.timer.source == segundoTimer) {
				segundo++;
				cout << "| Seg :" << segundo << " |" << endl;  //printf
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
					// Mostrar cuadrante superior izquierdo sin alterar la posición del jugador
					fstrow = 0; fstcol = 0;
					cout << "| Click en el primer rectangulo |" << endl; /// printf 
					fondo1(ventana, queue, fuente, estado_teclado, player, mapa_total, fstrow, fstcol);
				}
			}



			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT / 2 && y <= VRT * 4.5) {
				al_draw_filled_rectangle(HRZ * 5.5, VRT / 2, HRZ * 9.5, VRT * 4.5, gris);
				if (evento.mouse.button & 1) {
					// Mostrar cuadrante superior derecho sin alterar la posición del jugador
					fstrow = 0; fstcol = 10;
					cout << "| Click en el segundo rectangulo |" << endl; // usar printf
					fondo1(ventana, queue, fuente, estado_teclado, player, mapa_total, fstrow, fstcol);
				}
			}


			if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the second rectangle
				al_draw_filled_rectangle(HRZ / 2, VRT * 5.5, HRZ * 4.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {
					// Mostrar cuadrante inferior izquierdo sin alterar la posición del jugador
					fstrow = 10; fstcol = 0;
					cout << "| Click en el tercer rectangulo |" << endl; ///usar printf
					fondo1(ventana, queue, fuente, estado_teclado, player, mapa_total, fstrow, fstcol);
				}
			}


			if (x >= HRZ * 5.5 && x <= HRZ * 9.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
				// Handle click within the fourth rectangle
				al_draw_filled_rectangle(HRZ * 5.5, VRT * 5.5, HRZ * 9.5, VRT * 9.5, gris);
				if (evento.mouse.button & 1) {
					// Mostrar cuadrante inferior derecho sin alterar la posición del jugador
					fstrow = 10; fstcol = 10;
					cout << "| Click en el cuarto rectangulo |" << endl;
					fondo1(ventana, queue, fuente, estado_teclado, player, mapa_total, fstrow, fstcol);
				}
			}

		}


		// FIGURAS:


		//al_draw_text(fuente, al_map_rgb(255, 255, 255), 200, 200, 0, ("Segundo: " + to_string(segundo).c_str())); // REPARAR FUENTES :((


		al_flip_display();
	}
	return 0;
}
