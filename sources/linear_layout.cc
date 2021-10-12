#include "linear_layout.h"

#include "layout_attribute.h"
#include "layout_params.h"
#include "layout_types.h"
#include "property_manager.h"

#include "ui/views/view.h"

namespace {
constexpr int kLayout_Weight = 0;

LAYOUT_PROPERTY(layout_weight, kGroup_Linear, kLayout_Weight)

constexpr int kOrientationVertical = 1;
constexpr int kOrientationHorizontal = 0;

LAYOUT_CONVERT(horizontal, kOrientationHorizontal)
LAYOUT_CONVERT(vertical, kOrientationVertical)

constexpr int kGravityCenterVertical = 1;
constexpr int kGravityCenterHorizontal = 2;
constexpr int kGravityCenter =
    kGravityCenterVertical | kGravityCenterHorizontal;
constexpr int kGravityLeft = 4;
constexpr int kGravityRight = 5;
constexpr int kGravityTop = 6;
constexpr int kGravityBottom = 7;

LAYOUT_CONVERT(center, kGravityCenter)
LAYOUT_CONVERT(center_vertical, kGravityCenterVertical)
LAYOUT_CONVERT(center_horizontal, kGravityCenterHorizontal)
LAYOUT_CONVERT(left, kGravityLeft)
LAYOUT_CONVERT(right, kGravityRight)
LAYOUT_CONVERT(top, kGravityTop)
LAYOUT_CONVERT(bottom, kGravityBottom)
}  // namespace

namespace Layout {
class LinearLayout::LinearLayoutParams : public LayoutParams {
 public:
  LinearLayoutParams(const LayoutTypes& types) : LayoutParams(types) {
    size_t count = types.GetCount();
    for (size_t i = 0; i < count; ++i) {
      types.SetIndex(i);
      if (types.GetGroup() != kGroup_Linear)
        continue;

      int attr = types.GetType();
      if (attr == kLayout_Weight)
        weight = types.GetInt();
    }
  }

  static LinearLayoutParams* Get(views::View* v) {
    LayoutParams* layout_params = LayoutParams::Get(v);
    if (layout_params == nullptr)
      return nullptr;

    return static_cast<LinearLayoutParams*>(layout_params);
  }

  int weight = 0;
};

LayoutParams* LayoutLinear::BuildLayoutParams(const LayoutTypes& types) {
  return new LinearLayout::LinearLayoutParams(types);
}

LinearLayout::LinearLayout() : 
  orientation_(kOrientationHorizontal),
  gravity_(kGravityCenter){}

LinearLayout::~LinearLayout() = default;

void LinearLayout::SetOrientation(int orientation) {
  orientation_ = orientation;
}

void LinearLayout::SetGravity(int gravity) {
  gravity_ = gravity;
}

void LinearLayout::Measure(views::View* host,
                           int width_measure_spec,
                           int height_measure_spec) {
  padding_ = host->GetInsets();

  if (orientation_ == kOrientationHorizontal)
    MeasureHorizontal(host, width_measure_spec, height_measure_spec);
  else
    MeasureVertical(host, width_measure_spec, height_measure_spec);
}

void LinearLayout::MeasureVertical(views::View* host,
                                   int width_measure_spec,
                                   int height_measure_spec) {
  int total_length = 0;
  int total_weight = 0;
  int max_width = 0;

  int my_width = -1;
  int my_height = -1;
  int width = 0;
  int height = 0;

  const int width_mode = MeasureSpec::Mode(width_measure_spec);
  const int height_mode = MeasureSpec::Mode(height_measure_spec);
  const int width_size = MeasureSpec::Size(width_measure_spec);
  const int height_size = MeasureSpec::Size(height_measure_spec);

  if (width_mode != MeasureSpec::UNSPECIFIED)
    my_width = width_size;

  if (height_mode != MeasureSpec::UNSPECIFIED)
    my_height = height_size;

  for (auto* child : host->children()) {
    LinearLayoutParams* layout_param = LinearLayoutParams::Get(child);

    int child_width_measure_spec = GetChildMeasureSpec(
        layout_param->width, padding_.left(), padding_.right(), my_width);
    int child_height_measure_spec = GetChildMeasureSpec(
        layout_param->height, padding_.top(), padding_.bottom(), my_height);

    MeasureChild(child, child_width_measure_spec, child_height_measure_spec);

    total_weight += layout_param->weight;
    total_length += layout_param->margin.top() + layout_param->margin.bottom();

    if (layout_param->weight == 0 && layout_param->height != LayoutParams::MATCH_PARENT)
      total_length += layout_param->size.height();

    max_width = std::max(max_width, layout_param->size.width());
  }

  if (width_mode == MeasureSpec::UNSPECIFIED)
    width = max_width + padding_.left() + padding_.right();
  else
    width = MeasureSpec::Size(width_measure_spec);

  if (height_mode == MeasureSpec::UNSPECIFIED)
    height = total_length + padding_.top() + padding_.bottom();
  else
    height = MeasureSpec::Size(height_measure_spec);

  int remain_height =
      height - total_length - padding_.top() - padding_.bottom();
  int divide = 0;
  int remain = 0;
  if (remain_height > 0 && total_weight) {
    divide = remain_height / total_weight;
    remain = remain_height % total_weight;
  }

  int gravity_width = gravity_;
  if (gravity_width != kGravityLeft && gravity_width != kGravityRight &&
      (gravity_width & kGravityCenterHorizontal) == 0)
    gravity_width = kGravityLeft;

  int gravity_height = total_weight > 0 ? kGravityTop : gravity_;
  if (gravity_height != kGravityTop && gravity_height != kGravityBottom &&
      (gravity_height & kGravityCenterVertical) == 0)
    gravity_height = kGravityTop;

  int x = padding_.left();
  int y = padding_.top();
  int w = width - padding_.left() - padding_.right();
  //int h = height - padding_.top() - padding_.bottom();
  
  if (gravity_height == kGravityTop)
    y = padding_.top();
  else if (gravity_width == kGravityBottom)
    y = height - padding_.bottom() - total_length;
  else
    y = padding_.top() +
        (height - padding_.top() - padding_.bottom() - total_length) / 2;

  for (auto* child : host->children()) {
    LinearLayoutParams* layout_param = LinearLayoutParams::Get(child);
    if (layout_param->width == LayoutParams::MATCH_PARENT ||
        layout_param->height == LayoutParams::MATCH_PARENT) {
      int child_width_measure_spec = GetChildMeasureSpec(
          layout_param->width, padding_.left(), padding_.right(), width);
      int child_height_measure_spec = GetChildMeasureSpec(
          layout_param->height, padding_.top(), padding_.bottom(), height);

      MeasureChild(child, child_width_measure_spec, child_height_measure_spec);
    }

    if (layout_param->height == LayoutParams::MATCH_PARENT) {
      layout_param->top = padding_.top() + layout_param->margin.top();
      layout_param->bottom =
          height - padding_.bottom() - layout_param->margin.bottom();
    } else {
      layout_param->top = y + layout_param->margin.top();
      layout_param->bottom = layout_param->top;
      if (layout_param->weight > 0) {
        layout_param->bottom += layout_param->weight * divide;
        if (remain > 0) {
          int substract = std::min(layout_param->weight, remain);
          layout_param->bottom += substract;
          remain -= substract;
        }
      } else
        layout_param->bottom += layout_param->size.height();

      y = layout_param->bottom + layout_param->margin.bottom();
    }

    if (layout_param->width == LayoutParams::MATCH_PARENT) {
      layout_param->left = padding_.left() + layout_param->margin.left();
      layout_param->right =
          width - padding_.right() - layout_param->margin.right();
    } else {
      if (gravity_width == kGravityLeft) {
        layout_param->left =  padding_.left() + layout_param->margin.left();
        layout_param->right = layout_param->left + layout_param->size.width();
      } else if (gravity_width == kGravityRight) {
        layout_param->right =
            width - padding_.right() - layout_param->margin.right();
        layout_param->left = layout_param->right - layout_param->size.width();
      } else {
        x = (w - layout_param->margin.left() - layout_param->margin.right() -
             layout_param->size.width()) / 2;

        layout_param->left = x + padding_.left() + layout_param->margin.left();
        layout_param->right = layout_param->left + layout_param->size.width();
      }
    }
  }

  LayoutParams* layout_params = LayoutParams::Get(host);
  if (layout_params)
    layout_params->SetDimension(width, height);
}

void LinearLayout::MeasureHorizontal(views::View* host,
                                     int width_measure_spec,
                                     int height_measure_spec) {
  int total_length = 0;
  int total_weight = 0;
  int max_height = 0;

  int my_width = -1;
  int my_height = -1;
  int width = 0;
  int height = 0;

  const int width_mode = MeasureSpec::Mode(width_measure_spec);
  const int height_mode = MeasureSpec::Mode(height_measure_spec);
  const int width_size = MeasureSpec::Size(width_measure_spec);
  const int height_size = MeasureSpec::Size(height_measure_spec);

  if (width_mode != MeasureSpec::UNSPECIFIED)
    my_width = width_size;

  if (height_mode != MeasureSpec::UNSPECIFIED)
    my_height = height_size;

  for (auto* child : host->children()) {
    LinearLayoutParams* layout_param = LinearLayoutParams::Get(child);

    int child_width_measure_spec = GetChildMeasureSpec(
        layout_param->width, padding_.left(), padding_.right(), my_width);
    int child_height_measure_spec = GetChildMeasureSpec(
        layout_param->height, padding_.top(), padding_.bottom(), my_height);

    MeasureChild(child, child_width_measure_spec, child_height_measure_spec);

    total_weight += layout_param->weight;
    total_length += layout_param->margin.left() + layout_param->margin.right();

    if (layout_param->weight == 0 && layout_param->width != LayoutParams::MATCH_PARENT)
      total_length += layout_param->size.width();

    max_height = std::max(max_height, layout_param->size.height());
  }

  if (width_mode != MeasureSpec::EXACTLY)
    width = total_length + padding_.left() + padding_.right();
  else
    width = MeasureSpec::Size(width_measure_spec);

  if (height_mode != MeasureSpec::EXACTLY)
    height = max_height + padding_.top() + padding_.bottom();
  else
    height = MeasureSpec::Size(height_measure_spec);

  int remain_width = width - total_length - padding_.left() - padding_.right();
  int divide = 0;
  int remain = 0;
  if (remain_width > 0 && total_weight) {
    divide = remain_width / total_weight;
    remain = remain_width % total_weight;
  }

  int gravity_width = total_weight > 0 ? kGravityLeft : gravity_;
  if (gravity_width != kGravityLeft && gravity_width != kGravityRight &&
      (gravity_width & kGravityCenterHorizontal) == 0)
    gravity_width = kGravityLeft;

  int gravity_height = gravity_;
  if (gravity_height != kGravityTop && gravity_height != kGravityBottom &&
      (gravity_height & kGravityCenterVertical) == 0)
    gravity_height = kGravityTop;

  int x = padding_.left();
  int y = padding_.top();
  //int w = width - padding_.left() - padding_.right();
  int h = height - padding_.top() - padding_.bottom();
  
  if (gravity_width == kGravityLeft)
    x = padding_.left();
  else if (gravity_width == kGravityRight)
    x = width - padding_.right() - total_length;
  else
    x = padding_.left() + (width - padding_.left() - padding_.right() - total_length) / 2;

  for (auto* child : host->children()) {
    LinearLayoutParams* layout_param = LinearLayoutParams::Get(child);
    if (layout_param->width == LayoutParams::MATCH_PARENT ||
        layout_param->height == LayoutParams::MATCH_PARENT) {
      int child_width_measure_spec = GetChildMeasureSpec(
          layout_param->width, padding_.left(), padding_.right(), width);
      int child_height_measure_spec = GetChildMeasureSpec(
          layout_param->height, padding_.top(), padding_.bottom(), height);

      MeasureChild(child, child_width_measure_spec, child_height_measure_spec);
    }

    if (layout_param->width == LayoutParams::MATCH_PARENT) {
      layout_param->left = padding_.left() + layout_param->margin.left();
      layout_param->right = width - padding_.right() - layout_param->margin.right();
    } else {
      layout_param->left = x + layout_param->margin.left();
      layout_param->right = layout_param->left;
      if (layout_param->weight > 0) {
        layout_param->right += layout_param->weight * divide;
        if (remain > 0) {
          int substract = std::min(layout_param->weight, remain);
          layout_param->right += substract;
          remain -= substract;
        }
      } else
        layout_param->right += layout_param->size.width();

      x = layout_param->right + layout_param->margin.right();
    }

    if (layout_param->height == LayoutParams::MATCH_PARENT) {
      layout_param->top = padding_.top() + layout_param->margin.top();
      layout_param->bottom =
          height - padding_.bottom() - layout_param->margin.bottom();
    } else {
      if (gravity_height == kGravityTop) {
        layout_param->top = padding_.top() + layout_param->margin.top();
        layout_param->bottom = layout_param->top + layout_param->size.height();
      } else if (gravity_height == kGravityBottom) {
        layout_param->bottom = height - padding_.bottom() - layout_param->margin.bottom();
        layout_param->top = layout_param->bottom - layout_param->size.height();
      } else {
        y = (h - layout_param->size.height() - layout_param->margin.top() -
             layout_param->margin.bottom()) / 2;

        layout_param->top = y + padding_.top() + layout_param->margin.top();
        layout_param->bottom = layout_param->top + layout_param->size.height();
      }
    }
  }

  LayoutParams* layout_params = LayoutParams::Get(host);
  if (layout_params)
    layout_params->SetDimension(width, height);
}

void LinearLayout::MeasureChild(views::View* child,
                                int width_measure_spec,
                                int height_measure_spec) {
  LayoutMaster* layout_manager =
      static_cast<LayoutMaster*>(child->GetLayoutManager());
  if (layout_manager) {
    layout_manager->Measure(child, width_measure_spec, height_measure_spec);
    return;
  }

  LayoutParams* params = LayoutParams::Get(child);
  gfx::Size size = child->GetPreferredSize();

  int my_width = -1;
  int my_height = -1;
  int width = 0;
  int height = 0;

  const int width_mode = MeasureSpec::Mode(width_measure_spec);
  const int height_mode = MeasureSpec::Mode(height_measure_spec);
  const int width_size = MeasureSpec::Size(width_measure_spec);
  const int height_size = MeasureSpec::Size(height_measure_spec);

  if (width_mode != MeasureSpec::UNSPECIFIED)
    my_width = width_size;

  if (height_mode != MeasureSpec::UNSPECIFIED)
    my_height = height_size;

  if (width_mode == MeasureSpec::EXACTLY)
    width = my_width;

  if (height_mode == MeasureSpec::EXACTLY)
    height = my_height;

  const bool wrap_content_width = width_mode != MeasureSpec::EXACTLY;
  const bool wrap_content_height = height_mode != MeasureSpec::EXACTLY;

  if (params->width > 0)
    params->size.set_width(params->width);
  else if (wrap_content_width)
    params->size.set_width(size.width());
  else
    params->size.set_width(width);

  if (params->height > 0)
    params->size.set_height(params->height);
  else if (wrap_content_height)
    params->size.set_height(size.height());
  else
    params->size.set_height(height);
}

int LinearLayout::GetChildMeasureSpec(int child_size,
                                      int start_padding,
                                      int end_padding,
                                      int my_size) {
  int child_spec_mode = 0;
  int child_spec_size = 0;
  
  if (child_size > 0) {
    child_spec_size = child_size;
    child_spec_mode = MeasureSpec::EXACTLY;
    return MeasureSpec::Make(child_spec_size, child_spec_mode);
  }

  if (child_size == LayoutParams::MATCH_PARENT && my_size > 0) {
    child_spec_size = my_size - start_padding - end_padding;
    child_spec_mode = MeasureSpec::EXACTLY;
  } else {
    child_spec_size = 0;
    child_spec_mode = MeasureSpec::UNSPECIFIED;
  }

  return MeasureSpec::Make(child_spec_size, child_spec_mode);
}

}  // namespace Layout
