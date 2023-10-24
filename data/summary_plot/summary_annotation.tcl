set model [load_charts_model -tsv data/scatter.tsv -first_line_header]
set plot [create_charts_plot -type summary -model $model -columns {{group 4} {columns {0 1 2 3}}}]

set_charts_property -plot $plot -name options.selectMode -value DATA

set_charts_property -plot $plot -name xaxis.visible -value 1
set_charts_property -plot $plot -name yaxis.visible -value 1

connect_charts_signal -plot $plot -from newPlotObjsDrawn -to add_annotations

proc add_annotations { view plot } {
  set_charts_data -plot $plot -name current_obj -value "cell:0:2"
  echo [get_charts_data -plot $plot -name current_obj]

  remove_charts_annotation -plot $plot -all

# set p1 [get_charts_data -plot $plot -name subplot_to_plot -data {0 2 2 5}]
# set p2 [get_charts_data -plot $plot -name subplot_to_plot -data {0 2 4 6}]
  set p1 [get_charts_data -plot $plot -name subplot_to_plot -data {2 5}]
  set p2 [get_charts_data -plot $plot -name subplot_to_plot -data {4 6}]
echo $p1
echo $p2

  set rect [create_charts_rectangle_annotation -plot $plot -start $p1 -end $p2]

# set p3 [get_charts_data -plot $plot -name subplot_to_plot -data {0 2 5 7}]
  set p3 [get_charts_data -plot $plot -name subplot_to_plot -data {5 7}]
echo $p3

  set point [create_charts_point_annotation -plot $plot -position $p3 -symbol star -size 32px]
}
