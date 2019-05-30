# Bubble/Hier Bubble Plot

proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

#set model1 [load_charts_model -json data/flare.json]

#set plot1 [create_charts_plot -model $model1 -type bubble \
#  -columns {{name 0} {value 1}} \
#  -title "bubble"]

#connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

#set model2 [load_charts_model -csv data/flare.csv -comment_header -column_type {{{1 real}}}]

#set plot2 [create_charts_plot -model $model2 -type bubble \
#  -columns {{name 0} {value 1}} \
#  -title "bubble"]

#connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed

#set model3 [load_charts_model -csv data/pareto.csv -comment_header]

#set plot3 [create_charts_plot -model $model3 -type bubble \
#  -columns {{name 0} {value 1}} \
#  -title "bubble"]

#connect_charts_signal -plot $plot3 -from objIdPressed -to objPressed

set model4 [load_charts_model -csv data/flare1.csv -comment_header -column_type {{{2 real}}}]

set plot4 [create_charts_plot -model $model4 -type bubble \
  -columns {{name 1} {value 2}} \
  -title "bubble"]

connect_charts_signal -plot $plot4 -from objIdPressed -to objPressed
