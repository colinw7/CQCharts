proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"

  set strs [split $id ":"]

  set type [lindex $strs 0]

  if       {"$type" == "point"} {
    set x [get_charts_property -plot $plot -object $id -name x]
    set y [get_charts_property -plot $plot -object $id -name y]

    echo "$x $y"
  } elseif {"$type" == "impulse"} {
    set x  [get_charts_property -plot $plot -object $id -name x ]
    set y1 [get_charts_property -plot $plot -object $id -name y1]
    set y2 [get_charts_property -plot $plot -object $id -name y2]

    echo "$x $y1 $y2"
  }
}

set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set plot [create_charts_plot -type xy -columns {{group 0} {x 1} {y 2}} -title "Group XY Plot"]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
