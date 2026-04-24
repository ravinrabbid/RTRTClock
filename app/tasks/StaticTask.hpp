#ifndef TASKS_DETAIL_STATICTASK_HPP_
#define TASKS_DETAIL_STATICTASK_HPP_

#include "FreeRTOS.h"
#include "task.h"

#include <string>

namespace Piclock::Tasks {

template <size_t STACK_SIZE> class StaticTask {
  private:
    std::string m_name;
    UBaseType_t m_priority;

    StackType_t m_stack[STACK_SIZE];
    StaticTask_t m_buffer;

    TaskHandle_t m_handle{nullptr};

    static void runTaskFunc(void *params) { reinterpret_cast<StaticTask *>(params)->taskFunc(); };

    virtual void taskFunc() = 0;

  protected:
    StaticTask(std::string name, UBaseType_t priority) : m_name{std::move(name)}, m_priority{priority} {};

  public:
    virtual ~StaticTask() {
        if (m_handle != nullptr) {
            vTaskDelete(m_handle);
        }
    };

    StaticTask(const StaticTask &) = delete;
    StaticTask(StaticTask &&) = delete;
    StaticTask &operator=(const StaticTask &) = delete;
    StaticTask &operator=(StaticTask &&) = delete;

    bool create() {
        if (m_handle == nullptr) {
            xTaskCreateStatic(runTaskFunc, m_name.c_str(), STACK_SIZE, this, m_priority, m_stack, &m_buffer);
        }

        return m_handle == nullptr;
    };

    [[nodiscard]] TaskHandle_t getTaskHandle() const { return m_handle; }
};

} // namespace Piclock::Tasks

#endif // TASKS_DETAIL_STATICTASK_HPP_
