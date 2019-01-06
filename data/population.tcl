proc annotationSlot { viewId id } {
  #puts "$viewId, $id"

  #get_charts_property -annotation $id -name position
  #get_charts_property -annotation $id -name text

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

  #set model1Id [load_model -csv data/population.csv -first_line_header]
  #filter_model -model $model1Id -expr "\$sex == 1 || \$year == $year"

  if       {$::place == "sidebyside"} {
    set title "Male $year"
  } elseif {$::place == "overlay"} {
    set title "Male and Female $year"
  }

  set ::plot1Id [create_plot -type barchart -columns "name=1,value=3" -title $title]

  set_charts_property -plot $::plot1Id -name fill.color -value "#4444aa"
  set_charts_property -plot $::plot1Id -name fill.alpha -value 0.5
  set_charts_property -plot $::plot1Id -name key.visible -value 0

  set filter "sex:2,year:$year"
  set model2Id [load_model -csv data/population.csv -first_line_header -filter $filter]

  #set model2Id [load_model -csv data/population.csv -first_line_header]
  #filter_model -model $model2Id -expr "\$sex == 2 || \$year == $year"

  if       {$::place == "sidebyside"} {
    set title "Female $year"
  } elseif {$::place == "overlay"} {
    set title ""
  }

  set ::plot2Id [create_plot -type barchart -columns "name=1,value=3" -title $title]

  set_charts_property -plot $::plot2Id -name fill.color -value "#aa4444"
  set_charts_property -plot $::plot2Id -name fill.alpha -value 0.5
  set_charts_property -plot $::plot2Id -name key.visible -value 0

  if       {$::place == "sidebyside"} {
    place_plots -horizontal $::plot1Id $::plot2Id
  } elseif {$::place == "overlay"} {
    group_plots -overlay $::plot1Id $::plot2Id
  }
}

set modelId [load_model -csv data/population.csv -first_line_header]
#puts $modelId

set nr [get_charts_data -model $modelId -name num_rows]
#puts $nr
set nc [get_charts_data -model $modelId -name num_columns]
#puts $nc

set ::minYear [get_charts_data -model $modelId -column 0 -name min]
set ::maxYear [get_charts_data -model $modelId -column 0 -name max]

set ::year $::minYear

set ::viewId ""

set ::place "overlay"

plotYear $::year

set ::viewId [get_charts_property -plot $::plot1Id -name viewId]

set dtx [measure_charts_text -view $::viewId -name width -text "xx"]
puts $dtx

set tx [expr {2*$dtx}]
puts $tx

set ty 97

set text1Id [create_text_annotation -view $::viewId -id prev -position [list $tx $ty] -text "Prev" -border 1 -background 1]

set tw [measure_charts_text -view $::viewId -name width -text "Prev"]
set tx [expr {$tx + $tw + $dtx}]
puts $tx

set text2Id [create_text_annotation -view $::viewId -id next -position [list $tx $ty] -text "Next" -border 1 -background 1]

set tw [measure_charts_text -view $::viewId -name width -text "Next"]
set tx [expr {$tx + $tw + 2*$dtx}]
puts $tx

set text3Id [create_text_annotation -view $::viewId -id overlay -position [list $tx $ty] -text "Overlay" -border 1 -background 1]

set tw [measure_charts_text -view $::viewId -name width -text "Overlay"]
set tx [expr {$tx + $tw + $dtx}]
puts $tx

set text4Id [create_text_annotation -view $::viewId -id sidebyside -position [list $tx $ty] -text "Side By Side" -border 1 -background 1]
connect_charts -view $::viewId -from annotationIdPressed -to annotationSlot
