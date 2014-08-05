// This is a translate of the original code: https://github.com/MilesOkeefe/magic8

#include <pebble.h>
#include <time.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *black_text_layer;
static AppTimer *timer;

/* Posibles Respuestas */
static char *answers[20] = {"En mi opinión, sí",
		     "Es cierto",
		     "Claro que sí",
		     "Sin ninguna duda",
		     "Probablemente",
		     "Buen pronóstico",
		     "Claro que sí",
		     "Todo apunta a que sí",
		     "¡Sin ninguna duda!",
		     "Sí",
		     "Debes confiar en ello",
		     "No se...no se...vuelve a intentarlo",
		     "No quieras saberlo",
		     "No estás preparado para la respuesta",
		     "No cuentes con ello",
		     "Ni de coña",
		     "La respuesta es: NO",
		     "Podría ser... pero NO",
		     "Lo dudo mucho",
		     "Evidentemente"};

/* random() related parts are from https://github.com/ad1217/pebbloids/blob/master/src/Pebbloids.c */
long seed;
static int random(int max){
    seed=(((seed*214013L+2531011L) >> 16) & 32767);
    return ((seed%max));
}
int count;
int max;
int reading;
static void timer_callback(void *data) {
	if(count < max){
		if(count%2 == 1){
			reading = random(20);
			text_layer_set_text(text_layer, answers[reading]);
		}else{
			text_layer_set_text(text_layer, "--------");
		}
		timer = app_timer_register(100 /* milliseconds */, timer_callback, NULL);
		count++;
	}else{
		reading = random(20);
		text_layer_set_text(text_layer, answers[reading]);
	}
}

void update_message(void){
	count = 0;
	max = random(5) + 2;
	timer = app_timer_register(300 /* milliseconds */, timer_callback, NULL);
	light_enable(true);//backlight on black background looks blue like a magic 8 ball
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_message();
}
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}
void accel_tap_handler(AccelAxisType axis, int32_t direction){
  update_message();
}
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 70 }, .size = { bounds.size.w , bounds.size.h} });
  text_layer_set_background_color(text_layer, GColorBlack);
  window_set_background_color(window, GColorBlack);
  window_set_background_color(window, GColorBlack);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  update_message();
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}
static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
	    .load = window_load,
	    .unload = window_unload
	  });
	const bool animated = true;
    window_set_fullscreen(window, true);
	window_stack_push(window, animated);

	/* Random seed */
	time_t timval=time(NULL);
	struct tm tims=*(localtime(&timval));
	seed=tims.tm_hour*3600+tims.tm_min*60+tims.tm_sec;
	accel_tap_service_subscribe(&accel_tap_handler);
}
static void deinit(void) {
	accel_tap_service_unsubscribe();
	window_destroy(window);
}

int main(void) {
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Inicializado, ventana: %p", window);
	app_event_loop();
	deinit();
}