
#pragma once

#include <atomic>
#include <memory>

class Application
{
  public:
    static auto create() -> std::unique_ptr<Application>;

  private:
    Application() = default;

    std::atomic_flag stop_flag_;
};
