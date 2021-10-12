#ifndef _LAYOUT_DEFLATE_H_
#define _LAYOUT_DEFLATE_H_

#include <string>

namespace views {
class View;
}

namespace Layout {

  views::View* LayoutBuild(const std::string& resource);

}  // namespace Layout

#endif