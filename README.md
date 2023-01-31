# relative layout

relative layout for chromium ui layout

# using layout

* First you need write a layout xml file like this

```c
<?xml version="1.0" encoding="utf-8"?>
<RelativeView id="@+id/root"
    layout_width="match_parent"
    layout_height="match_parent">
     <Button id="@+id/top"
          layout_width="0"
          layout_height="100"
          layout_left_right="@id/left"
          layout_right_right="parent"
          layout_top_top="parent"
          text="top"/>
     <Button id="@+id/right"
          layout_width="100"
          layout_top_bottom="@id/top"
          layout_right_right="parent"
          layout_bottom_bottom="parent"
          text="right"/>
     <Button id="@+id/bottom"
          layout_height="100"
          layout_left_left="parent"
          layout_right_left="@id/right"
          layout_bottom_bottom="parent"
          text="bottom"/>
     <Button id="@+id/left"
          layout_width="100"
          layout_left_left="parent"
          layout_top_top="parent"
          layout_bottom_top="@id/bottom"
          text="left"/>
          
     <Button id="@+id/middle"
          layout_width="100"
          layout_height="100"
          layout_left_right="@id/left"
          layout_right_left="@id/right"
          layout_top_bottom="@id/top"
          layout_bottom_top="@id/bottom"
          text="middle"/>
</RelativeView>
```
you can find a xml example named 1.xml in project folder

* Second load xml resource to initial views
```c
ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
View* v = Layout::LayoutBuild(rb.LoadDataResourceString(IDR_LAYOUT_1_XML));
AddChildView(v);
SetUseDefaultFillLayout(true);
```
* Third you can use the generated id file to get view
```c
//layout_id.h
namespace R::id { 

constexpr int Left                      = 0x7F0F0000;
constexpr int Right                     = 0x7F0F0001;
constexpr int Top                       = 0x7F0F0002;
constexpr int Bottom                    = 0x7F0F0003;
constexpr int Middle                    = 0x7F0F0004;

} //namespace R::id
```

# Effect
sources/res/1.xml

![image](https://user-images.githubusercontent.com/11361001/137422245-15cac355-97e6-49a8-925c-24af6fd93285.png)

# Requirements
* chromium 90+

# Authors
* wlggdm(wlggdm@163.com)
