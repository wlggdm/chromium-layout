#ifndef _LAYOU_ATTRIBUTE_H_
#define _LAYOU_ATTRIBUTE_H_

#include <string>

namespace views {
class View;
}

namespace Layout {
class LayoutTypes;
class LayoutParams;

class LayoutView {
 public:
  LayoutView();
  virtual ~LayoutView();

  static LayoutView* GetViewByName(const std::string& name);

  views::View* Build(const LayoutTypes& types);

  virtual LayoutParams* BuildLayoutParams(const LayoutTypes& types);

 protected:
  virtual views::View* Create();
  virtual void Clear();
  virtual void Deflate(const LayoutTypes& types);
  virtual void Apply(views::View* view);

 private:
  int id;
  int padding_left;
  int padding_right;
  int padding_top;
  int padding_bottom;
};

class LayoutButton : public LayoutView {
 public:
  LayoutButton();
  ~LayoutButton() override;

 private:
  views::View* Create() override;
  void Clear() override;
  void Deflate(const LayoutTypes& types) override;
  void Apply(views::View* view) override;

 private:
  std::string text;
  int gravity;
};

class LayoutLinear : public LayoutView {
 public:
  LayoutLinear();
  ~LayoutLinear() override;

  LayoutParams* BuildLayoutParams(const LayoutTypes& types) override;

 private:
  views::View* Create() override;
  void Clear() override;
  void Deflate(const LayoutTypes& types) override;
  void Apply(views::View* view) override;

 private:
  int orientation;
  int gravity;
};

class LayoutRelative : public LayoutView {
 public:
  LayoutRelative();
  ~LayoutRelative() override;

  LayoutParams* BuildLayoutParams(const LayoutTypes& types) override;

 private:
  views::View* Create() override;
  void Clear() override;
  void Deflate(const LayoutTypes& types) override;
  void Apply(views::View* view) override;
};
}  // namespace Layout
#endif
