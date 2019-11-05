set model [load_charts_model -csv data/book_revenue_hier.csv -comment_header]

set model1 [fold_charts_model -model $model -column 0 -separator "/"]

set plot [create_charts_plot -model $model1 -type sunburst -columns {{name 0} {color 1}}]

#set model [load_charts_model -tsv data/coffee.tsv -first_line_header -column_type {{{1 color}}}]

#set model1 [fold_charts_model -model $model -column 0 -separator "/"]

#set plot [create_charts_plot -model $model1 -type sunburst -columns {{name 0} {color 1}} \
#  -title "coffee characteristics" -properties {{options.multiRoot 1}}]
