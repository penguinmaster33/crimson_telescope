#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>
#define ANCHO_PANTALLA 1024
#define ALTO_PANTALLA 768
#define TAMAÑO_CUADRADO 32
#define VELOCIDAD 5
#define MAX_BLOQUES 100

typedef struct { int x; int y; int ancho; int alto; } Bloque;
typedef struct { int x; int y; int ancho; int alto; int vel_x; int vel_y; } Jugador;
// Función para verificar colisión entre dos rectángulos
bool verificar_colision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) 
{
	return !(x1 + w1 <= x2 || x1 >= x2 + w2 || y1 + h1 <= y2 || y1 >= y2 + h2);
}
// Función para colocar un bloque en el mapa
void colocar_bloque(Bloque bloques[], int *num_bloques, int x, int y) 
{  
	if (*num_bloques < MAX_BLOQUES) { 
		bloques[*num_bloques].x = x;
		bloques[*num_bloques].y = y; 
		bloques[*num_bloques].ancho = TAMAÑO_CUADRADO;
		bloques[*num_bloques].alto = TAMAÑO_CUADRADO; 
		(*num_bloques)++;
	}
}
// Función para dibujar todos los bloques
void dibujar_bloques(Bloque bloques[], int num_bloques) 
{
	for (int i = 0; i < num_bloques; i++) 
	{  
		al_draw_filled_rectangle(bloques[i].x, bloques[i].y, bloques[i].x + bloques[i].ancho, bloques[i].y + bloques[i].alto, al_map_rgb(100, 100, 100)); 
	}
}
// Función para actualizar la posición del jugador con colisiones
void actualizar_jugador(Jugador *jugador, Bloque bloques[], int num_bloques) 
{
	int nueva_x = jugador->x + jugador->vel_x; 
	int nueva_y = jugador->y + jugador->vel_y; 
// Verificar colisiones con bloques     
bool colision = false;
for (int i = 0; i < num_bloques; i++) 
{ 
	if (verificar_colision(nueva_x, nueva_y, jugador->ancho, jugador->alto, bloques[i].x, bloques[i].y, bloques[i].ancho, bloques[i].alto)) 
	{
		colision = true; 
		break;  
	}
} 
	// Verificar límites de pantalla     
if (nueva_x < 0 || nueva_x + jugador->ancho > ANCHO_PANTALLA) 
{
	colision = true; 
} 
if (nueva_y < 0 || nueva_y + jugador->alto > ALTO_PANTALLA)
{ 
	colision = true;
}
	// Actualizar posición si no hay colisión     
if (!colision) 
{
	jugador->x = nueva_x; 
	jugador->y = nueva_y;
} 
	// Resetear velocidad     
jugador->vel_x = 0; jugador->vel_y = 0;

}
int main(void) 
{
	
	
	// Inicializar Allegro     
		if (!al_init()) 
		{
			fprintf(stderr, "Error al inicializar Allegro\n"); 
			return -1;
		} 
		
		// Inicializar add-ons     
		al_init_primitives_addon();
		al_install_keyboard(); 
		
		// Crear ventana ALLEGRO_DISPLAY     
		*display = al_create_display(ANCHO_PANTALLA, ALTO_PANTALLA);
		if (!display)
		{
			fprintf(stderr, "Error al crear la pantalla\n");
			return -1;
		}
		
		al_set_window_title(display, "Cuadrado con Colisiones - Allegro");
		// Crear event queue ALLEGRO_EVENT_QUEUE     
		*queue = al_create_event_queue();
		al_register_event_source(queue, al_get_display_event_source(display));
		al_register_event_source(queue, al_get_keyboard_event_source());  
		
		// Crear timer ALLEGRO_TIMER     
		*timer = al_create_timer(1.0 / 60.0);
		al_register_event_source(queue, al_get_timer_event_source(timer));
		al_start_timer(timer);
		
		// Inicializar jugador     
		
		Jugador jugador;
		jugador.x = ANCHO_PANTALLA / 2 - TAMAÑO_CUADRADO / 2;
		jugador.y = ALTO_PANTALLA / 2 - TAMAÑO_CUADRADO / 2;
		jugador.ancho = TAMAÑO_CUADRADO; 
		jugador.alto = TAMAÑO_CUADRADO;
		jugador.vel_x = 0;
		jugador.vel_y = 0;
		
		// Inicializar bloques 
		Bloque bloques[MAX_BLOQUES]; 
		int num_bloques = 0;
		
		// Crear algunos bloques iniciales     
		// 
		colocar_bloque(bloques, &num_bloques, 100, 100); 
		colocar_bloque(bloques, &num_bloques, 200, 200); 
		colocar_bloque(bloques, &num_bloques, 300, 150);
		bool ejecutando = true;
		bool redraw = false; 
		ALLEGRO_KEYBOARD_STATE estado_teclado; 
		while (ejecutando)  
		{  
			ALLEGRO_EVENT evento; 
			while (al_get_next_event(queue, &evento)) 
			{ 
				switch (evento.type) 
				{  
					case ALLEGRO_EVENT_DISPLAY_CLOSE: ejecutando = false;
					break; 
					case ALLEGRO_EVENT_KEY_DOWN: if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					{
						ejecutando = false;  
					}
					break; 
					case ALLEGRO_EVENT_TIMER: redraw = true;
					break; 
				}
			} 
				
		// Obtener estado del teclado para movimiento continuo         
		al_get_keyboard_state(&estado_teclado); 
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_UP)) 
		{ 
			jugador.vel_y = -VELOCIDAD; 
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_DOWN)) 
		{ 
			jugador.vel_y = VELOCIDAD; 
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT)) 
		{ 
		jugador.vel_x = -VELOCIDAD; 
		}
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) 
		{ 
			jugador.vel_x = VELOCIDAD; 
		}
		// Colocar bloques con clic del mouse (simulado con ESPACIO)         
		if (al_key_down(&estado_teclado, ALLEGRO_KEY_SPACE)) 
		{
			
			// Obtener posición del mouse para una versión más avanzada             
		// // Por ahora, usamos una posición fija             
			static bool espacio_presionado = false; 
			if (!espacio_presionado)
			{
				ALLEGRO_MOUSE_STATE mouse_state; 
				al_get_mouse_state(&mouse_state);
				int bloque_x = (mouse_state.x / TAMAÑO_CUADRADO) * TAMAÑO_CUADRADO;
				int bloque_y = (mouse_state.y / TAMAÑO_CUADRADO) * TAMAÑO_CUADRADO;
				colocar_bloque(bloques, &num_bloques, bloque_x, bloque_y); 
				espacio_presionado = true;
			}
		}  
		else
		{
			printf("error en dibujo de bloque\n");
			}
		// Actualizar posición del jugador         
		actualizar_jugador(&jugador, bloques, num_bloques); 
		// Dibujar         
		if (redraw)
		{
			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Dibujar bloques             
			dibujar_bloques(bloques, num_bloques);
			// Dibujar jugador (cuadrado rojo)             
			al_draw_filled_rectangle(jugador.x, jugador.y, jugador.x + jugador.ancho, jugador.y + jugador.alto, al_map_rgb(255, 0, 0));
			redraw = false; 
		} 
	}  
	
	// Limpiar recursos     
	al_destroy_timer(timer);
	al_destroy_event_queue(queue); 
	al_destroy_display(display); 

	return 0;
}