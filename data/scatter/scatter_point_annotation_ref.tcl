proc eset { var exp } {
  upvar $var var1

  set var [expr $exp]
}

proc tcl::mathfunc::max { a b } {
  if {$a > $b} {
    return $a
  } else {
    return $b
  }
}

proc tcl::mathfunc::min { a b } {
  if {$a < $b} {
    return $a
  } else {
    return $b
  }
}

set_charts_data -name path_list -value [list /home/colinw/dev/progs/charts/data/CQCharts/data /home/colinw/dev/icons/svg .]

create_charts_symbol_set -name point_symbols

set min_symbol [add_charts_symbol -set point_symbols -svg weather_snow.svg -name weather_snow -filled 1]
set max_symbol [add_charts_symbol -set point_symbols -svg weather_clear.svg  -name weather_clear -filled 1]

echo $min_symbol
echo $max_symbol

set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set column 1

# get min max value from model column
set nr [get_charts_data -model $model -name num_rows]

set min_value -1
set max_value -1

for {set r 0} {$r < $nr} {incr r} {
  set value [get_charts_data -model $model -column $column -row $r -name value -role display]

  if {$min_value < 0} {
    set min_value $value
    set max_value $value
  } else {
    set min_value [expr {min($min_value, $value)}]
    set max_value [expr {max($max_value, $value)}]
  }
}

echo "$min_value $max_value"

set plot [create_charts_plot -type scatter -columns [list [list x 0] [list y $column]] \
  -title "New York Temperatures"]

set min_point [create_charts_point_annotation -plot $plot -position {24 -24 px}]
set max_point [create_charts_point_annotation -plot $plot -position {24 24 px}]

set_charts_property -annotation $min_point -name symbol.symbol -value $min_symbol
set_charts_property -annotation $min_point -name symbol.size   -value "24px"
set_charts_property -annotation $min_point -name fill.color    -value "blue"

set_charts_property -annotation $max_point -name symbol.symbol -value $max_symbol
set_charts_property -annotation $max_point -name symbol.size   -value "24px"
set_charts_property -annotation $max_point -name fill.color    -value "red"

connect_charts_signal -plot $plot -from plotObjsAdded -to updateAnnotations

proc updateAnnotations { view plot } {
  set objs [get_charts_data -plot $plot -name objects]

  set points {}

  foreach obj $objs {
    set typeName [get_charts_property -plot $plot -object $obj -name typeName]

    if {$typeName == "point"} {
      set p [get_charts_property -plot $plot -object $obj -name point]

      set value [lindex $p 1]

      if {$value == $::max_value} {
        set_charts_property -annotation $::max_point -name objRef -value [list $obj center]
      }

      if {$value == $::min_value} {
        set_charts_property -annotation $::min_point -name objRef -value [list $obj center]
      }
    }
  }
}
