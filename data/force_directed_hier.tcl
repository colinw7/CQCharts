set model [load_charts_model -json data/flare.json]

set plot [create_charts_plot -model $model -type force_directed -columns {{namePair 0} {count 1}}]
