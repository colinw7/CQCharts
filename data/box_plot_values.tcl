proc objPressed { view plot id } {
  #echo "$view $plot $id"

  set model [get_charts_data -plot $plot -name model]

  set inds [get_charts_data -plot $plot -object $id -name inds]

  foreach ind $inds {
    set id [get_charts_data -model $model -ind $ind -name value]

    echo $id
  }
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set view1 [create_charts_view]
set plot1 [create_charts_plot -view $view1 -model $model -type boxplot -columns {{value {0 1 2 3}}}]

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

#set view2 [create_charts_view]
#set plot2 [create_charts_plot -view $view2 -model $model -type boxplot \
#  -columns {{value 0} {group 4}}]

show_charts_create_plot_dlg -model $model
