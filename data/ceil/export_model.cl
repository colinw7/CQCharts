load_model -tsv data/cities1.dat -comment_header
modelId = _rc

print modelId

export_model -model $modelId -to "csv"
export_model -model $modelId -to "tsv"

load_model -tsv data/comma.tsv
modelId = _rc

print modelId

export_model -model $modelId -to "csv"
export_model -model $modelId -to "tsv"
