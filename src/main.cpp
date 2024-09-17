// Display images inside a terminal
// Copyright (C) 2024  JustKidding
//
// This file is part of ueberzugpp.
//
// ueberzugpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ueberzugpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ueberzugpp.  If not, see <https://www.gnu.org/licenses/>.

#include <expected>
#include <print>
#include <memory>

#include "passkey.hpp"

template <class T>
using Result = std::expected<T, std::string_view>;

template <class T>
using ResultP = std::expected<std::unique_ptr<T>, std::string_view>;

class Person
{
  public:
    using PassKey = base::PassKey<Person>;

    static auto create(int age, const std::string &name) -> ResultP<Person>
    {
        constexpr int max_age = 100;
        if (age > max_age) {
            return std::unexpected("bruh");
        }
        auto person = std::make_unique<Person>(PassKey());
        person->name_ = name;
        person->age_ = age;
        return person;
    }

    explicit Person([[maybe_unused]] const PassKey &pass) {}

    // rvo or fail to compile
    Person(Person &&) = delete;
    Person(Person const &) = delete;
    Person &operator=(Person &&) = delete;
    Person &operator=(Person const &) = delete;

    void say_hello() { std::println("Hello, my name is {} and I am {}", name_, age_); }

  private:
    int age_;
    std::string name_;
};

auto main() -> int
{
    auto person = Person::create(100, "Jhon");
    if (person) {
        (*person)->say_hello();
    }

    // Person person2(*person);
    // person2.say_hello();
}
