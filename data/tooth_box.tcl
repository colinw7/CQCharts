proc objPressed { view plot id } {
  echo "objPressed: $view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

proc annotationSlot { view plot id } {
  echo "annotationSlot: $view $plot $id"
}

proc addAnnotations { view plot } {
  echo "addAnnotations: $view $plot"

  remove_shape -plot $plot -all

  set horizontal [get_property -plot $plot -name options.horizontal]

  set objs [get_charts_data -plot $plot -name objects]

  set points {}

  foreach obj $objs {
    set typeName [get_property -plot $plot -object $obj -name typeName]

    if {$typeName == "whisker"} {
      set pos    [get_property -plot $plot -object $obj -name pos   ]
      set mean   [get_property -plot $plot -object $obj -name mean  ]
    # set stddev [get_property -plot $plot -object $obj -name stddev]

      if {! $horizontal} {
        lappend points [list $pos $mean]
      } else {
        lappend points [list $mean $pos]
      }
    }
  }

  set polyId [create_polyline_shape -plot $plot -id mean_line -points $points \
   -background 1 -background_color red -background_alpha 0.5]
}

set model [load_model -csv data/ToothGrowth.csv -first_line_header]

set view [create_view]
sync_qt

set plot1 [create_plot -view $view -model $model -type boxplot -columns "group=dose,value=len"]
sync_qt

connect_chart -plot $plot1 -from plotObjsAdded -to addAnnotations

connect_chart -plot $plot1 -from objIdPressed -to objPressed

connect_chart -plot $plot1 -from annotationIdPressed -to annotationSlot
