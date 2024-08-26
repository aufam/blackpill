#include <apps/app.h>
#include <apps/terminal.h>
#include <etl/async.h>
#include <etl/heap.h>
#include <etl/placeholder.h>

using namespace Project;
using delameta::FileDescriptor;
using etl::Err;
using etl::Ok;

[[export]]
Terminal terminal;

[[export]]
FileDescriptor* fd_debug = nullptr;

static void terminal_work(const char* port, bool as_debug) {
    auto fd = FileDescriptor::Open(FL, port, 0).expect(::panic);
    if (as_debug) fd_debug = &fd;

    for (;;) {
        fd.wait_until_ready();
        auto data = TRY_OR(fd.read(), continue);

        auto sv = etl::string_view(data.data(), data.size());
        for (auto cmd: sv.split("\n")) terminal.execute(fd, cmd);
    }
}

APP(terminal) {
    terminal.route("tasks", &etl::task::resources);

    terminal.route("heap", etl::placeholder::retval = std::unordered_map<const char*, size_t> {
        {"freeSize", etl::heap::freeSize},
        {"minimumEverFreeSize", etl::heap::minimumEverFreeSize},
        {"totalSize", etl::heap::totalSize},
    });

    etl::async(&terminal_work, "/usb", false);
    etl::async(&terminal_work, "/uart1", true);
}

static void process_execution_result(FileDescriptor& fd, const Terminal::Result& result, etl::StringView name) {
    auto name_sv = std::string_view(name.data(), name.len());

    if (result.is_ok()) {
        if (result.unwrap() != "") {
            fd.write(fmt::format("Ok: {}: {}\n", name_sv, result.unwrap()));
        } else {
            fd.write(fmt::format("Ok: {}\n", name_sv));
        }
    } else {
        if (result.unwrap_err() != "") {
            fd.write(fmt::format("Err: {}: {}\n", name_sv, result.unwrap_err()));
        }
        else {
            fd.write(fmt::format("Err: {}\n", name_sv));
        }
    }
}

void Terminal::execute(FileDescriptor& fd, etl::StringView cmd) {
    auto args = cmd.split(" ");
    for (auto &[name, handler]: routers) if (args[0] == name) {
        return process_execution_result(fd, handler(args), name);
    }

    for (auto &[name, handler]: routers_async) if (args[0] == name) {
        auto future = etl::async([&handler, &fd, cmd=std::string(cmd.begin(), cmd.end())]() {
            etl::StringView sv = {cmd.c_str(), cmd.size()};
            auto args = sv.split(" ");
            process_execution_result(fd, handler(args), args[0]);
        });

        if (not future.valid()) {
            return process_execution_result(fd, Err("Task is full"), args[0]);
        } else {
            return;
        }
    }

    process_execution_result(fd, Err("No matching command"), args[0]);
}