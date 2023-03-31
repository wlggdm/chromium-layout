#include "layout_params.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "layout_metadata_impl_macros.h"
#include "ui/base/metadata/base_type_conversion.h"

/*
struct layout_length {};
MAKE_TYPE_UNIQUE(layout_length);

namespace ui::metadata {
template <>
struct TypeConverter<UNIQUE_TYPE_NAME(layout_length)>
    : ui::metadata::BaseTypeConverter<true, false> {
  static std::u16string ToString(int) { return u""; }
  static absl::optional<int> FromString(const std::u16string& source_value) {
    if (source_value == u"WRAP_CONTENT")
      return -1;

    int ret;
    if (base::StringToInt(source_value, &ret))
      return ret;

    return absl::nullopt;
  }
  static ValidStrings GetValidStrings() { return {u"WRAP_CONTENT", u">=0"}; }
};

using LengthConverter = TypeConverter<UNIQUE_TYPE_NAME(layout_length)>;
}  // namespace ui::metadata
*/
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

LayoutParams::LayoutParams() = default;

LayoutParams::~LayoutParams() = default;
/*
void LayoutParams::OnViewIsDeleting(views::View* observed_view) {
  delete this;
}
*/
bool LayoutParams::SetValueAsString(const std::string& key,
                                    const std::u16string& value) {
  auto* member_metadata = GetClassMetaData()->FindMemberData(key);
  if (!member_metadata)
    return false;

  member_metadata->SetValueAsString(this, value);

  return true;
}

void LayoutParams::SetDimension(int w, int h) {
  size.SetSize(w, h);
}

struct LengthTypeConverter : ui::metadata::BaseTypeConverter<true, false> {
  static std::u16string ToString(int value) {
    if (value == LayoutParams::WRAP_CONTENT)
      return u"wrap_content";

    return base::NumberToString16(value);
  }

  static absl::optional<int> FromString(const std::u16string& source_value) {
    if (source_value == u"wrap_content")
      return LayoutParams::WRAP_CONTENT;

    int ret;
    if (base::StringToInt(source_value, &ret))
      return ret;

    return absl::nullopt;
  }

  static ui::metadata::ValidStrings GetValidStrings() {
    return {u"wrap_content", u">=0"};
  }
};

BEGIN_METADATA_BASE(LayoutParams)
ADD_LAYOUT_PROPERTY_METADATA(int, layout_width, Width, LengthTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(int, layout_height, Height, LengthTypeConverter)
ADD_LAYOUT_PROPERTY_METADATA(gfx::Insets, layout_margin, Margin)
ADD_LAYOUT_PROPERTY_METADATA(gfx::Insets, layoug_padding, Padding)
END_METADATA

}  // namespace Layout
