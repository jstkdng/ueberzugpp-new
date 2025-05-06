#include <spdlog/spdlog.h>
#include <fmt/format.h>

int main()
{
    int n = 3;
    spdlog::info("&n={:p}", fmt::ptr(&n));
    return 0;
}
