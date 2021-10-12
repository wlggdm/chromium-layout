#include "layout_params.h"

#include "layout_attribute.h"
#include "layout_types.h"
#include "property_manager.h"
#include "layout_master.h"

#include "ui/views/view.h"

namespace {
constexpr int VALUE_NOT_SET = std::numeric_limits<int>::min();

constexpr int kLayout_Width = 0;
constexpr int kLayout_Height = 1;
constexpr int kLayout_Margin_Left = 2;
constexpr int kLayout_Margin_Right = 3;
constexpr int kLayout_Margin_Top = 4;
constexpr int kLayout_Margin_Bottom = 5;

LAYOUT_PROPERTY(layout_width, kGroup_Layout, kLayout_Width)
LAYOUT_PROPERTY(layout_height, kGroup_Layout, kLayout_Height)
LAYOUT_PROPERTY(layout_margin_left, kGroup_Layout, kLayout_Margin_Left)
LAYOUT_PROPERTY(layout_margin_right, kGroup_Layout, kLayout_Margin_Right)
LAYOUT_PROPERTY(layout_margin_top, kGroup_Layout, kLayout_Margin_Top)
LAYOUT_PROPERTY(layout_margin_bottom, kGroup_Layout, kLayout_Margin_Bottom)
}  // namespace

namespace Layout {

namespace MeasureSpec {
int Mode(int measure_spec) {
  return measure_spec & MODE_MASK;
}

int Size(int measure_spec) {
  return measure_spec & ~MODE_MASK;
}

int Make(int size, int mode) {
  return size | mode;
}

}  // namespace MeasureSpec

LayoutParams* LayoutParams::Get(views::View* view) {
  views::View* parent = view->parent();
  if (parent == nullptr)
    return nullptr;

  views::LayoutManager* layout_manager = parent->GetLayoutManager();
  if (layout_manager == nullptr)
    return nullptr;

  return static_cast<LayoutMaster*>(layout_manager)->GetLayoutParams(view);
}

void LayoutParams::Set(views::View* view, LayoutParams* params) {
  views::View* parent = view->parent();
  if (parent == nullptr)
    return;

  views::LayoutManager* layout_manager = parent->GetLayoutManager();
  if (layout_manager == nullptr)
    return;

  static_cast<LayoutMaster*>(layout_manager)->SetLayoutParams(view, params);
}

LayoutParams::LayoutParams(const LayoutTypes& types)
    : width(0),
      height(0),
      left(VALUE_NOT_SET),
      top(VALUE_NOT_SET),
      right(VALUE_NOT_SET),
      bottom(VALUE_NOT_SET) {
  size_t count = types.GetCount();
  for (size_t i = 0; i < count; ++i) {
    types.SetIndex(i);
    if (types.GetGroup() != kGroup_Layout)
      continue;

    int attr = types.GetType();
    switch (attr) {
      case kLayout_Width: {
        width = types.GetDimension();
        break;
      }
      case kLayout_Height: {
        height = types.GetDimension();
        break;
      }
      case kLayout_Margin_Left: {
        margin.set_left(types.GetInt());
        break;
      }
      case kLayout_Margin_Right: {
        margin.set_right(types.GetInt());
        break;
      }
      case kLayout_Margin_Top: {
        margin.set_top(types.GetInt());
        break;
      }
      case kLayout_Margin_Bottom: {
        margin.set_bottom(types.GetInt());
        break;
      }
    }
  }
}

void LayoutParams::SetDimension(int w, int h) {
  size.SetSize(w, h);
}

LayoutParams* LayoutView::BuildLayoutParams(const LayoutTypes& types) {
  return new LayoutParams(types);
}

}  // namespace Layout
