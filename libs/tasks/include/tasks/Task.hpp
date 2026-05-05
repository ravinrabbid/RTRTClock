#ifndef TASKS_TASK_HPP_
#define TASKS_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"

#include <array>
#include <string>
#include <string_view>

namespace RTRTClock::Tasks {

class Task {
  private:
    const std::string m_name;
    const UBaseType_t m_priority;

    TaskHandle_t m_handle{nullptr};

    virtual void taskFunc() = 0;

    virtual TaskHandle_t internalCreate() = 0;

  protected:
    static void runTaskFunc(void *params) {
        reinterpret_cast<Task *>(params)->taskFunc();
    };

    Task(std::string_view name, UBaseType_t priority)
        : m_name{name}, m_priority{priority} {};

  public:
    virtual ~Task() { remove(); };

    Task(const Task &) = delete;
    Task(Task &&) = delete;
    Task &operator=(const Task &) = delete;
    Task &operator=(Task &&) = delete;

    [[nodiscard]] std::string getName() const { return m_name; };
    [[nodiscard]] UBaseType_t getPriority() const { return m_priority; };

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
    std::array<StackType_t, STACK_SIZE> m_stack;
    StaticTask_t m_buffer{};

    TaskHandle_t internalCreate() final {
        return xTaskCreateStatic(runTaskFunc, getName().c_str(), STACK_SIZE,
                                 static_cast<void *>(this), getPriority(),
                                 m_stack.data(), &m_buffer);
    };

  protected:
    StaticTask(std::string_view name, UBaseType_t priority)
        : Task{name, priority} {};
};

template <size_t STACK_SIZE> class DynamicTask : public Task {
  private:
    TaskHandle_t internalCreate() final {
        TaskHandle_t handle{nullptr};

        if (xTaskCreate(runTaskFunc, getName().c_str(), STACK_SIZE,
                        static_cast<void *>(this), getPriority(),
                        &handle) == pdPASS) {
            return handle;
        };

        return nullptr;
    };

  protected:
    DynamicTask(std::string_view name, UBaseType_t priority)
        : Task{name, priority} {};
};

} // namespace RTRTClock::Tasks

#endif // TASKS_TASK_HPP_
