set model [load_charts_model -csv data/venn.csv -comment_header]

set plot [create_charts_plot -model $model -type venn \
  -columns {{name 0} {value 1}} -title "Venn Plot"]
