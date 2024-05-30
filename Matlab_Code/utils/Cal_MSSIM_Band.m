function dResult = Cal_MSSIM_Band(OriData, PreData, iBand)
OriData = double(OriData);
PreData = double(PreData);
[h,w,~]=size(OriData);
dResult = 0;

dsumOri = 0;
dsumPre = 0;
dsumOri2 = 0;
dsumPre2 = 0;
dsumPlus = 0;
dmin = 0;
dmax = 0;
for jj = 1:h
    for kk = 1:w
        if(OriData(jj,kk,iBand) < dmin)
            dmin = OriData(jj,kk,iBand);
        end
        if(OriData(jj,kk,iBand) > dmax)
            dmax = OriData(jj,kk,iBand);
        end
    end
end

for jj = 1:h
    for kk = 1:w
        OriData(jj,kk,iBand) = (OriData(jj,kk,iBand) - dmin) / (dmax - dmin);
        PreData(jj,kk,iBand) = (PreData(jj,kk,iBand) - dmin) / (dmax - dmin);
        
        dsumOri = dsumOri + OriData(jj,kk,iBand);
        dsumOri2 = dsumOri2 + OriData(jj,kk,iBand) * OriData(jj,kk,iBand);
        dsumPre = dsumPre + PreData(jj,kk,iBand);
        dsumPre2 = dsumPre2 + PreData(jj,kk,iBand) * PreData(jj,kk,iBand);
        dsumPlus = dsumPlus + OriData(jj,kk,iBand) * PreData(jj,kk,iBand);
    end
end
daveOri = dsumOri / (h * w);
daveOri2 = dsumOri2 / (h * w);
davePre = dsumPre / (h * w);
davePre2 = dsumPre2 / (h * w);
davePlus = dsumPlus / (h * w);

dvarOri = daveOri2 - daveOri * daveOri;
dvarPre = davePre2 - davePre * davePre;

dcov = davePlus - daveOri * davePre;

dResult = dResult + ((2 * daveOri * davePre + 0.0001) * (2 * dcov + 0.0003)) / ((daveOri * daveOri + davePre * davePre + 0.0001) * (dvarOri + dvarPre + 0.0003));
end