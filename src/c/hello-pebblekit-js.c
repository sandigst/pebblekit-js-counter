#include <pebble.h>

static Window *s_window = NULL;
static TextLayer *s_text_layer = NULL;
static AppSync s_sync;
#define SYNC_BUFFER_SIZE 64
static uint8_t s_sync_buffer[SYNC_BUFFER_SIZE];

static uint32_t s_count = 0;

void sendMessage(Tuplet *values, uint8_t count)
{
  app_sync_set(&s_sync, values, count);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // Reset
  Tuplet values[] = {
      TupletInteger(MESSAGE_KEY_Reset, s_count)};
  app_sync_set(&s_sync, values, 1);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // Count up
  Tuplet values[] = {
      TupletInteger(MESSAGE_KEY_CountUp, s_count)};
  app_sync_set(&s_sync, values, 1);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // Count down
  Tuplet values[] = {
      TupletInteger(MESSAGE_KEY_CountDown, s_count)};
  app_sync_set(&s_sync, values, 1);
}

static void click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void sync_changed_callback(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context)
{
  if (key == MESSAGE_KEY_Count)
  {
    s_count = new_tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received count: %ld", s_count);

    static char s_count_buffer[32];
    snprintf(s_count_buffer, sizeof(s_count_buffer), "Count: %ld", new_tuple->value->int32);
    text_layer_set_text(s_text_layer, s_count_buffer);
    return;
  }
}

void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App message error: %d, dict error: %d", app_message_error, dict_error);
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "Sync...");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void window_unload(Window *window)
{
  text_layer_destroy(s_text_layer);
}

static void init(void)
{
  s_window = window_create();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  const bool animated = true;
  window_stack_push(s_window, animated);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Initialized app, pushed window: %p", s_window);

  app_message_open(1024, 1024);

  Tuplet initial_values[] = {
      TupletInteger(MESSAGE_KEY_Count, (int32_t)0),
      TupletInteger(MESSAGE_KEY_CountUp, (uint8_t)0),
      TupletInteger(MESSAGE_KEY_CountDown, (uint8_t)0),
      TupletInteger(MESSAGE_KEY_Reset, (uint8_t)0),
  };

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Allocated message buffer, tuplets: %d, size: %d, result: %d", ARRAY_LENGTH(initial_values), SYNC_BUFFER_SIZE, s_sync_buffer == NULL ? 0 : 1);
  app_sync_init(&s_sync, s_sync_buffer, SYNC_BUFFER_SIZE, initial_values, ARRAY_LENGTH(initial_values), sync_changed_callback, sync_error_callback, NULL);
}

static void deinit(void)
{
  app_sync_deinit(&s_sync);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Free message buffer");

  window_destroy(s_window);
}

int main(void)
{
  init();

  app_event_loop();

  deinit();
}
