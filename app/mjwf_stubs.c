#include <stddef.h>

#if defined(__EMSCRIPTEN__)
struct dl_phdr_info;

// Weak stub for dl_iterate_phdr so that MuJoCo's optional
// backtrace / introspection paths link cleanly under Emscripten.
__attribute__((weak))
int dl_iterate_phdr(int (*callback)(struct dl_phdr_info *, size_t, void *), void *data) {
  (void)callback;
  (void)data;
  return 0;
}
#endif

