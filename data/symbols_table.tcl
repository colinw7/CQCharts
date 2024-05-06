proc objPressed { view plot id } {
  set inds [get_charts_data -plot $plot -object $id -name inds]
  if {[llength $inds] < 1} { return }

  set ind [lindex $inds 0]

  set model [get_charts_data -plot $plot -name model]

  set name [get_charts_data -model $model -name value -ind $ind]

  set_charts_property -annotation $::pointId -name symbol.symbol -value "$name"

  set_charts_property -annotation $::textId -name text.string -value "Symbol $name"
}

proc fillSlot { viewId id } {
  echo "fillSlot $viewId $id"

  if {$id == "imageToggle"} {
    set checked [get_charts_property -annotation $::toggleId -name checked]

    set checked1 [expr {1 - $checked}]

    set_charts_property -annotation $::toggleId -name checked -value $checked1

    if {$checked1} {
      set_charts_property -annotation $::toggleId -name text.string \
        -value "<font color='gray'>Stroked</color> <font color='blue'><b>Filled</b></font>"
    } else {
      set_charts_property -annotation $::toggleId -name text.string \
        -value "<font color='blue'><b>Stroked</b></font> <font color='gray'>Filled</color>"
    }

    set_charts_property -annotation $::pointId -name fill.visible -value $checked1
  }
}

set symbols [get_charts_data -name symbols]

set model [load_charts_model -tcl [list $symbols]]

set plot [create_charts_plot -type table -model $model -columns {{columns {0}}}]

set_charts_property -plot $plot -name header.visible -value 0

set view [get_charts_data -plot $plot -name view]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed

set textId [create_charts_text_annotation -view $view -id symbolText -tip "Symbol Text" \
  -position {10 80} -text "Symbol"]

set pointId [create_charts_point_annotation -view $view -id symbolPoint -tip "Symbol Point" \
  -position {20 50} -symbol cross -size 240px -properties {{fill.color green}}]

set toggleId [create_charts_text_annotation -view $view -id imageToggle -tip "Filled" \
  -position {10 20} -text "Filled" -properties {{checkable 1} {checked 0}}]

connect_charts_signal -view $view -from annotationPressed -to fillSlot

set_charts_property -annotation $::toggleId -name text.html                 -value 1 
set_charts_property -annotation $::toggleId -name coloring.uncheckedLighter -value 0.0 

set_charts_property -annotation $::toggleId -name text.string \
  -value "<font color='blue'><b>Stroked</b></font> <font color='gray'>Filled</color>"
