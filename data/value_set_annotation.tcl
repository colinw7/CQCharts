proc randIn { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

proc setValues { } {
  set ::values {}

  for {set i 0} {$i < $::n} {incr i} {
    lappend ::values [randIn 10 100]
  }
}

proc nSpinChanged { i } {
  set ::n $i

  #echo $::n

  setValues

  set_charts_property -annotation $::valueSetAnn -name values -value $::values
}

#---

set n 9

setValues

set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set valueSetAnn [create_charts_value_set_annotation -plot $plot \
                   -rectangle {25 25 75 75} -values $values]

set_charts_property -annotation $valueSetAnn -name drawType -value FACTOR

set_charts_property -plot $plot -name animation.running -value 1
set_charts_property -plot $plot -name animation.tick    -value 20

#---

# add control frame
set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

set nSpin [qt_create_widget -type CQIntegerSpin -name n]

qt_set_property -object $nSpin -property value -value $n
qt_set_property -object $nSpin -property minimum -value 1
qt_set_property -object $nSpin -property maximum -value 999

qt_connect_widget -name $nSpin -signal "valueChanged(int)" -proc nSpinChanged

qt_add_child_widget -parent $frame -child $nSpin

#---

set widgetAnn [create_charts_widget_annotation -plot $plot -id n \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $widgetAnn -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set nSpin "$view_path|frame|$nSpin"
