set model [load_charts_model -csv data/pareto.csv -comment_header -spreadsheet]

set view [create_charts_view]

set plot [create_charts_plot -model $model -type bar -columns {{name 0} {values 1}}]
