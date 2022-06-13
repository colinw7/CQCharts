# state,code,2020/12/17,2020/12/16,2020/12/15,2020/12/14,2020/12/13,2020/12/12,2020/12/11
set model [load_charts_model -csv data/grid_covid.csv -first_line_header]

set row_column [process_charts_model -model $model -add -expr {0} -header Row]
set col_column [process_charts_model -model $model -add -expr {0} -header Col]

# col,row,code
set map_model [load_charts_model -csv data/us_grid_map.csv -first_line_header]

set nr [get_charts_data -model $map_model -name num_rows]

for {set r 0} {$r < $nr} {incr r} {
  set col   [get_charts_data -quiet -model $map_model -row $r -column 0 -name value]
  set row   [get_charts_data -quiet -model $map_model -row $r -column 1 -name value]
  set state [get_charts_data -quiet -model $map_model -row $r -column 2 -name value]

  if {$state == ""} {
    continue
  }

  set state_row($state) $row
  set state_col($state) $col
}

set nr [get_charts_data -model $model -name num_rows]

for {set r 0} {$r < $nr} {incr r} {
  set state [get_charts_data -model $model -row $r -column 1 -name value]

  set row -1
  set col -1

  if {[info exists state_row($state)]} {
    set row $state_row($state)
    set col $state_col($state)
  }

  set_charts_data -model $model -row $r -column $row_column -name value -value $row
  set_charts_data -model $model -row $r -column $col_column -name value -value $col
}

set plot [create_charts_plot -type grid -model $model \
  -columns {{name 0} {label 1} {values {2 3 4 5 6 7 8}}}]

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name fade.enabled -value 1
set_charts_property -view $view -name fade.alpha   -value 0.5

set_charts_property -plot $plot -name columns.row    -value $row_column
set_charts_property -plot $plot -name columns.column -value $col_column

set_charts_property -plot $plot -name coloring.defaultPalette -value pinks

set_charts_property -plot $plot -name options.cellPalette -value set3
