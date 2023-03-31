#include "layout_manager_base.h"

#include "layout_params.h"

#include "ui/views/view.h"

DEFINE_UI_CLASS_PROPERTY_TYPE(Layout::LayoutParams*)
DEFINE_UI_CLASS_PROPERTY_KEY(Layout::LayoutParams*, kLayoutParams, nullptr)

namespace Layout {

LayoutManagerBase::LayoutManagerBase() = default;

LayoutManagerBase::~LayoutManagerBase() = default;

void LayoutManagerBase::Layout(views::View* host) {
  gfx::Rect child_area = host->GetContentsBounds();

  Measure(host, MeasureSpec::Make(child_area.width(), MeasureSpec::EXACTLY),
          MeasureSpec::Make(child_area.height(), MeasureSpec::EXACTLY));

  for (auto* child : host->children()) {
    LayoutParams* layout_params = LayoutParams::Get(child);

    gfx::Rect bounds(layout_params->left, layout_params->top,
                     layout_params->right - layout_params->left,
                     layout_params->bottom - layout_params->top);
    bounds.Offset(child_area.x(), child_area.y());

    child->SetBoundsRect(bounds);
  }
}

gfx::Size LayoutManagerBase::GetPreferredSize(const views::View* host) const {
  gfx::Size size = const_cast<LayoutManagerBase*>(this)->Measure(
      host, MeasureSpec::Make(0, MeasureSpec::UNSPECIFIED),
      MeasureSpec::Make(0, MeasureSpec::UNSPECIFIED));

  gfx::Insets insets = host->GetInsets();
  size.Enlarge(insets.width(), insets.height());

  return size;
}

int LayoutManagerBase::GetPreferredHeightForWidth(const views::View* host,
                                                  int width) const {
  gfx::Size size = const_cast<LayoutManagerBase*>(this)->Measure(
      host, MeasureSpec::Make(width, MeasureSpec::EXACTLY),
      MeasureSpec::Make(0, MeasureSpec::UNSPECIFIED));

  return size.height() + host->GetInsets().height();
}

void LayoutManagerBase::ViewAdded(views::View* host, views::View* view) {
  DCHECK(view->GetProperty(kLayoutParams) == nullptr);

  auto* layout_params = CreateLayoutParams();
  view->SetProperty(kLayoutParams, layout_params);
}

void LayoutManagerBase::ViewRemoved(views::View* host, views::View* view) {
  auto* layout_params = LayoutParams::Get(view);
  if (layout_params)
    delete layout_params;
}

LayoutParams* LayoutParams::Get(const views::View* v) {
  return v->GetProperty(kLayoutParams);
}
}  // namespace Layout
