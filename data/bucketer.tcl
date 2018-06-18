set model [load_model -csv data/gaussian.txt -comment_header]

process_model -model $model -add -expr "bucket(0,0.1)" -header bucket
