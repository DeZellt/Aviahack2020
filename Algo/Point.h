#ifndef AVIASHIT_POINT_H
#define AVIASHIT_POINT_H

struct TPoint {
    int32_t x;
    int32_t y;

    TPoint(): x(0), y(0) {}
    TPoint(int32_t x, int32_t y): x(x), y(y) {}
};

#endif //AVIASHIT_POINT_H