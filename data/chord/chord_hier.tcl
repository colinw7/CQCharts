# hierarchical model
set model [load_charts_model -json data/flare.json]

set plot [create_charts_plot -model $model -type chord -columns {{link 0} {value 1}}]
