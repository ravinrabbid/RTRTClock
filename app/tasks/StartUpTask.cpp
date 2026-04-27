#include "StartUpTask.h"

#include "pico/cyw43_arch.h"

namespace RTRTClock::Tasks {

void StartUpTask::taskFunc() {
    if (cyw43_arch_init() != 0) {
        panic("cyw43_arch_init failed");
    }

    for (Task &task : m_tasks) {
        task.create();
    }

    remove();
}

} // namespace RTRTClock::Tasks