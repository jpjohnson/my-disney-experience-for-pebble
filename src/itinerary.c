/*********
* MyDisneyExperience for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE WALT DISNEY COMPANY, WALT DISNEY WORLD,
* OR DISNEY PARKS IN ANY WAY
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* itinerary.c
*/

#include <pebble.h>
#include "pebble-assist.h"
#include "common.h"
#include "itinerary.h"

#define MAX_ITINERARY 20

static Plan itinerary[MAX_ITINERARY];

static int num_plans;
static char itineraryError[128];

static void itinerary_clean_list();
static uint16_t itinerary_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t itinerary_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t itinerary_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t itinerary_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void itinerary_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void itinerary_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void itinerary_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void itinerary_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window *itinerary_window;
static MenuLayer *itinerary_menu_layer;

static GBitmap *fp_icon;

void itinerary_init(void) {
	itinerary_window = window_create();

	fp_icon = gbitmap_create_with_resource(RESOURCE_ID_FP_ICON);

	itinerary_menu_layer = menu_layer_create_fullscreen(itinerary_window);
	menu_layer_set_callbacks(itinerary_menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = itinerary_menu_get_num_sections_callback,
		.get_num_rows = itinerary_menu_get_num_rows_callback,
		.get_header_height = itinerary_menu_get_header_height_callback,
		.get_cell_height = itinerary_menu_get_cell_height_callback,
		.draw_header = itinerary_menu_draw_header_callback,
		.draw_row = itinerary_menu_draw_row_callback,
		.select_click = itinerary_menu_select_callback,
		.select_long_click = itinerary_menu_select_long_callback,
	});

	menu_layer_set_click_config_onto_window(itinerary_menu_layer, itinerary_window);
	menu_layer_add_to_window(itinerary_menu_layer, itinerary_window);
}

void itinerary_show() {
	itinerary_clean_list();
	window_stack_push(itinerary_window, true);
}

void itinerary_destroy(void) {
	gbitmap_destroy(fp_icon);
	layer_remove_from_parent(menu_layer_get_layer(itinerary_menu_layer));
	menu_layer_destroy_safe(itinerary_menu_layer);
	window_destroy_safe(itinerary_window);
}

static void itinerary_clean_list() {
	memset(itinerary, 0x0, sizeof(itinerary));
	num_plans = 0;
	itineraryError[0] = '\0';
	menu_layer_set_selected_index(itinerary_menu_layer, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(itinerary_menu_layer);
}

bool itinerary_is_on_top() {
	return itinerary_window == window_stack_get_top_window();
}

void itinerary_in_received_handler(DictionaryIterator *iter) {
	Tuple *index_tuple = dict_find(iter, PLAN_INDEX);
	Tuple *name_tuple = dict_find(iter, PLAN_NAME);
	Tuple *time_tuple = dict_find(iter, PLAN_TIME);
	Tuple *fp_tuple = dict_find(iter, PLAN_FP);

	if (index_tuple && name_tuple) {
		Plan plan;
		plan.index = index_tuple->value->int16;
		strncpy(plan.name, name_tuple->value->cstring, sizeof(plan.name));
		strncpy(plan.time, time_tuple->value->cstring, sizeof(plan.time));
		plan.fastPass = fp_tuple->value->int16;
		itinerary[plan.index] = plan;
		num_plans++;
		menu_layer_reload_data_and_mark_dirty(itinerary_menu_layer);
	}
}

static uint16_t itinerary_menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
    return 1;
}

static uint16_t itinerary_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return (num_plans) ? num_plans : 1;
}

static int16_t itinerary_menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t itinerary_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void itinerary_menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
    menu_cell_basic_header_draw(ctx, cell_layer, "Itinerary");
}

static void itinerary_menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    if (strlen(itineraryError) != 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Error!", itineraryError, NULL);
    } else if (num_plans == 0) {
        menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
    } else {
		if (itinerary[cell_index->row].fastPass) {
			menu_cell_basic_draw(ctx, cell_layer, itinerary[cell_index->row].name, itinerary[cell_index->row].time, fp_icon);
		} else {
			menu_cell_basic_draw(ctx, cell_layer, itinerary[cell_index->row].name, itinerary[cell_index->row].time, NULL);
    	}
	}
}

static void itinerary_menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    //
}

static void itinerary_menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    //
}
