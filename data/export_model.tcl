set modelId [load_charts_model -tsv data/cities1.dat -comment_header]

puts $modelId

export_charts_model -model $modelId -to "csv"
export_charts_model -model $modelId -to "tsv"

set modelId [load_charts_model -tsv data/comma.tsv]

puts $modelId

export_charts_model -model $modelId -to "csv"
export_charts_model -model $modelId -to "tsv"
