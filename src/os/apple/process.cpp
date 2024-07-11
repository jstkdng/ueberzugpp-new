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

#include "os/process.hpp"

#include <format>
#include <libproc.h>
#include <sys/types.h>

Process::Process(int pid)
    : pid(pid)
{
    struct proc_bsdshortinfo sproc;
    struct proc_bsdinfo proc;

    int status = proc_pidinfo(pid, PROC_PIDT_SHORTBSDINFO, 0, &sproc, PROC_PIDT_SHORTBSDINFO_SIZE);
    if (status == PROC_PIDT_SHORTBSDINFO_SIZE) {
        ppid = static_cast<int>(sproc.pbsi_ppid);
    }

    status = proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &proc, PROC_PIDTBSDINFO_SIZE);
    if (status == PROC_PIDTBSDINFO_SIZE) {
        tty_nr = static_cast<int>(proc.e_tdev);
        minor_dev = minor(tty_nr);
        pty_path = std::format("/dev/ttys{:03}", minor_dev);
    }
}