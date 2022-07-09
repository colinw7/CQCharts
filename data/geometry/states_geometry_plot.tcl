proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model1 [load_charts_model -tsv data/states.tsv -comment_header \
  -column_type {{{1 polygon_list}}}]

set plot1 [create_charts_plot -model $model1 -type geometry \
  -columns {{name 0} {geometry 1}} \
  -title "geometry"]

connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

#set model2 [load_charts_model -tsv data/choropeth.tsv]

#set plot2 [create_charts_plot -model $model2 -type geometry \
#  -columns {{name 0} {geometry 1} {value 2}} \
#  -title "choropeth"]

#connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed
