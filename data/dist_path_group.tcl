set model [load_charts_model -csv data/book_revenue_hier.csv -comment_header]

set model1 [fold_charts_model -model $model -column 0 -separator "/"]

set plot [create_charts_plot -model $model1 -type distribution -columns {{name 0} {values 0}}]

set_charts_property -plot $plot -name dataGrouping.path -value false
