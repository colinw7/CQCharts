# Bubble/Hier Bubble Plot

proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

#set model1 [load_charts_model -json data/flare.json]

#set plot1 [create_charts_plot -model $model1 -type hierbubble \
#  -columns {{name 0} {value 1}} \
#  -title "hierarchical bubble (hier data)"]

#connect_charts_signal -plot $plot1 -from objIdPressed -to objPressed

set model2 [load_charts_model -csv data/flare.csv -comment_header -column_type {{{1 real}}}]

set plot2 [create_charts_plot -model $model2 -type hierbubble \
  -columns {{name 0} {value 1}} \
  -title "hierarchical bubble (flat data)"]

connect_charts_signal -plot $plot2 -from objIdPressed -to objPressed

#set model3 [load_charts_model -csv data/hier.csv -comment_header]

#set plot3 [create_charts_plot -model $model3 -type hierbubble \
#  -columns {{name 0} {value 1}} \
#  -title "hierarchical bubble"]

#connect_charts_signal -plot $plot3 -from objIdPressed -to objPressed

if {0} {
CQChartsTest -csv data/hier_files.csv \
 -type hierbubble -columns {{name 0} {value 1}} \
 -plot_title "hier files"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type hierbubble -columns {{name 0} {color 1}} \
 -plot_title "coffee characteristics"

CQChartsTest -csv data/hier_order.csv -comment_header \
 -type hierbubble -columns {{name 0} {value 1} {color 2}} \
 -plot_title "hierarchical bubble"

CQChartsTest -csv data/book_revenue.csv -first_line_header \
 -type hierbubble -columns {{name {0 1 2}} {value 3}} \
 -plot_title "book revenue"
}
