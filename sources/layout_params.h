#pragma once

#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/size.h"

namespace views {
class View;
}

namespace Layout {

namespace MeasureSpec {
constexpr int MODE_SHIFT = 30;
constexpr int MODE_MASK = 0X3 << MODE_SHIFT;
constexpr int UNSPECIFIED = 0 << MODE_SHIFT;
constexpr int EXACTLY = 1 << MODE_SHIFT;
constexpr int AT_MOST = 2 << MODE_SHIFT;

int Mode(int measure_spec);
int Size(int measure_spec);
int Make(int size, int mode);
}  // namespace MeasureSpec

class LayoutParams{
 public:
  static constexpr int MATCH_PARENT = -1;
  static constexpr int WRAP_CONTENT = -2;

  static LayoutParams* Get(views::View*);
  static void Set(views::View*, LayoutParams*);

  LayoutParams(const class LayoutTypes& types);
  virtual ~LayoutParams() { }

  void SetDimension(int w, int h);

  int width;
  int height;

  gfx::Insets margin;

  int left;
  int top;
  int right;
  int bottom;

  gfx::Size size;
};
}  // namespace Layout
