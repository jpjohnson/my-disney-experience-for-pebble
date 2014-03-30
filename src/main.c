/*********
* MyDisneyExperience for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE WALT DISNEY COMPANY, WALT DISNEY WORLD,
* OR DISNEY PARKS IN ANY WAY
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* main.c
*/

#include <pebble.h>
#include "pebble-assist.h"
#include "common.h"
#include "itinerary.h"

static Window *window;
static TextLayer *text_layer;
static BitmapLayer *loading_icon_layer;
static GBitmap *loading_icon;

void out_sent_handler(DictionaryIterator *sent, void *context) {
    // outoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason,
                        void *context) {
    // outgoing message failed
}

void in_received_handler(DictionaryIterator *iter, void *context) {
    if (!itinerary_is_on_top()) {
        window_stack_pop_all(true);
        itinerary_show();
    }
    if (itinerary_is_on_top()) {
        itinerary_in_received_handler(iter);
    }
}

void in_dropped_handler(AppMessageResult reason, void *context) {

}

static void init(void) {
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);
    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());

    window = window_create();
    window_stack_push(window, true);

    Layer *window_layer = window_get_root_layer(window);

    GRect bounds = layer_get_frame(window_layer);

    loading_icon_layer = bitmap_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
    loading_icon = gbitmap_create_with_resource(RESOURCE_ID_LOADING_ICON);\
    bitmap_layer_set_bitmap(loading_icon_layer, loading_icon);
    layer_add_child(window_layer, bitmap_layer_get_layer(loading_icon_layer));

    text_layer = text_layer_create(GRect(0, 115, bounds.size.w, 30));
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text(text_layer, "Loading...");
    layer_add_child(window_layer, text_layer_get_layer(text_layer));

    itinerary_init();
}

static void deinit(void) {
    itinerary_destroy();
    text_layer_destroy(text_layer);
    bitmap_layer_destroy(loading_icon_layer);
    gbitmap_destroy(loading_icon);
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
