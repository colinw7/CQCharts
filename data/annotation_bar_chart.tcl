set values [list 10 20 30 25 5 45 65 25 20]

set n [llength $values]

set plot [create_charts_plot -type empty -title "Bar Chart" \
  -xmin -0.5 -xmax [expr {$n - 0.5}] -ymin 0 -ymax 70]

set_charts_property -plot $plot -name xaxis.valueType -value INTEGER

set x 0

foreach v $values {
  set x1 [expr {$x - 0.5}]
  set x2 [expr {$x + 0.5}]

  set bar [create_charts_rectangle_annotation -plot $plot -rectangle [list $x1 0 $x2 $v]]

  set px [expr {(1.0*$x)/$n}]

  set_charts_property -annotation $bar -name fill.color -value "palette $px"
  set_charts_property -annotation $bar -name margin -value "4px 0px 4px 0px"

  incr x
}
