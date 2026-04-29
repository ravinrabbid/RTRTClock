#ifndef TASKS_TASK_HPP_
#define TASKS_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"

#include <string>

namespace RTRTClock::Tasks {

class Task {
  private:
    TaskHandle_t m_handle{nullptr};

    virtual void taskFunc() = 0;

    virtual TaskHandle_t internalCreate() = 0;

  protected:
    const std::string m_name;
    const UBaseType_t m_priority;

    static void runTaskFunc(void *params) {
        reinterpret_cast<Task *>(params)->taskFunc();
    };

    Task(std::string name, UBaseType_t priority)
        : m_name{std::move(name)}, m_priority{priority} {};

  public:
    virtual ~Task() { remove(); };

    Task(const Task &) = delete;
    Task(Task &&) = delete;
    Task &operator=(const Task &) = delete;
    Task &operator=(Task &&) = delete;

    virtual void create() {
        if (m_handle == nullptr) {
            m_handle = internalCreate();
        }
    };

    virtual void remove() final {
        if (m_handle != nullptr) {
            vTaskDelete(m_handle);
            m_handle = nullptr;
        }
    };
};

template <size_t STACK_SIZE> class StaticTask : public Task {
  private:
    StackType_t m_stack[STACK_SIZE];
    StaticTask_t m_buffer;

    virtual TaskHandle_t internalCreate() final {
        return xTaskCreateStatic(runTaskFunc, m_name.c_str(), STACK_SIZE,
                                 (void *)this, m_priority, m_stack, &m_buffer);
    };

  protected:
    StaticTask(std::string name, UBaseType_t priority)
        : Task{std::move(name), priority} {};
};

template <size_t STACK_SIZE> class DynamicTask : public Task {
  private:
    virtual TaskHandle_t internalCreate() final {
        TaskHandle_t handle{nullptr};

        if (xTaskCreate(runTaskFunc, m_name.c_str(), STACK_SIZE, (void *)this,
                        m_priority, &handle) == pdPASS) {
            return handle;
        };

        return nullptr;
    };

  protected:
    DynamicTask(std::string name, UBaseType_t priority)
        : Task{std::move(name), priority} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_TASK_HPP_
