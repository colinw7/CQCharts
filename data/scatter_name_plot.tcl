set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1} {name 4}} \
 -title "Named Sepal Length/Width"]
