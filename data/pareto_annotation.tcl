set model [load_charts_model -csv data/pareto.csv -comment_header]

set_charts_data -model $model -column 1 -name column_type -value real

set y1 [process_charts_model -calc -expr {column(1)+$_}]

set total [lindex $y1 end]

echo $y1

set y2 [process_charts_model -vars [list [list total $total]] -calc -expr "100.0*column(1)/$total"]

echo $y2

set i 0

set ny [llength $y2]

set points {}

for {set i 0} {$i < $ny} {incr i} {
  set y [lindex $y2 $i]

  set xy [list $i $y]

  lappend points $xy
}

echo $points

set x [list 0 [expr {$ny - 1}]]
set y { 0 100 }

set modelId [load_charts_model -tcl [list $x $y] -first_line_header]

set plotId [create_charts_plot -type empty \
  -xmin [lindex $x 0] -xmax [lindex $x 1] \
  -ymin [lindex $y 0] -ymax [lindex $y 1] \
  -title "Percentage Points"]

set pointsId [create_charts_polyline_annotation -plot $plotId -points $points]
