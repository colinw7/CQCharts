proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x petalLength} {y sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}} \
  -title "Scatter Group"]

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

#set plot2 [create_charts_plot -model $model -type scatter \
#  -columns {{x sepalLength} {y sepalWidth} {group species} {color species}} \
#  -properties {{xaxis.userLabel {Sepal Length}} {yaxis.userLabel {Sepal Width}}}]

#connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed

#place_charts_plots -horizontal [list $plot1 $plot2]
