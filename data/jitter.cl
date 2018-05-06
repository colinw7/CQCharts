nsamp = 160

x = arrayR(nsamp)
y = arrayR(nsamp)

rrand(0)

for (i = 1; i < nsamp; ++i)
  x[i] =       invnorm(rrand())
  y[i] = 100.0*invnorm(rrand())
endfor

i = int(x)

load -var i -var y
modelId = _rc

add_plot -type scatter -columns "x=0,y=1"
plotInd = _rc
