# ,abv,ibu,id,name,style,brewery_id,ounces
set model [load_charts_model -csv data/beers.csv -first_line_header]

set_charts_data -model $model -column 1 -name column_type \
  -value {{real} {format %g%%} {format_scale 100}}

#set plot [create_charts_plot -model $model -type scatter \
#  -columns {{x ibu} {y abv} {group style}}]
set plot [create_charts_plot -model $model -type scatter \
  -columns {{x ibu} {y abv} {group brewery_id} {tips {name style ounces}}}]

set_charts_property -plot $plot -name range.view -value {25 0 100 100}

#set_charts_property -plot $plot -name margins.outer.left  -value 512px
#set_charts_property -plot $plot -name margins.outer.right -value 32px

#---

proc addFilterStr { str filter } {
  upvar $str str1

  if {"$str1" == ""} {
    append str1 $filter
  } else {
    append str1 " || " $filter
  }
}

proc updateFilter { } {
  set size12 [get_charts_property -annotation $::glass_size12 -name checked]
  set size16 [get_charts_property -annotation $::glass_size16 -name checked]

  set ale_beer    [get_charts_property -annotation $::ale_beer    -name checked]
  set ipa_beer    [get_charts_property -annotation $::ipa_beer    -name checked]
  set lager_beer  [get_charts_property -annotation $::lager_beer  -name checked]
  set porter_beer [get_charts_property -annotation $::porter_beer -name checked]
  set stout_beer  [get_charts_property -annotation $::stout_beer  -name checked]

  set sizeFilter ""

  if {$size12} {
    addFilterStr sizeFilter {$ounces=="12"}
  }

  if {$size16} {
    addFilterStr sizeFilter {$ounces=="16"}
  }

  set styleFilter ""
 
  if {$ale_beer} {
    addFilterStr styleFilter {match(@{style},".*Ale.*")}
  }

  if {$ipa_beer} {
    addFilterStr styleFilter {match(@{style},".*IPA.*")}
  }

  if {$lager_beer} {
    addFilterStr styleFilter {match(@{style},".*Lager.*")}
  }

  if {$porter_beer} {
    addFilterStr styleFilter {match(@{style},".*Porter.*")}
  }

  if {$stout_beer} {
    addFilterStr styleFilter {match(@{style},".*Stout.*")}
  }

  set filter ""

  if       {"$sizeFilter" == "" || "$styleFilter" == ""} {
    append filter {$ounces=="-1"}
  } else {
    append filter "(" $sizeFilter ") && (" $styleFilter ")"
  }

  filter_charts_model -model $::model -expr $filter
}

proc imageSlot { viewId id } {
  set mode [get_charts_data -view $viewId -name mouse_modifier]

  if       {$mode == "toggle"} {
    set checked [expr {1 - [get_charts_property -annotation $id -name checked]}]

    set_charts_property -annotation $id -name checked -value $checked
  } elseif {$mode == "replace"} {
    set checked 1

    if {"$id" == "size12" || "$id" == "size16"} {
      set_charts_property -annotation $::glass_size12 -name checked -value 0
      set_charts_property -annotation $::glass_size16 -name checked -value 0
    } else {
      set_charts_property -annotation $::ale_beer    -name checked -value 0
      set_charts_property -annotation $::ipa_beer    -name checked -value 0
      set_charts_property -annotation $::lager_beer  -name checked -value 0
      set_charts_property -annotation $::porter_beer -name checked -value 0
      set_charts_property -annotation $::stout_beer  -name checked -value 0
    }

    set_charts_property -annotation $id -name checked -value $checked
  }

  updateFilter
}

set view [get_charts_property -plot $plot -name viewId]

set_charts_property -view $view -name theme.dark -value 1

set_charts_property -plot $plot -name key.visible -value 0

set_charts_property -plot $plot -name symbol.size           -value 8px
set_charts_property -plot $plot -name symbol.fill.alpha     -value 0.3
set_charts_property -plot $plot -name symbol.stroke.visible -value 0

set glass_size12 [create_charts_image_annotation -view $view -id size12 \
  -rectangle {5 85 15 95} -image "data/beer_glass_12.svg" \
  -properties {{checkable 1} {checked 1}}]
set glass_size16 [create_charts_image_annotation -view $view -id size16 \
  -rectangle {15 85 25 95} -image "data/beer_glass_16.svg" \
  -properties {{checkable 1} {checked 1}}]

set ale_beer [create_charts_image_annotation -view $view -id ale \
  -rectangle {0 65 5 80} -image "data/beer_bottle_ale.svg" \
  -properties {{checkable 1} {checked 1}}]
set ipa_beer [create_charts_image_annotation -view $view -id ipa \
  -rectangle {5 65 10 80} -image "data/beer_bottle_ipa.svg" \
  -properties {{checkable 1} {checked 1}}]
set lager_beer [create_charts_image_annotation -view $view -id lager \
  -rectangle {10 65 15 80} -image "data/beer_bottle_lager.svg" \
  -properties {{checkable 1} {checked 1}}]
set porter_beer [create_charts_image_annotation -view $view -id porter \
  -rectangle {15 65 20 80} -image "data/beer_bottle_porter.svg" \
  -properties {{checkable 1} {checked 1}}]
set stout_beer [create_charts_image_annotation -view $view -id stout \
  -rectangle {20 65 25 80} -image "data/beer_bottle_stout.svg" \
  -properties {{checkable 1} {checked 1}}]

connect_charts_signal -view $view -from annotationIdPressed -to imageSlot

updateFilter
