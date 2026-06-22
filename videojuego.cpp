/*
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
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>	
	
#define AN_P 1000	
#define AL_P 800

using namespace std;

typedef struct { int x; int y; int ancho; int alto; int vel_x; int vel_y; } Jugador;

// --- Estado y lógica del juego (player, bloques, colisiones) ---
struct Player {
	int x, y;    // posición (centro)
	int w, h;    // tamaño
	float vx, vy; // velocidad
	float speed; // velocidad de movimiento horizontal
	bool onGround;
};

struct Block {
	int x, y, w, h; // rectángulo sólido
	bool solid;
};

struct GameState {
	Player player;
	std::vector<Block> blocks;
	float gravity;
};

// puntero al timer global para que cualquier bucle lo reconozca
ALLEGRO_TIMER* gameTimer = NULL;

// estado global sencillo
GameState gameState;

// Inicializa el estado del juego (player y algunos bloques de ejemplo)
void initGame(GameState &g) {
	// jugador centrado
	g.player.x = AN_P / 2;
	g.player.y = AL_P / 2;
	g.player.w = 40;
	g.player.h = 40;
	g.player.vx = 0;
	g.player.vy = 0;
	g.player.speed = 250.0f; // px/s
	g.player.onGround = false;

	// gravedad
	g.gravity = 800.0f; // px/s^2

	// bloques de ejemplo
	g.blocks.clear();
	g.blocks.push_back(Block{200, 600, 400, 40, true});
	g.blocks.push_back(Block{650, 500, 200, 40, true});
	g.blocks.push_back(Block{50, 350, 150, 40, true});
}

// AABB simple
bool aabbOverlap(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
	return !(ax + aw <= bx || ax >= bx + bw || ay + ah <= by || ay >= by + bh);
}

// Comprueba colisiones y corrige la posición del jugador
void checkCollisions(GameState &g) {
	Player &p = g.player;

	// comprobar con cada bloque
	for (const Block &b : g.blocks) {
		if (!b.solid) continue;

		if (aabbOverlap(p.x - p.w/2, p.y - p.h/2, p.w, p.h, b.x, b.y, b.w, b.h)) {
			// reacción simple: empujar hacia arriba si venimos desde arriba
			// calcular intersección por ejes
			int px1 = p.x - p.w/2;
			int py1 = p.y - p.h/2;
			int px2 = px1 + p.w;
			int py2 = py1 + p.h;

			int bx1 = b.x;
			int by1 = b.y;
			int bx2 = bx1 + b.w;
			int by2 = by1 + b.h;

			int overlapX = std::min(px2, bx2) - std::max(px1, bx1);
			int overlapY = std::min(py2, by2) - std::max(py1, by1);

			if (overlapX < overlapY) {
				// resolver por X
				if (p.x < b.x) {
					p.x -= overlapX;
				} else {
					p.x += overlapX;
				}
				p.vx = 0;
			} else {
				// resolver por Y
				if (p.y < b.y) {
					// colisión por arriba (player toca la parte superior del bloque)
					p.y -= overlapY;
					p.vy = 0;
					p.onGround = true;
				} else {
					// colisión por debajo
					p.y += overlapY;
					p.vy = 0;
				}
			}
		}
	}
}

// Actualiza la lógica del juego. dt en segundos (por ejemplo: 1/60)
void updateGame(GameState &g, double dt) {
	// leer estado del teclado directamente (permite control continuo)
	ALLEGRO_KEYBOARD_STATE kb;
	al_get_keyboard_state(&kb);

	Player &p = g.player;

	// movimiento horizontal
	float move = 0.0f;
	if (al_key_down(&kb, ALLEGRO_KEY_LEFT) || al_key_down(&kb, ALLEGRO_KEY_A)) move -= 1.0f;
	if (al_key_down(&kb, ALLEGRO_KEY_RIGHT) || al_key_down(&kb, ALLEGRO_KEY_D)) move += 1.0f;
	p.vx = move * p.speed;

	// salto (si está en suelo)
	if ((al_key_down(&kb, ALLEGRO_KEY_UP) || al_key_down(&kb, ALLEGRO_KEY_W)) && p.onGround) {
		p.vy = -420.0f; // impulso de salto
		p.onGround = false;
	}

	// aplicar gravedad
	p.vy += g.gravity * (float)dt;

	// integrar posición (primer X luego Y para colisiones separadas)
	p.x += (int)(p.vx * dt);
	checkCollisions(g); // colisiones X

	p.y += (int)(p.vy * dt);
	checkCollisions(g); // colisiones Y
}

// Dibuja el fondo del juego: bloques y jugador
void renderGameBackground(const GameState &g) {
	// fondo oscuro por defecto
	al_clear_to_color(al_map_rgb(20,20,30));

	// dibujar bloques
	for (const Block &b : g.blocks) {
		al_draw_filled_rectangle(b.x, b.y, b.x + b.w, b.y + b.h, al_map_rgb(100, 200, 120));
	}

	// dibujar jugador como rectángulo
	const Player &p = g.player;
	al_draw_filled_rectangle(p.x - p.w/2, p.y - p.h/2, p.x + p.w/2, p.y + p.h/2, al_map_rgb(200,80,80));
}

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
void fondo1(WindowState &ws, ALLEGRO_DISPLAY* ventana, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_FONT* fuente) {
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

		// Si llega un evento timer del juego, actualizamos la lógica
		if (ev.type == ALLEGRO_EVENT_TIMER && ev.timer.source == gameTimer) {
			// dt fijo igual a la frecuencia del timer (1/60)
			updateGame(gameState, 1.0/60.0);
		}

		// Dibujar el fondo del "juego" detrás de la UI del editor
		renderGameBackground(gameState);

		// Dibujar un indicador de "salir" en la esquina
		ALLEGRO_COLOR backButton = al_map_rgb(80, 80, 80);
		al_draw_filled_rectangle(10, 10, 110, 40, backButton);
		al_draw_text(fuente, al_map_rgb(255,255,255), 20, 15, 0, "Salir (click derecho / ESC)");

		// Dibujar la forma seleccionada (encima del fondo del juego)
		ALLEGRO_COLOR sCol = ws.shapeColor;
		int cx = ws.pos_x;
		int cy = ws.pos_y;
		int s = ws.size;

		if (ws.shape == SHAPE_TRIANGLE) {
			if (ws.filled)
				al_draw_filled_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol);
			else
				al_draw_triangle(cx, cy - s, cx - s, cy + s, cx + s, cy + s, sCol, 2.0);

		} else if (ws.shape == SHAPE_CIRCLE) {
			if (ws.filled)
				al_draw_filled_circle(cx, cy, s, sCol);
			else
				al_draw_circle(cx, cy, s, sCol, 2.0);
		} else if (ws.shape == SHAPE_RECTANGLE) {
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
	// Timer dedicado a la lógica del juego (60Hz)
	gameTimer = al_create_timer(1.0 / 60);

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	al_register_event_source(queue, al_get_timer_event_source(segundoTimer));
	al_register_event_source(queue, al_get_timer_event_source(FPS));
	al_register_event_source(queue, al_get_timer_event_source(gameTimer));
	al_start_timer(segundoTimer);
	al_start_timer(FPS);
	al_start_timer(gameTimer);

	// Inicializar estado del juego
	initGame(gameState);

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
	int x = -1, y = -1;
	int HRZ = AN_P/10;
	int VRT = AL_P/10;

	while (true)
	{
		ALLEGRO_EVENT evento;
		al_wait_for_event(queue, &evento);
		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (evento.timer.source == segundoTimer) {
				segundo++;
				cout << "| Seg :" << segundo << " |" << endl;
			} else if (evento.timer.source == gameTimer) {
				// Tick de la lógica del juego (se ejecuta incluso en el menú)
				updateGame(gameState, 1.0/60.0);
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
					
					// Entrar en el bucle modal para esa ventana/editor
					cout << "| Click en el segundo rectangulo |" << endl;
					fondo2(ws, ventana, queue, fuente);

					
				}
			}
			

			/*if (x >= HRZ / 2 && x <= HRZ * 4.5 && y >= VRT * 5.5 && y <= VRT * 9.5) {
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
*/