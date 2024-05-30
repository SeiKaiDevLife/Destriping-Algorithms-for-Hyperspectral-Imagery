function Y = Softthreshold(X, thr)

[h,w,d] = size(X);
for i = 1:h
    for j = 1:w
        for k = 1:d
            if X(i,j,k) > thr
                Y(i,j,k) = X(i,j,k) - thr;
            elseif X(i,j,k) < 0 - thr
                Y(i,j,k) = X(i,j,k) + thr;
            else
                Y(i,j,k) = 0;
            end
        end
    end
end


end