# layout
lite android layout for chromium

include linearlayout and relativelayout

If you are familiar with android, it is easy to get started

# using layout

First you need write a layout xml file like this

```c
<?xml version="1.0" encoding="utf-8"?>
<RelativeLayout
    layout_width="match_parent"
    layout_height="match_parent">
    
    <Button id="@+id/top"
          layout_width="wrap_content"
          layout_height="wrap_content"
          layout_margin_top="20"
          layout_left_left="parent"
          layout_right_right="parent"
          layout_top_top="parent"
          text="relatvie up"/>
          
     <Button id="@+id/middle"
          layout_width="wrap_content"
          layout_height="100"
          layout_margin_top="20"
          layout_left_left="@id/top"
          layout_right_right="@id/top"
          layout_top_bottom="@id/top"
          layout_padding_left="20"
          layout_padding_right="20"
          text="relatvie middle"/>
          
     <Button id="@+id/right"
          layout_width="wrap_content"
          layout_height="wrap_content"
          layout_margin_top="20"
          layout_left_right="@id/middle"
          layout_right_right="parent"
          layout_top_top="@id/middle"
          layout_bottom_bottom="@id/middle"
          text="relative right"/>
</RelativeLayout>
```
you can find a xml example named 1.xml in project folder

Second load xml resource to initial views
```c
ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
View* v = Layout::LayoutBuild(rb.LoadDataResourceString(IDR_LAYOUT_1_XML));
AddChildView(v);
SetUseDefaultFillLayout(true);
```

# Requirements
* chromium 90+

# Authors
* wlggdm(wlggdm@163.com)