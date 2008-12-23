#ifndef DEPRECATED
#if __GNUC__>3
#define DEPRECATED __attribute__((deprecated))
#define PRIVATE __attribute__((deprecated))
#else
#define DEPRECATED
#define PRIVATE
#endif
#endif
