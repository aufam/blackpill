#include <apps/app.h>
#include <apps/terminal.h>
#include <delameta/endpoint.h>
#include <etl/async.h>
#include <etl/heap.h>
#include <etl/placeholder.h>

using namespace Project;
using namespace etl::literals;
using delameta::Descriptor;
using delameta::Endpoint;
using delameta::info;
using etl::Err;
using etl::Ok;

[[export]]
Terminal terminal;

static void terminal_work(const char* uri) {
    auto ep = Endpoint::Open(uri).expect(::panic);
    for (;;) {
        auto data = TRY_OR(ep.read(), continue);

        DBG_VAL(info, terminal.routers.size());
        for (auto ch: data) {
            DBG_VAL(info, ch);
        }

        auto sv = etl::string_view(data.data(), data.size());
        for (auto cmd: sv.split("\n")) terminal.execute(ep, cmd);
    }
}

APP(terminal) {
    terminal.route("tasks", &etl::task::resources);

    terminal.route("echo", [](std::string_view sv) {
        return sv;
    });

    terminal.route("heap", etl::placeholder::retval = std::unordered_map<const char*, size_t> {
        {"freeSize", etl::heap::freeSize},
        {"minimumEverFreeSize", etl::heap::minimumEverFreeSize},
        {"totalSize", etl::heap::totalSize},
    });

    terminal.route_async("async_test", []() {
        etl::time::sleep(1s);
    });

    etl::async(&terminal_work, "serial:///usb");
    etl::async(&terminal_work, "serial:///uart1?baud=9600");
}

static void process_execution_result(Descriptor& fd, const Terminal::Result& result, etl::StringView name) {
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

void Terminal::execute(Descriptor& fd, etl::StringView cmd) {
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
