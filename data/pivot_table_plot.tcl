proc objPressed { view plot id } {
  #echo "$view $plot $id"

  set rows [get_charts_data -plot $plot -object $id -name rows]

  #echo "$rows"

  set_charts_property -plot $::plot2 -name options.mode    -value ROWS
  set_charts_property -plot $::plot2 -name options.rowNums -value $rows
}

# Name,Party,Province,Age,Gender
set model [load_charts_model -csv data/mps.csv -first_line_header]

set plot1 [create_charts_plot -type pivot -columns {{x 2} {y 1} {value 3}}]

set plot2 [create_charts_plot -type table -columns {{columns {0 1 2 3 4}}}]

set_charts_property -plot $plot1 -name range.view -value {0 25 100 100}
set_charts_property -plot $plot2 -name range.view -value {0 0 100 25}

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed
