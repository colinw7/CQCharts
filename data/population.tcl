proc annotationSlot { viewId id } {
  #puts "$viewId, $id"

  #get_property -view $viewId -annotation $id -name position
  #get_property -view $viewId -annotation $id -name text

  if       {$id == "next"} {
    if {$::year < $::maxYear} {
      incr ::year 10
    }
  } elseif {$id == "prev"} {
    if {$::year > $::minYear} {
      incr ::year -10
    }
  } elseif {$id == "overlay"} {
    set ::place $id
  } elseif {$id == "sidebyside"} {
    set ::place $id
  }

  plotYear $::year
}

proc plotYear { year } {
  if {$::viewId != ""} {
    remove_plot -view $::viewId -all
  }

  set filter "sex:1,year:$year"

  set model1Id [load_model -csv data/population.csv -first_line_header -filter $filter]

  set title "Male %year"

  set ::plot1Id [create_plot -type barchart -columns "name=1,value=3" -title $title]

  set_property -view $::viewId -plot $::plot1Id -name fill.color -value "#4444aa"
  set_property -view $::viewId -plot $::plot1Id -name fill.alpha -value 0.5
  set_property -view $::viewId -plot $::plot1Id -name key.visible -value 0

  set filter "sex:2,year:$year"

  set model2Id [load_model -csv data/population.csv -first_line_header -filter $filter]

  set title "Female $year"

  set ::plot2Id [create_plot -type barchart -columns "name=1,value=3" -title $title]

  set_property -view $::viewId -plot $::plot2Id -name fill.color -value "#aa4444"
  set_property -view $::viewId -plot $::plot2Id -name fill.alpha -value 0.5
  set_property -view $::viewId -plot $::plot2Id -name key.visible -value 0

  if       {$::place == "sidebyside"} {
    place_plots -horizontal $::plot1Id $::plot2Id
  } elseif {$::place == "overlay"} {
    group_plots -overlay $::plot1Id $::plot2Id
  }
}

set modelId [load_model -csv data/population.csv -first_line_header]
#puts $modelId

set nr [get_model -ind $modelId -name num_rows]
#puts $nr
set nc [get_model -ind $modelId -name num_columns]
#puts $nc

set ::minYear [get_model -ind $modelId -column 0 -name min]
set ::maxYear [get_model -ind $modelId -column 0 -name max]

set ::year $::minYear

set ::viewId ""

set ::place "overlay"

plotYear $::year

set ::viewId [get_view -name id]

set dtx [get_view -view $::viewId -name text_width -data "xx"]
puts $dtx

set tx [expr {2*$dtx}]
puts $tx

set text1Id [text_shape -id prev -x $tx -y 95 -text "Prev" -border 1 -background 1]

set tw [get_view -view $::viewId -name text_width -data "Prev"]
set tx [expr {$tx + $tw + $dtx}]
puts $tx

set text2Id [text_shape -id next -x $tx -y 95 -text "Next" -border 1 -background 1]

set tw [get_view -view $::viewId -name text_width -data "Next"]
set tx [expr {$tx + $tw + 2*$dtx}]
puts $tx

set text3Id [text_shape -id overlay -x $tx -y 95 -text "Overlay" -border 1 -background 1]

set tw [get_view -view $::viewId -name text_width -data "Overlay"]
set tx [expr {$tx + $tw + $dtx}]
puts $tx

set text4Id [text_shape -id sidebyside -x $tx -y 95 -text "Side By Side" -border 1 -background 1]
connect -view $::viewId -from annotationIdPressed -to annotationSlot
