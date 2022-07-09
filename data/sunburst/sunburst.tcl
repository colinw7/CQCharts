#set model1 [load_charts_model -json data/flare.json]

#set plot1 [create_charts_plot -model $model1 -type sunburst -columns {{name 0} {value 1}} -title "sunburst"]

#set model2 [load_charts_model -csv data/flare.csv -comment_header]

#set plot2 [create_charts_plot -model $model2 -type sunburst -columns {{name 0} {value 1}} -title "sunburst"]

set model3 [load_charts_model -tsv data/coffee.tsv -first_line_header \
  -column_type {{{1 color}}}]

set plot3 [create_charts_plot -model $model3 -type sunburst \
  -columns {{name 0} {color 1}} -title "coffee characteristics" \
  -properties {{options.multiRoot 1}}]

#set model4 [load_charts_model -csv data/book_revenue.csv -first_line_header]

#set plot4 [create_charts_plot -model $model4 -type sunburst \
  -columns {{name {0 1 2} {value 3}} -title "book revenue"]
