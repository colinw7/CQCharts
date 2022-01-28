# Dendrogram

set model1 [load_charts_model -csv data/sankey_energy.csv -comment_header]

set plot1 [create_charts_plot -model $model1 -type dendrogram \
  -columns {{link 0} {value 1}} -title "dendrogram"]
