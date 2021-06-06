#set model [load_charts_model -csv data/sankey.csv -comment_header \
# -column_type {{{0 name_pair}}}]

set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set maxv [get_charts_data -model $model -column 1 -name details.max]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set nameInd {}
set ind     1
set edges   {}

for {set r 0} {$r < $nr} {incr r} {
  set path  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set names [split $path "/"]

  set from [lindex $names 0]
  set to   [lindex $names 1]

  if {! [dict exists $nameInd $from]} {
    dict set nameInd $from $ind 
    incr ind
  }

  if {! [dict exists $nameInd $to]} {
    dict set nameInd $to $ind 
    incr ind
  }

  lappend edges [list $from $to $value $r]
}

set nn [llength [dict keys $nameInd]]
set nn1 [expr {$nn + 1}]

set plot [create_charts_plot -model $model -type empty -xmin 0 -xmax $nn1 -ymin 0 -ymax $nn1]

set_charts_property -plot $plot -name xaxis.valueType              -value INTEGER
set_charts_property -plot $plot -name xaxis.majorIncrement         -value 1
set_charts_property -plot $plot -name xaxis.ticks.label.text.font  -value -8
set_charts_property -plot $plot -name xaxis.ticks.label.text.angle -value 90
set_charts_property -plot $plot -name xaxis.grid.middle           -value 1
set_charts_property -plot $plot -name xaxis.grid.lines            -value MAJOR

set_charts_property -plot $plot -name yaxis.valueType             -value INTEGER
set_charts_property -plot $plot -name yaxis.majorIncrement        -value 1
set_charts_property -plot $plot -name yaxis.ticks.label.text.font -value -8
set_charts_property -plot $plot -name yaxis.grid.middle           -value 1
set_charts_property -plot $plot -name yaxis.grid.lines            -value MAJOR

set xaxis_obj [get_charts_data -plot $plot -name xaxis_obj]
set yaxis_obj [get_charts_data -plot $plot -name yaxis_obj]

foreach edge $edges {
  set from  [lindex $edge 0]
  set to    [lindex $edge 1]
  set value [lindex $edge 2]
  set row   [lindex $edge 3]

  set fromInd [dict get $nameInd $from]
  set toInd   [dict get $nameInd $to]

  set x1 [expr {$fromInd - 0.5}]
  set y1 [expr {$toInd - 0.5}]
  set x2 [expr {$x1 + 1}]
  set y2 [expr {$y1 + 1}]

  set id "${from}:${to}"

  set tip "$from -> $to = $value"

  set color  [expr {$value/$maxv}]
  set ncolor "palette $color"

  set rect [create_charts_rectangle_annotation -plot $plot -id $id -tip $tip \
             -start [list $x1 $y1] -end [list $x2 $y2]]

  set_charts_property -annotation $rect -name fill.color -value $ncolor

  set ind [list $plot $row 0]

  set_charts_property -annotation $rect -name index -value $ind
}

set_charts_data -plot $plot -object $xaxis_obj -name tick_label -value [list 0 {}]
set_charts_data -plot $plot -object $yaxis_obj -name tick_label -value [list 0 {}]
set_charts_data -plot $plot -object $xaxis_obj -name tick_label -value [list $nn1 {}]
set_charts_data -plot $plot -object $yaxis_obj -name tick_label -value [list $nn1 {}]

dict for { name value } $nameInd {
  set_charts_data -plot $plot -object $xaxis_obj -name tick_label -value [list $value $name]
  set_charts_data -plot $plot -object $yaxis_obj -name tick_label -value [list $value $name]
}
