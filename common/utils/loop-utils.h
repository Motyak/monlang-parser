#ifndef LOOP_UTILS_H
#define LOOP_UTILS_H

#define until(x) while(!(x))

/* works with while,for loops, as well as do..while */
/* works with multiple loops sequentially */
/* kind of works with nested loops as well */
inline thread_local bool __first_it;
inline thread_local unsigned long long __nth_it;
#define LOOP __first_it = true; __nth_it = 1;
#define ENDLOOP __first_it = false; __nth_it += 1;

// not tested yet
#define CONTINUE \
    { \
        __first_it = false; __nth_it += 1; \
        continue; \
    }

// not tested yet
#define BREAK \
    { \
        __first_it = false; __nth_it += 1; \
        break; \
    }

#endif // LOOP_UTILS_H
