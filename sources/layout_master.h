#ifndef _LAYOUT_MASTER_H_
#define _LAYOUT_MASTER_H_

#include <map>
#include <memory>

#include "ui/views/layout/layout_manager.h"

namespace views {
class View;
}

namespace Layout {
class LayoutParams;

class LayoutMaster : public views::LayoutManager {
 public:
  LayoutMaster();
  ~LayoutMaster() override;

  // views::LayoutManager
  void Layout(views::View* host) override;
  gfx::Size GetPreferredSize(const views::View* host) const override;

  virtual void Measure(views::View* v,
                       int width_measure_spec,
                       int height_measure_spec) = 0;

  LayoutParams* GetLayoutParams(views::View* v);
  void SetLayoutParams(views::View* v, LayoutParams*);

  void SetRoot(bool root);

 private:
  std::map<views::View*, std::unique_ptr<LayoutParams>> layout_params;

  bool root_;
};
}  // namespace Layout

#endif