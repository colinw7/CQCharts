set model [load_charts_model -csv data/book_revenue_hier.csv -comment_header]

set model1 [fold_charts_model -model $model -column 0 -separator "/"]

set plot [create_charts_plot -type table -model $model1 -columns {{columns {0 1}}}]
