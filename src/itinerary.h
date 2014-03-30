/*********
* MyDisneyExperience for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE WALT DISNEY COMPANY, WALT DISNEY WORLD,
* OR DISNEY PARKS IN ANY WAY
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* itinerary.h
*/

#pragma once

void itinerary_init(void);
void itinerary_show();
void itinerary_destroy(void);
void itinerary_in_received_handler(DictionaryIterator *iter);
bool itinerary_is_on_top();
