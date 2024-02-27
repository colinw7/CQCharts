set model [load_charts_model -csv data/one.csv -comment_header]

set plot [create_charts_plot -model $model -type dendrogram -columns {{name 0} {value 1}}]
