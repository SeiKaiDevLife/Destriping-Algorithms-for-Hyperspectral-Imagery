function OutData = fold_mode_13(InData)
[h,w,d] = size(InData);

for ii = 1:d
    for jj = 1:h
        for kk = 1:w
            i = h + 1 - jj;
            j = kk;
            k = ii;
            OutData(i,k,j) = InData(i,j,k);
        end
    end
end

end

