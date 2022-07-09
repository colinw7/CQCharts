set model [load_charts_model -csv data/bubble.csv -comment_header]

set plot [create_charts_plot -model $model -type pie \
  -columns {{label ID} {values {{Population}}} {group Region} {color {Fertility Rate}}} \
  -title "pie chart"]
