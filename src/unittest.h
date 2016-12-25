#ifndef UNITTEST_H
#define UNITTEST_H
#include <assert.h>

#define EXCEPT_TRUE(x) \
    do {                                \
        assert((x));                   \
    }while(0)

#define EXCEPT_FALSE(x) \
    do {                                \
        assert(!(x));                   \
    }while(0)

#define EXCEPT_EQ(a, b) \
    do {                                \
        assert((a) == (b));          \
    }while(0)

#define EXCEPT_NE(a, b) \
    do {                                \
        assert((a) != (b));          \
    }while(0)

#endif // UNITTEST_H

