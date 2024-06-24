proc modelChanged { model } {
  set v [get_charts_data -model $model -row 0 -column 0 -name value]

  updateDial $v
}

proc updateDial { v } {
  set_charts_property -annotation $::arc -name spanAngle -value [expr {-180*$v}]

  set mph [expr {int(100*$v)}]

  set_charts_property -annotation $::text -name text.string -value "$mph mph"
}

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set axis [create_charts_axis_annotation -plot $plot -start 0 -end 100 -position 0.5]

# center 0, 0, radius 0.8
set_charts_property -annotation $axis -name path \
  -value "M -0.8 0.0 C -0.8 0.4 -0.4 0.8 0 0.8 C 0.4 0.8 0.8 0.4 0.8 0.0"
set_charts_property -annotation $axis -name valueType \
  -value INTEGER

# center 0, 0, radius 0.6
set rect [create_charts_rectangle_annotation -plot $plot -rectangle {-0.6 -0.6 0.6 0.6}]

set_charts_property -annotation $rect -name shapeType -value CIRCLE

set text [create_charts_text_annotation -plot $plot -position {0 -0.2} -text "56 mph"]

set_charts_property -annotation $text -name text.font -value +12
set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
set_charts_property -annotation $text -name text.color -value black
set_charts_property -annotation $text -name text.contrast -value 1

set_charts_property -annotation $rect -name textInd -value $text

set arc [create_charts_pie_slice_annotation -plot $plot \
  -position {0 0} -inner_radius 0.3 -outer_radius 0.6 -start_angle 180 -span_angle -120]

set_charts_property -annotation $arc -name fill.color -value "#448844"

updateDial 0

#---

set vmodel [create_charts_random_model -rows 1 -columns 1 -ranges {{0.0 1.0}}]

connect_charts_signal -model $vmodel -from modelChanged -to modelChanged
