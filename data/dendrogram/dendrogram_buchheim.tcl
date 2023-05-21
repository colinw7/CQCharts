# Dendrogram

set theme3 [create_charts_palette -theme theme3]

set_charts_palette -theme $theme3 -name desc -value "Theme 3"

set palette [create_charts_palette -palette green_red_1]

set_charts_palette -palette green_red_1 -name defined_colors -value [list {0 green} {1 red}]

set colors [get_charts_palette -palette green_red_1 -name defined_colors]

set_charts_palette -theme theme3 -name palettes -value [list green_red_1]

###---

set model [load_charts_model -csv data/flare.csv -comment_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{name 0} {value 1} {color 1} {size 1}} -title "dendrogram"]

set_charts_property -plot $plot -name options.placeType -value BUCHHEIM

###---

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name theme.name -value theme3
set_charts_property -plot $plot -name coloring.defaultPalette -value green_red_1
