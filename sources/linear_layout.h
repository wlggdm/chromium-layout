#ifndef _LINEAR_LAYOUT_H_
#define _LINEAR_LAYOUT_H_

#include <memory>
#include <vector>

#include "ui/gfx/geometry/insets.h"

#include "layout_master.h"

namespace Layout {

class LayoutParams;

class LinearLayout : public LayoutMaster {
 public:
  LinearLayout();
  ~LinearLayout() override;

  void SetOrientation(int orientation);
  void SetGravity(int gravity);

  // LayoutMaster
  void Measure(views::View* v,
               int width_measure_spec,
               int height_measure_spec) override;

  class LinearLayoutParams;

 private:
  void MeasureVertical(views::View* v,
                       int width_measure_spec,
                       int height_measure_spec);

  void MeasureHorizontal(views::View* v,
                         int width_measure_spec,
                         int height_measure_spec);

  void MeasureChild(views::View* child,
                    int width_measure_spec,
                    int height_measure_spec);

  int GetChildMeasureSpec(int child_size,
                          int start_padding,
                          int end_padding,
                          int my_size);

  int orientation_;
  int gravity_;

  gfx::Insets padding_;
};

}  // namespace Layout

#endif
