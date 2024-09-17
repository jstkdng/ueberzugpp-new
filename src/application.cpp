#include "application.hpp"

//Application::Application() = default;

auto Application::create() -> std::unique_ptr<Application>
{
    return std::make_unique<Application>();
}
