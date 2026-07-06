#if defined(__linux__) || defined(__sun) || defined(__APPLE__) || \
    defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
    defined(__GNU__) || defined(__hurd__) || defined(__DragonFly__)
  #define unix_like 1
#endif
