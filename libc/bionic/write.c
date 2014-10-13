#include <anemu.h>

extern ssize_t __write(int, void *, size_t);

ssize_t write(int fd, void *buf, size_t count) {
#ifdef WITH_TAINT_TRACKING
    return emu_trampoline_write(fd, buf, count);
#else
    return __write(fd, buf, count);
#endif
}
