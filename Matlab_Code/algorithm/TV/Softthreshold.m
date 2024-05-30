function Y = Softthreshold(X, thr)

[h,w] = size(X);
for i = 1:h
    for j = 1:w
        if X(i,j) > thr
            Y(i,j) = X(i,j) - thr;
        elseif X(i,j) < 0 - thr
            Y(i,j) = X(i,j) + thr;
        else
            Y(i,j) = 0;
        end
    end
end


end