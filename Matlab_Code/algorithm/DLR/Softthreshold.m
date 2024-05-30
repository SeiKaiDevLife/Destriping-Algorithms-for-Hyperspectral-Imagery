function Y = Softthreshold(X, thr)

Y = sign(X).* max( abs(X) - thr, 0);


end