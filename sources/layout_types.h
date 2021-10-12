#ifndef _LAYOUT_TYPES_H_
#define _LAYOUT_TYPES_H_

#include <string>
#include <vector>
#include <stdint.h>

namespace Layout {

class LayoutTypes {
 public:
  LayoutTypes();
  ~LayoutTypes();

  void Reset();

  void Add(const std::string& key, const std::string& value);

  size_t GetCount() const;

  void SetIndex(size_t index) const;

	int GetGroup() const;
  int GetType() const;

  bool GetBool() const;

  int GetId() const;

  int GetInt() const;

  int GetDimension() const;

  std::string GetString() const;

  int GetColor();

 private:
  std::vector<std::pair<uint8_t, std::string>> attrs_;

  mutable size_t index;
};
}  // namespace Layout

#endif