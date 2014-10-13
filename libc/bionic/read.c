#include <anemu.h>

extern ssize_t __read(int, void *, size_t);

ssize_t read(int fd, void *buf, size_t count) {
#ifdef WITH_TAINT_TRACKING
    return emu_trampoline_read(fd, buf, count);
#else
    return __read(fd, buf, count);
#endif
}
