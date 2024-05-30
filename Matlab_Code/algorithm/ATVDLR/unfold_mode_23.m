function OutData = unfold_mode_23(InData)
[h,w,d] = size(InData);

for ii = 1:d
    for jj = 1:h
        for kk = 1:w
            i = h + 1 - jj;
            j = kk;
            k = ii;
            OutData(j,k,i) = InData(i,j,k);
        end
    end
end

end