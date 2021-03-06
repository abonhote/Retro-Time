// Standard includes
#include <pebble.h>

enum {
  AKEY_NUMBER,
  AKEY_TEXT,
};

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *battery_layer;
TextLayer *charge_layer;
TextLayer *connection_layer;

static char bt_conn[] = "\uf09e"; // \uf071
static char bt_disconn[] = "\uf071";
static char batt_char_full[] = "\uf005";
// static char batt_char_half[] = "\uf123";
static char batt_char_empt[] = "\uf006";

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "\uf004 \uf004 \uf004 \uf004 \uf004 \uf0e7"; // initialize, so we know the length
  static char charge_text[] = "   ";
  
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "Battery Status: %d", charge_state.charge_percent);

  if (charge_state.charge_percent > 80) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorYellow);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_full, batt_char_full, batt_char_full, batt_char_full, batt_char_full);
  } else if (charge_state.charge_percent > 60) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorYellow);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_full, batt_char_full, batt_char_full, batt_char_full, batt_char_empt);
  } else if (charge_state.charge_percent > 40) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorYellow);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_full, batt_char_full, batt_char_full, batt_char_empt, batt_char_empt);
  } else if (charge_state.charge_percent > 20) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorYellow);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_full, batt_char_full, batt_char_empt, batt_char_empt, batt_char_empt);
  } else if (charge_state.charge_percent > 5) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorRed);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_full, batt_char_empt, batt_char_empt, batt_char_empt, batt_char_empt);
  } else {
    #ifdef PBL_COLOR
      text_layer_set_text_color(battery_layer, GColorRed);
    #endif
    snprintf(battery_text,sizeof(battery_text),"%s %s %s %s %s",batt_char_empt, batt_char_empt,batt_char_empt, batt_char_empt, batt_char_empt);
  }
  
  if (charge_state.is_charging) {
    snprintf(charge_text, sizeof(charge_text), "\uf0e7");
    //snprintf(charge_text, sizeof(charge_text), "\uf1e6");
  } else {
    snprintf(charge_text, sizeof(charge_text), "   ");
  }

  text_layer_set_text(battery_layer, battery_text);
  text_layer_set_text(charge_layer, charge_text);
}

// Called once per second
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00 AM"; // Needs to be static because it's used by the system later.
  static char date_text[] = "Mon, Jan 31"; // Needs to be static because it's used by the system later.

  strftime(time_text, sizeof(time_text), "%I:%M %p", tick_time);
  text_layer_set_text(time_layer, time_text);
  
 // strftime(date_text, sizeof(date_text), "%a, %b %d", tick_time);
  strftime(date_text, sizeof(date_text), "%a, %b %d", tick_time);
  text_layer_set_text(date_layer, date_text);

  handle_battery(battery_state_service_peek());
}

void set_signal(bool connected, bool pulse) {
  if (!connected) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(connection_layer, GColorIcterine);
    #endif
    text_layer_set_text(connection_layer,bt_disconn);
   // if (pulse == 1) vibes_double_pulse() ;
  } else {
    #ifdef PBL_COLOR
      text_layer_set_text_color(connection_layer, GColorVividCerulean);
    #endif
    text_layer_set_text(connection_layer,bt_conn);
  }
}


static void handle_bluetooth(bool connected) {
  set_signal(connected, true);
}


void in_received_handler(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "In received called");
  // Check for fields you expect to receive
  Tuple *text_tuple = dict_find(received, AKEY_TEXT);

  // Act on the found fields received
  if (text_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", text_tuple->value->cstring);
  }
}

void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Incoming message dropped");
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
}

// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);
  GFont awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AWESOME_12));
  GFont minecraft_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MINECRAFTIA_24));
  GFont minecraft_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MINECRAFTIA_16));

  // Init Layers
  time_layer = text_layer_create(GRect(0, 63, frame.size.w /* width */, 34/* height */));
  date_layer = text_layer_create(GRect(0, 93, frame.size.w /* width */, 34/* height */));
  connection_layer = text_layer_create(GRect(-2, 2, /* width */ frame.size.w, 34 /* height */));
  battery_layer = text_layer_create(GRect(2, 2, /* width */ frame.size.w, 34 /* height */));
  charge_layer = text_layer_create(GRect(76, 2, /* width */ frame.size.w, 34 /* height */));
  
  // Set Colors, if required
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_background_color(date_layer, GColorClear);
  
  text_layer_set_background_color(connection_layer, GColorClear);
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_background_color(charge_layer, GColorClear);
  
    
  #ifdef PBL_COLOR
    text_layer_set_text_color(connection_layer, GColorVividCerulean);
    text_layer_set_text_color(battery_layer, GColorYellow);
    text_layer_set_text_color(charge_layer, GColorPastelYellow);
  #else
    text_layer_set_text_color(connection_layer, GColorWhite);
    text_layer_set_text_color(battery_layer, GColorWhite);
    text_layer_set_text_color(charge_layer, GColorWhite);
  #endif
    
  // Init the text layer used to show the time
  text_layer_set_font(time_layer, minecraft_font);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text(time_layer, " : ");
  
  // Init the text layer used to show the date  
  text_layer_set_font(date_layer, minecraft_font_small);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_text(date_layer, "Mon, Jan 31");

  // Init the text layer used to show bluetooth connection
  text_layer_set_font(connection_layer, awesome_font);
  text_layer_set_text_alignment(connection_layer, GTextAlignmentRight);
  text_layer_set_text(connection_layer, bt_conn);

  // Init the text layer used to show the battery percentage
  text_layer_set_font(battery_layer, awesome_font);
  text_layer_set_text_alignment(battery_layer, GTextAlignmentLeft);
  text_layer_set_text(battery_layer, "\uf004 \uf004 \uf004 \uf004");
  
  // Init the text layer used to show the battery charge state
  text_layer_set_font(charge_layer, awesome_font);
  text_layer_set_text_alignment(charge_layer, GTextAlignmentLeft);
  text_layer_set_text(charge_layer, "\uf004 \uf004 \uf004 \uf004");

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, SECOND_UNIT);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_minute_tick);
  battery_state_service_subscribe(&handle_battery);
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = &handle_bluetooth
  });
  
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  layer_add_child(root_layer, text_layer_get_layer(time_layer));
  layer_add_child(root_layer, text_layer_get_layer(date_layer));
  layer_add_child(root_layer, text_layer_get_layer(connection_layer));
  layer_add_child(root_layer, text_layer_get_layer(battery_layer));
  layer_add_child(root_layer, text_layer_get_layer(charge_layer));
  
  // Set the triangle if there's no connection - even after returning from somewhere else  
  bool app_connection = connection_service_peek_pebble_app_connection();
  set_signal(app_connection,false);
}

static void do_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  text_layer_destroy(time_layer);
  text_layer_destroy(connection_layer);
  text_layer_destroy(battery_layer);
  text_layer_destroy(charge_layer);
  window_destroy(window);
}


// The main event/run loop for our app
int main(void) {
  do_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  do_deinit();
}