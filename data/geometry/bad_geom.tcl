set model [load_charts_model -csv data/bad_geom.csv -first_line_header]

set plot [create_charts_plot -type geometry -columns {{name 0} {geometry 1}}]
