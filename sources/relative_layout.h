#ifndef _RELATIVE_LAYOUT_H_
#define _RELATIVE_LAYOUT_H_

#include <memory>
#include <vector>

#include "layout_manager_base.h"

#include "ui/gfx/geometry/insets.h"

namespace views {
class View;
}

namespace Layout {
class RelativeLayoutParams;

class RelativeLayout : public LayoutManagerBase {
 public:
  RelativeLayout();
  ~RelativeLayout() override;

  // LayoutManagerBase
  gfx::Size Measure(const views::View* host,
                    int width_measure_spec,
                    int height_measure_spec) override;

 private:
  void SortChildren(const views::View* host);

  void ApplyHorizontalSizeRules(RelativeLayoutParams* params, int my_width);
  void ApplyVerticalSizeRules(RelativeLayoutParams* params, int my_height);

  void MeasureChildHorizontal(views::View* child, RelativeLayoutParams* params);

  void MeasureChildVertical(views::View* child, RelativeLayoutParams* params);

  void PositionChildHorizontal(views::View* view,
                               RelativeLayoutParams* params,
                               int my_width,
                               bool wrap_content);

  void PositionChildVertical(views::View* view,
                             RelativeLayoutParams* params,
                             int my_height,
                             bool wrap_content);

  RelativeLayoutParams* GetRelatedViewParams(int id);

  views::View* GetRelatedView(int id);

  class LayoutParams* CreateLayoutParams() override;

 private:
  struct DependencyGraph;
  std::unique_ptr<DependencyGraph> graph_;

  bool dirty_hierarchy = true;

  std::vector<views::View*> sorted_horizontal_children_;
  std::vector<views::View*> sorted_vertical_children_;
};

}  // namespace Layout

#endif
