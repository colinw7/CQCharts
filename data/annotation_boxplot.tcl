set values1 [list 850 740 900 1070 930 850 950 980 980 880 1000 980 930 650 760 810 1000 1000 960 960]

set values2 [list 960 940 960 940 880 800 850 880 900 840 830 790 810 880 880 830 800 790 760 800]

set values3 [list 880 880 880 860 720 720 620 860 970 950 880 910 850 870 840 840 850 840 840 840]

set values4 [list 890 810 810 820 800 770 760 740 750 760 910 920 890 860 880 720 840 850 850 780]

set values5 [list 890 840 780 810 760 810 790 810 820 850 870 870 810 740 810 940 950 800 810 870]

set values [list $values1 $values2 $values3 $values4 $values5]

set n [llength $values]

set xmin 0
set ymin -0.1
set xmax [expr {$n  + 1}]
set ymax 1.1

set plot [create_charts_plot -type empty -xmin $xmin -ymin $ymin -xmax $xmax -ymax $ymax]

set_charts_property -plot $plot -name xaxis.valueType -value INTEGER

set x 1

foreach values $values {
  set x1 [expr {$x - 0.1}]
  set x2 [expr {$x + 0.1}]

  set rect [list $x1 0 $x2 1]

  set ann [create_charts_value_set_annotation -plot $plot -rectangle $rect -values $values]

  set_charts_property -annotation $ann -name density.drawType    -value WHISKER_BAR
  set_charts_property -annotation $ann -name density.orientation -value Vertical

  set px [expr {(1.0*($x - 1))/$n}]

  set_charts_property -annotation $ann -name fill.color -value "palette $px"

  incr x
}
