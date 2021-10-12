#ifndef _PROPERTY_MANAGER_H_
#define _PROPERTY_MANAGER_H_

#include <map>
#include <string>

namespace Layout {
constexpr uint8_t kGroup_View = 1;
constexpr uint8_t kGroup_Control = 2;
constexpr uint8_t kGroup_Layout = 3;
constexpr uint8_t kGroup_Linear = 4;
constexpr uint8_t kGroup_Relative = 5;

constexpr uint8_t kGroup_Invalid = 0;

uint8_t LayoutGroup(uint8_t params);
uint8_t LayoutIndex(uint8_t params);

uint8_t FindProperty(const std::string& property);
bool FindValue(const std::string& property, int* value);

struct PropertyCollect {
  PropertyCollect(std::string, uint8_t group, uint8_t index);
};

struct PropertyValue {
  PropertyValue(std::string, int);
};
}  // namespace Layout


#define LAYOUT_PROPERTY(logo, group, index) \
  Layout::PropertyCollect property_##logo(#logo, Layout::group, index);

#define LAYOUT_CONVERT(logo, value) \
  Layout::PropertyValue property_value_##logo(#logo, value);

#endif