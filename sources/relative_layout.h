#ifndef _RELATIVE_LAYOUT_H_
#define _RELATIVE_LAYOUT_H_

#include <memory>
#include <vector>

#include "ui/gfx/geometry/insets.h"

#include "layout_master.h"

namespace Layout {

class LayoutParams;

class RelativeLayout : public LayoutMaster {
 public:
  RelativeLayout();
  ~RelativeLayout() override;

  // LayoutMaster
  void Measure(views::View* v,
               int width_measure_spec,
               int height_measure_spec) override;

  class RelativeLayoutParams;

 private:
  void SortChildren(views::View* host);

  void ApplyHorizontalSizeRules(RelativeLayoutParams* params, int my_width);
  void ApplyVerticalSizeRules(RelativeLayoutParams* params, int my_height);

  void MeasureChildHorizontal(views::View* child,
                              RelativeLayoutParams* params,
                              int my_width,
                              int my_height);

  void MeasureChild(views::View* child,
                    int width_measure_spec,
                    int height_measure_spec);

  void MeasureChild(views::View* child,
                    RelativeLayoutParams* params,
                    int my_width,
                    int my_height);

  void PositionChildHorizontal(views::View* view,
                               RelativeLayoutParams* params,
                               int my_width,
                               bool wrap_content);

  void PositionChildVertical(views::View* view,
                             RelativeLayoutParams* params,
                             int my_height,
                             bool wrap_content);

  int GetChildMeasureSpec(int child_start,
                          int child_end,
                          int child_size,
                          int start_margin,
                          int end_margin,
                          int start_padding,
                          int end_padding,
                          int my_size);

  RelativeLayoutParams* GetRelatedViewParams(int id);

  views::View* GetRelatedView(int id);

  struct DependencyGraph;
  std::unique_ptr<DependencyGraph> graph_;

  gfx::Insets padding_;

  bool dirty_hierarchy = true;

  std::vector<views::View*> sorted_horizontal_children_;
  std::vector<views::View*> sorted_vertical_children_;
};

}  // namespace Layout

#endif
