#ifndef VEC4_H
#define VEC4_H

struct Vec4 {
  float x=0, y=0, z=0, w=0;
  constexpr Vec4() = default;
  constexpr Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};

#endif
