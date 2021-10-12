#include "layout_attribute.h"

#include "layout_types.h"
#include "property_manager.h"
#include "relative_layout.h"
#include "linear_layout.h"

#include "base/strings/utf_string_conversions.h"

#include "ui/views/border.h"
#include "ui/views/background.h"
#include "ui/views/view.h"
#include "ui/views/controls/button/label_button.h"

namespace {
constexpr int kId = 0;
constexpr int kLayout_Padding_Left = 1;
constexpr int kLayout_Padding_Right = 2;
constexpr int kLayout_Padding_Top = 3;
constexpr int kLayout_Padding_Bottom = 4;

constexpr int kColor = 5;
constexpr int kText = 6;
constexpr int kGravity = 7;

LAYOUT_PROPERTY(id, kGroup_View, kId)
LAYOUT_PROPERTY(layout_padding_left, kGroup_View, kLayout_Padding_Left)
LAYOUT_PROPERTY(layout_padding_right, kGroup_View, kLayout_Padding_Right)
LAYOUT_PROPERTY(layout_padding_top, kGroup_View, kLayout_Padding_Top)
LAYOUT_PROPERTY(layout_padding_bottom, kGroup_View, kLayout_Padding_Bottom)

LAYOUT_PROPERTY(color, kGroup_Control, kColor)
LAYOUT_PROPERTY(text, kGroup_Control, kText)
LAYOUT_PROPERTY(gravity, kGroup_Control, kGravity)

constexpr int kLayout_Orientation = 1;
constexpr int kLayout_Gravity = 2;

LAYOUT_PROPERTY(layout_orientation, kGroup_Linear, kLayout_Orientation)
LAYOUT_PROPERTY(layout_gravity, kGroup_Linear, kLayout_Gravity)

constexpr int kGravityCenterVertical = 1;
constexpr int kGravityCenterHorizontal = 2;
constexpr int kGravityCenter =
    kGravityCenterVertical | kGravityCenterHorizontal;
constexpr int kGravityLeft = 4;
constexpr int kGravityRight = 5;
constexpr int kGravityTop = 6;
constexpr int kGravityBottom = 7;


}  // namespace

namespace Layout {

LayoutView::LayoutView() {
  Clear();
}

LayoutView::~LayoutView() {}

LayoutView* LayoutView::GetViewByName(const std::string& name) {
  static std::map<std::string, LayoutView*> view_map = {
      {"View", new LayoutView },
      {"Button", new LayoutButton },
      {"RelativeLayout", new LayoutRelative },
      {"LinearLayout", new LayoutLinear }};

  auto iter = view_map.find(name);
  if (iter == view_map.end())
    return nullptr;

  return iter->second;
}

views::View* LayoutView::Build(const LayoutTypes& types) {
  Clear();

  size_t count = types.GetCount();
  for (size_t i = 0; i < count; ++i) {
    types.SetIndex(i);
    Deflate(types);
  }
    
  views::View* view = Create();
  Apply(view);

  return view;
}

void LayoutView::Clear() {
  id = -1;
  padding_left = 0;
  padding_right = 0;
  padding_top = 0;
  padding_bottom = 0;
}

views::View* LayoutView::Create() {
  return new views::View();
}

void LayoutView::Deflate(const LayoutTypes& types) {
  if (types.GetGroup() != kGroup_View)
    return;

  int attr = types.GetType();
  switch (attr) {
    case kId: {
      id = types.GetId();
      break;
    }
    case kLayout_Padding_Left: {
      padding_left = types.GetInt();
      break;
    }
    case kLayout_Padding_Right: {
      padding_right = types.GetInt();
      break;
    }
    case kLayout_Padding_Top: {
      padding_top = types.GetInt();
      break;
    }
    case kLayout_Padding_Bottom: {
      padding_bottom = types.GetInt();
      break;
    }
  }
}

void LayoutView::Apply(views::View* view) {
  view->SetID(id);
  view->SetBorder(views::CreateEmptyBorder(
      gfx::Insets(padding_top, padding_left, padding_bottom, padding_right)));
}

LayoutButton::LayoutButton() {}

LayoutButton::~LayoutButton() {}

views::View* LayoutButton::Create() {
  return new views::LabelButton();
}

void LayoutButton::Clear() {
  LayoutView::Clear();

  text.clear();
  gravity = 0;
}

void LayoutButton::Deflate(const LayoutTypes& types) {
  LayoutView::Deflate(types);

  if (types.GetGroup() != kGroup_Control)
    return;

  int attr = types.GetType();
  switch (attr) {
    case kText: {
      text = types.GetString();
      break;
    }
    case kGravity: {
      gravity = types.GetInt();
      break;
    }
  }
}

void LayoutButton::Apply(views::View* view) {
  LayoutView::Apply(view);

  views::LabelButton* label_button = static_cast<views::LabelButton*>(view);
  label_button->SetText(base::UTF8ToUTF16(text));
  if (gravity == kGravityLeft || gravity == 0)
    label_button->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_LEFT);
  else if (gravity == kGravityRight)
    label_button->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_RIGHT);
  else if (gravity & kGravityCenterHorizontal)
    label_button->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_CENTER);
}

LayoutLinear::LayoutLinear() {}

LayoutLinear::~LayoutLinear() {}

views::View* LayoutLinear::Create() {
  views::View* v = LayoutView::Create();

  v->SetLayoutManager(std::make_unique<LinearLayout>());

  return v;
}

void LayoutLinear::Clear() {
  LayoutView::Clear();

  orientation = 0;
  gravity = 0;
}

void LayoutLinear::Deflate(const LayoutTypes& types) {
  LayoutView::Deflate(types);

  if (types.GetGroup() != kGroup_Linear)
    return;

  int attr = types.GetType();
  switch (attr) {
    case kLayout_Orientation: {
      orientation = types.GetInt();
      break;
    }
    case kLayout_Gravity: {
      gravity = types.GetInt();
      break;
    }
  }
}

void LayoutLinear::Apply(views::View* view) {
  LayoutView::Apply(view);

  LinearLayout* layout_manager = static_cast<LinearLayout*>(view->GetLayoutManager());
  
  layout_manager->SetOrientation(orientation);
  layout_manager->SetGravity(gravity);
}

LayoutRelative::LayoutRelative() {}

LayoutRelative::~LayoutRelative() {}

views::View* LayoutRelative::Create() {
  views::View* v = LayoutView::Create();

  v->SetLayoutManager(std::make_unique<RelativeLayout>());

  return v;
}

void LayoutRelative::Clear() {
  LayoutView::Clear();
}

void LayoutRelative::Deflate(const LayoutTypes& types) {
  LayoutView::Deflate(types);
}

void LayoutRelative::Apply(views::View* view) {
  LayoutView::Apply(view);
}

}  // namespace Layout
