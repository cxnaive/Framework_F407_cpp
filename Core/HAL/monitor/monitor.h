#ifndef H_MONITOR_H
#define H_MONITOR_H
#include <warpper.h>

#include <functional>
#include <vector>

class monitor {
   public:
    typedef std::function<void(void*)> lost_callback;

   private:
    static std::vector<monitor*> monitors;
    lost_callback callback;
    void* callback_identifier;
    int reload_count;

   public:
    int count;
    monitor(lost_callback _callback, int _count, void* _callback_identifier);
    ~monitor();
    void reset();
    bool is_online();
    friend void Monitor_Loop();
};

#endif