#ifndef _LAYOUT_MANAGER_BASE_H_
#define _LAYOUT_MANAGER_BASE_H_

#include "ui/views/layout/layout_manager.h"

namespace views {
class View;
}

namespace Layout {

class LayoutManagerBase : public views::LayoutManager {
 public:
  LayoutManagerBase();
  ~LayoutManagerBase() override;

  // views::LayoutManager
  void Layout(views::View* host) override;
  gfx::Size GetPreferredSize(const views::View* host) const override;
  int GetPreferredHeightForWidth(const views::View* host,
                                 int width) const override;

  void ViewAdded(views::View* host, views::View* view) override;
  void ViewRemoved(views::View* host, views::View* view) override;

  virtual gfx::Size Measure(const views::View* v,
                            int width_measure_spec,
                            int height_measure_spec) = 0;

 protected:
  virtual class LayoutParams* CreateLayoutParams() = 0;
};
}  // namespace Layout

#endif
