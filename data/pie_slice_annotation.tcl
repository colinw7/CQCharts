set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set values [list 10 20 30 25 5 45 65 25 20]

set pi [expr {atan(1.0)*4}]

set total 0

foreach v $values {
  set total [expr {$total + $v}]
}

set n [llength $values]

set ga 2.0

set da [expr {(360.0 - $n*$ga)/$total}]

set a 0

foreach v $values {
  set a1 [expr {$da*$v}]

  set ann [create_charts_pie_slice_annotation -plot $plotId -position {50 50} \
   -inner_radius 10 -outer_radius 40 -start_angle $a -span_angle $a1]

  set px [expr {$a1/360.0}]

  set_charts_property -annotation $ann -name fill.color -value "palette $px"

  set a [expr {$a + $a1 + $ga}]
}
