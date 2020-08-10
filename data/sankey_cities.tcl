# link column model
set model [load_charts_model -csv data/chord-cities.csv -comment_header]

set plot [create_charts_plot -model $model -type sankey \
  -columns {{link 0} {value 1} {group 2} {color 3} {controls 2}} -title "sankey"]
