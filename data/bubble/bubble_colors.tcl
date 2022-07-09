set model [load_charts_model -csv data/bubble.csv -comment_header]

set plot [create_charts_plot -model $model -type bubble \
  -columns {{name ID} {value {Population}} {color {Fertility Rate}}} \
  -title "Countries Life Expectancy"]
