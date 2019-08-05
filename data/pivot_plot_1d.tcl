# Name,Party,Province,Age,Gender
set model [load_charts_model -csv data/mps.csv -first_line_header]

set plot [create_charts_plot -type pivot -columns {{x 2} {value 1}} -title "Pivot Plot" \
           -properties {{options.valueType COUNT}}]
