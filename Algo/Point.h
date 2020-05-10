#ifndef AVIAHACK_POINT_H
#define AVIAHACK_POINT_H

struct TPoint {
    int32_t x;
    int32_t y;

    TPoint(): x(0), y(0) {}
    TPoint(int32_t x, int32_t y): x(x), y(y) {}
};

#endif //AVIAHACK_POINT_H