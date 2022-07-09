# TreeMap Plot

set model1 [load_charts_model -json data/flare.json]

set plot1 [create_charts_plot -model $model1 -type treemap \
  -columns {{name 0} {value 1}} -title "tree map"]

#set model2 [load_charts_model -csv data/flare.csv -comment_header]

#set plot2 [create_charts_plot -model $model2 -type treemap -columns {{names 0} {value 1}} -title "tree map"]

#set model3 [load_charts_model -tsv data/coffee.tsv -first_line_header]

#set plot3 [create_charts_plot -model $model3 -type treemap -columns {{names 0} {color 1}} -title "coffee characteristics"]

#set model4 [load_charts_model -tsv data/coffee.tsv -first_line_header \
#  -column_type {{{1 color}}}]

#set plot4 [create_charts_plot -model $model4 -type treemap -columns {{names 0} {color 1}} -title "coffee characteristics"]

#set model5 [load_charts_model -csv data/hier_order.csv -comment_header]

#set plot5 [create_charts_plot -model $model5 -type treemap -columns {{names 0} {value 1}} -title "tree map"]
