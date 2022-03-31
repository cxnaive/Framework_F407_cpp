// 外设监控
#include "monitor.h"

std::vector<monitor*> monitor::monitors;

monitor::monitor(lost_callback _callback, int _count, void* _callback_identifier) {
    reload_count = _count;
    count = _count;
    callback = _callback;
    callback_identifier = _callback_identifier;
    monitors.push_back(this);
}

monitor::~monitor() {
    for (auto it = monitors.begin(); it != monitors.end(); ++it) {
        if (*it == this) {
            monitors.erase(it);
            return;
        }
    }
}

void monitor::reset() {
    count = reload_count;
}

bool monitor::is_online(){
    return count >= 1;
}

void Monitor_Loop() {
    size_t temp_size = monitor::monitors.size();
    for (size_t i = 0; i < temp_size; i++) {
        monitor* item = monitor::monitors[i];
        item->count--;
        if (item->count <= 0) {
            item->count = 0;
            if (item->callback) {
                item->callback(item->callback_identifier);
            }
        }
    }
}
