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

set plot1 [create_charts_plot -model $model -type barchart \
  -columns {{value sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}}]

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed
