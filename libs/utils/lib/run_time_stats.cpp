#include "FreeRTOS.h"

#include "utils/run_time_stats.h"

#ifdef CONFIG_DEBUG_PRINT_RUNTIME_STATS

#include "task.h"

#include "hardware/timer.h"

#include <array>
#include <cstdio>
#include <format>
#include <string_view>
#include <vector>

namespace RTRTClock::Utils::RunTimeStats {

namespace {

constexpr size_t TASK_STACK_SIZE = configMINIMAL_STACK_SIZE + 256;
constexpr size_t STATS_LINE_LEN = configMAX_TASK_NAME_LEN + 1 + 41;

struct ValueWithUnit {
    uint32_t value;
    std::string_view unit;
};

ValueWithUnit convert_run_time(configRUN_TIME_COUNTER_TYPE raw_time) {
    const static std::string_view unit_us{"us"};
    const static std::string_view unit_ms{"ms"};
    const static std::string_view unit_sec{"sec"};
    const static std::string_view unit_min{"min"};
    const static std::string_view unit_h{"h"};
    const static std::string_view unit_d{"d"};

    if (raw_time < 10'000) { // <10ms
        return {.value = static_cast<uint32_t>(raw_time), .unit = unit_us};
    }
    raw_time /= 1000;
    if (raw_time < 60'000) { // <60sec
        return {.value = static_cast<uint32_t>(raw_time), .unit = unit_ms};
    }
    raw_time /= 1000;
    if (raw_time < 3'600) { // <60min
        return {.value = static_cast<uint32_t>(raw_time), .unit = unit_sec};
    }
    raw_time /= 60;
    if (raw_time < 1'440) { // <24h
        return {.value = static_cast<uint32_t>(raw_time), .unit = unit_min};
    }
    raw_time /= 60;
    if (raw_time < 168) { // <7d
        return {.value = static_cast<uint32_t>(raw_time), .unit = unit_h};
    }
    return {.value = static_cast<uint32_t>(raw_time / 24), .unit = unit_d};
}

ValueWithUnit convert_bytes(UBaseType_t raw_bytes) {
    const static std::string_view unit_b{"B"};
    const static std::string_view unit_kib{"KiB"};
    const static std::string_view unit_mib{"MiB"};

    if (raw_bytes < 4'096) {
        return {.value = static_cast<uint32_t>(raw_bytes), .unit = unit_b};
    }
    raw_bytes >>= 10;
    if (raw_bytes < 4'096) {
        return {.value = static_cast<uint32_t>(raw_bytes), .unit = unit_kib};
    }
    return {.value = static_cast<uint32_t>(raw_bytes >> 10), .unit = unit_mib};
}

void print_stats_task_func(void *params) {
    (void)params;

    static std::array<char, STATS_LINE_LEN + 1> system_stats_buffer{};
    static std::array<char, STATS_LINE_LEN + 1> task_stats_buffer{};
    static std::array<char, STATS_LINE_LEN + 1> line_buffer{};
    static std::array<char, STATS_LINE_LEN + 1> head_buffer{};

    std::format_to_n(head_buffer.data(), head_buffer.size(),
                     "{:>4s} {:<{}s}{:>6s}{:>6s}{:>8s}{:>9s}{:>6s}\0", "ID",
                     "Name", configMAX_TASK_NAME_LEN + 1, "CPrio", "BPrio",
                     "HighWM", "Time", "Time%");

    std::format_to_n(line_buffer.data(), line_buffer.size(), "{:-<{}s}\0", "",
                     STATS_LINE_LEN - 1);

    while (true) {
        auto task_count = uxTaskGetNumberOfTasks();
        std::vector<TaskStatus_t> task_statuses(task_count);
        configRUN_TIME_COUNTER_TYPE total_run_time = 0;

        task_count = uxTaskGetSystemState(
            task_statuses.data(), task_statuses.size(), &total_run_time);
        task_statuses.resize(task_count);

        const auto uptime = convert_run_time(total_run_time);
        const auto heap_free = convert_bytes(xPortGetFreeHeapSize());
        const auto heap_wm = convert_bytes(xPortGetMinimumEverFreeHeapSize());
        std::format_to_n(system_stats_buffer.data(), system_stats_buffer.size(),
                         "Uptime: {:>5d}{:>3.3s}    HeapFree: {:>5d}{:<3.3s}   "
                         "HeapWM: {:>5d}{:<3.3s}\0",
                         uptime.value, uptime.unit, heap_free.value,
                         heap_free.unit, heap_wm.value, heap_wm.unit);

        printf("\n");
        printf("%s\n", line_buffer.data());
        printf("%s\n", system_stats_buffer.data());
        printf("%s\n", line_buffer.data());
        printf("%s\n", head_buffer.data());
        printf("%s\n", line_buffer.data());

        total_run_time /= 100;
        if (total_run_time <= 0) {
            continue;
        }

        for (const auto &task : task_statuses) {
            const auto run_time = convert_run_time(task.ulRunTimeCounter);
            const auto run_time_percent =
                task.ulRunTimeCounter / total_run_time;
            const auto high_wm =
                convert_bytes(uxTaskGetStackHighWaterMark(task.xHandle) *
                              sizeof(portSTACK_TYPE));

            std::format_to_n(
                task_stats_buffer.data(), task_stats_buffer.size(),
                "{:>4d} "
                "{:{}.{}s}{:>6d}{:>6d}{:>5d}{:<3.3s}{:>6d}{:<3.3s}{:>5d}%\0",
                task.xTaskNumber, task.pcTaskName, configMAX_TASK_NAME_LEN + 1,
                configMAX_TASK_NAME_LEN + 1, task.uxCurrentPriority,
                task.uxBasePriority, high_wm.value, high_wm.unit,
                run_time.value, run_time.unit, run_time_percent);

            printf("%s\n", task_stats_buffer.data());
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

} // namespace

void print_stats_task_create() {
    static std::array<StackType_t, TASK_STACK_SIZE> print_stats_task_stack;
    static StaticTask_t print_stats_task_buffer;

    xTaskCreateStatic(print_stats_task_func, "Print Stats", TASK_STACK_SIZE,
                      nullptr, tskIDLE_PRIORITY + 1UL,
                      print_stats_task_stack.data(), &print_stats_task_buffer);
}

} // namespace RTRTClock::Utils::RunTimeStats

uint64_t get_run_time_counter_value() { return time_us_64(); }

#else // CONFIG_DEBUG_PRINT_RUNTIME_STATS

namespace RTRTClock::Utils::RunTimeStats {

void print_stats_task_create() {}

} // namespace RTRTClock::Utils::RunTimeStats

#endif // CONFIG_DEBUG_PRINT_RUNTIME_STATS