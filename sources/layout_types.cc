#include "layout_types.h"

#include "base/notreached.h"

#include "layout_params.h"
#include "property_manager.h"

namespace {
constexpr char kMatchParent[] = "match_parent";
constexpr char kWrapContent[] = "wrap_content";
constexpr char kParent[] = "parent";
}

namespace Layout {
LayoutTypes::LayoutTypes(){
}

LayoutTypes::~LayoutTypes() = default;

void LayoutTypes::Reset() {
  attrs_.clear();
}

void LayoutTypes::Add(const std::string& key, const std::string& value) {
  uint8_t group = FindProperty(key);
  if (group != kGroup_Invalid)
    attrs_.push_back({group, value});
}

size_t LayoutTypes::GetCount() const {
  return attrs_.size();
}

void LayoutTypes::SetIndex(size_t i) const {
  index = i;
}

int LayoutTypes::GetGroup() const {
  return LayoutGroup(attrs_[index].first);
}

int LayoutTypes::GetType() const {
  return LayoutIndex(attrs_[index].first);
}

bool LayoutTypes::GetBool() const {
  const std::string& content = attrs_[index].second;
  if (content == "false")
    return false;

  if (content == "true")
    return true;

  NOTREACHED();
}

int LayoutTypes::GetId() const {
  //@ref
  const std::string& id = attrs_[index].second;
  if (id == kParent)
    return -1;

  return std::stoi(id.substr(5), nullptr, 16);
}

int LayoutTypes::GetInt() const {
  int result;
  if (FindValue(attrs_[index].second, &result))
    return result;

  size_t length = 0;
  result = std::stoi(attrs_[index].second, &length);
  if (length == attrs_[index].second.size())
    return result;

  //error
  return 0;
}

int LayoutTypes::GetDimension() const {
  const std::string& content = attrs_[index].second;
  if (content == kMatchParent)
    return LayoutParams::MATCH_PARENT;

  if (content == kWrapContent)
    return LayoutParams::WRAP_CONTENT;

  return std::stoi(content);
}

std::string LayoutTypes::GetString() const {
  return attrs_[index].second;
}

int LayoutTypes::GetColor() {
  return std::stoi(attrs_[index].second, nullptr, 16);
}

}  // namespace Layout
