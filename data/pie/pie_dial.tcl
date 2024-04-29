proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -csv data/ages.csv -first_line_header -column_type {{{1 integer}}}]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {values 1}} -title "pie chart"]

set_charts_property -plot $plot -name scaling.equal -value 1
set_charts_property -plot $plot -name options.startAngle -value 0
set_charts_property -plot $plot -name options.angleExtent -value 180

set_charts_property -plot $plot -name dial.visible -value 1

set_charts_data -model $model -column 1 -name details.parameter -data current_value -value 10000000

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
