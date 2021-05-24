# hierarchical model
set model [load_charts_model -csv data/book_revenue_hier.csv -comment_header]

set plot [create_charts_plot -model $model -type sankey -columns {{link 0} {value 1}}]
