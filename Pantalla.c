#include <allegro5/allegro.h>

int main() 
{
   if (!al_init()) 
   {
       return -1;
   }

   ALLEGRO_DISPLAY *display = al_create_display(640, 480);
   if (!display) 
   {
       al_uninstall_system();
       return -1;
   }

   al_set_window_title(display, "Mi Ventana Allegro");

   bool running = true;
   ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
   al_register_event_source(event_queue, al_get_display_event_source(display));

   while (running) 
   {
       ALLEGRO_EVENT event;
       al_wait_for_event(event_queue, &event);
       if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
       {
           running = false;
       }
   }

   al_destroy_display(display);
   al_uninstall_system();
   return 0;
}