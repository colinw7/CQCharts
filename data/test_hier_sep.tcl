set model [load_charts_model -csv data/test_hier_sep.csv -first_line_header]

set plot [create_charts_plot -type barchart -columns {{name 0} {values 1}}]
