set model [load_charts_model -csv data/treemap_html.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram -columns {{label 2} {name 0} {value 1}}]

set_charts_property -plot $plot -name hier.size -value 0.1P
set_charts_property -plot $plot -name leaf.size -value 0.1P

set_charts_property -plot $plot -name leaf.label.html -value 1

set_charts_property -plot $plot -name options.hideDepth -value 3
