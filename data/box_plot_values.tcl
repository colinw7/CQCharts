proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/scatter.tsv -first_line_header]

set view1 [create_view]
set plot1 [create_plot -view $view1 -model $model -type boxplot -columns "value=0 1 2 3"]

connect_chart -plot $plot1 -from objIdPressed -to objPressed

#set view2 [create_view]
#set plot2 [create_plot -view $view2 -model $model -type boxplot -columns "value=0,group=4"]

create_plot_dlg -model $model

