# link column model

# Path,From,Group,To
set model [load_charts_model -csv data/chord-cities.csv -comment_header]

set plot [create_charts_plot -model $model -type adjacency \
  -columns {{link Path} {value From} {group Group} {color Group}} -title "adjacency"]
