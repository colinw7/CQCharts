# hierarchical model
set model [load_charts_model -json data/test_hier_attr.json]

set plot [create_charts_plot -model $model -type graphviz -columns {{link 0} {value 1}}]
