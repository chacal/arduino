#include "status_bar.hpp"

#define RSSI_TOP_LEFT_Y    60
#define RSSI_MARK_SIZE      4


void clear_status_bar_area(display &d) {
  d.clear_area(point{0, RSSI_TOP_LEFT_Y}, width{STATUS_BAR_WIDTH}, height{STATUS_BAR_HEIGHT});
}

void draw_rssi_markers(display &d, int8_t rssi) {
  // Draw 5 markers and fill them by every 10dBm RSSI increase starting from -100dBm (e.g. 3 "bars" means that RSSI is -80 - -70dBm)
  for (int i = 0; i < 5; ++i) {
    uint8_t x = i * (RSSI_MARK_SIZE + 1);
    uint8_t y = RSSI_TOP_LEFT_Y;
    auto should_fill = rssi > (-100 + i * 10);
    if(should_fill) {
      d.draw_box(point{x, y}, width{RSSI_MARK_SIZE}, height{RSSI_MARK_SIZE});
    } else {
      d.draw_frame(point{x, y}, width{RSSI_MARK_SIZE}, height{RSSI_MARK_SIZE});
    }
  }
}

void status_bar::update_rssi(int8_t rssi) {
  clear_status_bar_area(d);
  draw_rssi_markers(d, rssi);

  d.render();
}

