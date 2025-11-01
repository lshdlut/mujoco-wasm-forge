#include <stddef.h>

#if defined(__EMSCRIPTEN__)
struct dl_phdr_info;

__attribute__((weak))
int dl_iterate_phdr(int (*callback)(struct dl_phdr_info *, size_t, void *), void *data) {
  (void)callback;
  (void)data;
  return 0;
}
#endif

