# https://observablehq.com/@mbostock/hertzsprung-russell-diagram?collection=@observablehq/visualization
set model [load_charts_model -csv data/star_catalog.csv -first_line_header]

set plot [create_charts_plot -type scatter -columns {{x 1} {y 0}} -title "Star Catalog"]

set_charts_property -plot $plot -name symbol.type -value dot
