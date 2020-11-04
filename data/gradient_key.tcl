proc updateModel { } {
  set_charts_data -model $::model -name size -value [list $::n 5]

  set_charts_property -plot $::plot -name range.xmin -value $::xmin
  set_charts_property -plot $::plot -name range.ymin -value $::ymin
  set_charts_property -plot $::plot -name range.xmax -value $::xmax
  set_charts_property -plot $::plot -name range.ymax -value $::ymax

  set f 1.0

  if {$::n > 0} {
    set f [expr {1.0*($::xmax - $::xmin)/$::n}]
  }

  set_charts_data -model $::model -name value -header -column 0 -value "Index"
  set_charts_data -model $::model -name value -header -column 1 -value "X"
  set_charts_data -model $::model -name value -header -column 2 -value "cos(x)"
  set_charts_data -model $::model -name value -header -column 3 -value "sin(x)"
  set_charts_data -model $::model -name value -header -column 4 -value "tan(x)"

  for {set i 0} {$i < $::n} {incr i} {
    set x [expr {$i*$f + $::xmin}]
    set c [expr {cos($x)}]
    set s [expr {sin($x)}]
    set t [expr {tan($x)}]

    set_charts_data -model $::model -name value -row $i -column 0 -value $i
    set_charts_data -model $::model -name value -row $i -column 1 -value $x
    set_charts_data -model $::model -name value -row $i -column 2 -value $c
    set_charts_data -model $::model -name value -row $i -column 3 -value $s
    set_charts_data -model $::model -name value -row $i -column 4 -value $t
  }

  set_charts_property -key_item "keyitem:gradient:gradient" -name minValue -value $::ymin
  set_charts_property -key_item "keyitem:gradient:gradient" -name maxValue -value $::ymax
}

proc annotationSlot { viewId plotId id } {
  echo "annotationSlot: $viewId, $plotId, $id"
}

proc nSpinChanged { i } {
  set ::n $i

  updateModel
}

proc xminSpinChanged { r } {
  set ::xmin $r

  updateModel
}

proc xmaxSpinChanged { r } {
  set ::xmax $r

  updateModel
}

set pi [expr {atan(1)*4.0}]

set xmin 0.0
set xmax [expr {2*$pi}]

set ymin -1.0
set ymax 1.0

set n 100

set model [create_charts_data_model -rows $n -columns 5]

set plot [create_charts_plot -type xy -model $model \
  -xmin $xmin -ymin -1 -xmax $xmax -ymax 1 -columns {{x 1} {y {2 3 4}}}]

set_charts_property -plot $plot -name points.visible -value 1
set_charts_property -plot $plot -name points.symbol.fill.visible -value 1
set_charts_property -plot $plot -name points.symbol.fill.color -value palette
set_charts_property -plot $plot -name coloring.type -value Y_VALUE

#---

# add control frame
set frame [qt_create_widget -type QFrame -name frame]

set layout [qt_create_layout -type QVBoxLayout -parent $frame]

set xminSpin [qt_create_widget -type CQRealSpin    -name xmin]
set xmaxSpin [qt_create_widget -type CQRealSpin    -name xmax]
set nSpin    [qt_create_widget -type CQIntegerSpin -name n]

qt_set_property -object $xminSpin -property value -value $xmin
qt_set_property -object $xmaxSpin -property value -value $xmax
qt_set_property -object $nSpin    -property value -value $n

qt_connect_widget -name $xminSpin -signal "valueChanged(double)" -proc xminSpinChanged
qt_connect_widget -name $xmaxSpin -signal "valueChanged(double)" -proc xmaxSpinChanged
qt_connect_widget  -name $nSpin   -signal "valueChanged(int)"    -proc nSpinChanged

qt_add_child_widget -parent $frame -child $xminSpin
qt_add_child_widget -parent $frame -child $xmaxSpin
qt_add_child_widget -parent $frame -child $nSpin

#---

set ann [create_charts_widget_annotation -plot $plot -id n \
  -position [list 100 100 V] -widget $frame]

set_charts_property -annotation $ann -name align -value {AlignRight|AlignTop}

set view_path [get_charts_data -plot $plot -name view_path]

set xminSpin "$view_path|frame|$xminSpin"
set xmaxSpin "$view_path|frame|$xmaxSpin"
set nSpin    "$view_path|frame|$nSpin"

#connect_charts_signal -plot $plot -from annotationIdPressed -to annotationSlot

#---

# add gradient key
set key_annotation [create_charts_key_annotation -plot $plot -id key]

set_charts_property -annotation $key_annotation -name key.location -value TOP_LEFT

add_charts_key_item -annotation $key_annotation -text Gradient -gradient moreland -id "gradient"

#---

updateModel
