proc modelChanged { view plot } {
  #echo "modelChanged $view $plot"

  set modelInd [get_charts_property -plot $plot -name state.currentModelInd]

  if       {$modelInd == 1} {
    set_charts_property -plot $plot -name title.text.string -value "New York Temperatues"
  } elseif {$modelInd == 2} {
    set_charts_property -plot $plot -name title.text.string -value "San Francisco Temperatues"
  } elseif {$modelInd == 3} {
    set_charts_property -plot $plot -name title.text.string -value "Austin Temperatues"
  }
}

set model1 [load_charts_model -tsv data/multi_series_1.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]
set model2 [load_charts_model -tsv data/multi_series_2.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]
set model3 [load_charts_model -tsv data/multi_series_3.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -type scatter -model $model1 \
  -columns {{x 0} {y 1}} -title "City Temperatures"]

set_charts_data -plot $plot -name extra_model -value $model2
set_charts_data -plot $plot -name extra_model -value $model3

connect_charts_signal -plot $plot -from currentModelChanged -to modelChanged

set view [get_charts_data -plot $plot -name view]

modelChanged $view $plot
