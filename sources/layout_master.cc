#include "layout_master.h"
#include "layout_params.h"

#include "ui/views/view.h"

namespace Layout {

LayoutMaster::LayoutMaster() : root_(false){}

LayoutMaster::~LayoutMaster() {}

void LayoutMaster::Layout(views::View* host) {
  if (root_) {
    gfx::Size bounds = host->size();
    Measure(host, MeasureSpec::Make(bounds.width(), MeasureSpec::EXACTLY),
            MeasureSpec::Make(bounds.height(), MeasureSpec::EXACTLY));
  }

  for (auto* child : host->children()) {
    LayoutParams* layout_params = LayoutParams::Get(child);
    child->SetBounds(layout_params->left, layout_params->top,
                     layout_params->right - layout_params->left,
                     layout_params->bottom - layout_params->top);
  }
}

gfx::Size LayoutMaster::GetPreferredSize(const views::View* host) const {
  return gfx::Size();
}

LayoutParams* LayoutMaster::GetLayoutParams(views::View* v) {
  auto iter = layout_params.find(v);
  if (iter == layout_params.end())
    return nullptr;

  return iter->second.get();
}

void LayoutMaster::SetLayoutParams(views::View* v, LayoutParams* params) {
  layout_params[v] = std::unique_ptr<LayoutParams>(params);
}

void LayoutMaster::SetRoot(bool root) {
  root_ = root;
}

}  // namespace Layout