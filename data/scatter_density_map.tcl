proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x sepalLength} {y sepalWidth}} \
  -properties {{xaxis.userLabel {Sepal Length}} {yaxis.userLabel {Sepal Width}} \
               {densityMap.enabled 1}}]

connect_charts_signal -plot $plot -from objIdPressed -to objPressed
