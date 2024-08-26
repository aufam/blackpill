#include <fmt/format.h>
#include <delameta/debug.h>
#include <delameta/file_descriptor.h>

using namespace Project;
using delameta::FileDescriptor;

extern FileDescriptor* fd_debug;

static const char* get_file_name_only(const char* file) {
    const char* filename = strrchr((const char*)file, '/');
    if (!filename) {
        filename = strrchr((const char*)file, '\\');  // check for windows-style paths
    }
    return filename ? filename + 1 : (const char*)file; 
}

[[export, override_weak]]
void delameta::info(const char* file, int line, const std::string& msg) {
    if (fd_debug) fd_debug->write(fmt::format("{}:{} INFO: {}\n", get_file_name_only(file), line, msg));
}

[[export, override_weak]]
void delameta::warning(const char* file, int line, const std::string& msg) {
    if (fd_debug) fd_debug->write(fmt::format("{}:{} WARNING: {}\n", get_file_name_only(file), line, msg));
}
