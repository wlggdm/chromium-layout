#ifndef _LAYOUT_PARAMS_H_
#define _LAYOUT_PARAMS_H_

#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/metadata/metadata_types.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/size.h"
#include "ui/views/view_observer.h"

namespace views {
class View;
}

namespace Layout {

namespace MeasureSpec {
constexpr int MODE_SHIFT = 30;
constexpr int MODE_MASK = 0X3 << MODE_SHIFT;
constexpr int UNSPECIFIED = 0 << MODE_SHIFT;
constexpr int EXACTLY = 1 << MODE_SHIFT;
constexpr int AT_MOST = 2 << MODE_SHIFT;

int Mode(int measure_spec);
int Size(int measure_spec);
int Make(int size, int mode);
}  // namespace MeasureSpec

//解析layout datas
class LayoutParams : public ui::metadata::MetaDataProvider {
 public:
  static constexpr int MATCH_PARENT = -1;
  static constexpr int WRAP_CONTENT = -2;

  METADATA_HEADER_BASE(LayoutParams);

  explicit LayoutParams();
  ~LayoutParams() override;


  static LayoutParams* Get(const views::View*);

  // void OnViewIsDeleting(views::View* observed_view) override;
  bool SetValueAsString(const std::string& key, const std::u16string& value);

  void SetWidth(int width) { width_ = width; }
  int GetWidth() const { return width_; }

  void SetHeight(int height) { height_ = height; }
  int GetHeight() const { return height_; }

  // if use kMarginKey will associate with view
  void SetMargin(gfx::Insets margin) { margin_ = margin; }
  gfx::Insets GetMargin() const { return margin_; }

  void SetPadding(gfx::Insets padding) { padding_ = padding; }
  gfx::Insets GetPadding() const { return padding_; }

  void SetDimension(int w, int h);

 private:
  int width_ = 0;
  int height_ = 0;

  gfx::Insets margin_;
  gfx::Insets padding_;

  // gfx::Insets margin;

 public:
  int left;
  int top;
  int right;
  int bottom;

  gfx::Size size;
};
}  // namespace Layout

#endif