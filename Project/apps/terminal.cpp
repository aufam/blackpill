#include "main.hpp"
#include "etl/keywords.h"

using etl::Result;
using etl::Ok;
using etl::Err;

APP(terminal) {
    terminal.add("echo", [](etl::StringView text) -> const char* {
        return f("%.*s", text.len(), text.data());
    });

    terminal.add_async("test_async", [](int num) -> Result<const char*, const char*> {
        await | etl::task::sleep(1s);
        if (num > 0) {
            return Ok(f("%d is positive", num));
        } else if (num < 0) {
            return Err(f("%d is negative", num));
        } else {
            return Err(f("%d is zero", num));
        }
    });
}
