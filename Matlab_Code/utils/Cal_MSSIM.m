function dResult = Cal_MSSIM(OriData, PreData)
OriData = double(OriData);
PreData = double(PreData);
[h,w,d]=size(OriData);
dResult = 0;
for ii = 1:d
    dsumOri = 0;
    dsumPre = 0;
    dsumOri2 = 0;
    dsumPre2 = 0;
    dsumPlus = 0;
    dmin = 0;
    dmax = 0;
    for jj = 1:h
        for kk = 1:w
            if(OriData(jj,kk,ii) < dmin)
                dmin = OriData(jj,kk,ii);
            end
            if(OriData(jj,kk,ii) > dmax)
                dmax = OriData(jj,kk,ii);
            end
        end
    end

    for jj = 1:h
        for kk = 1:w
            OriData(jj,kk,ii) = (OriData(jj,kk,ii) - dmin) / (dmax - dmin);
            PreData(jj,kk,ii) = (PreData(jj,kk,ii) - dmin) / (dmax - dmin);
            
            dsumOri = dsumOri + OriData(jj,kk,ii);
            dsumOri2 = dsumOri2 + OriData(jj,kk,ii) * OriData(jj,kk,ii);
            dsumPre = dsumPre + PreData(jj,kk,ii);
            dsumPre2 = dsumPre2 + PreData(jj,kk,ii) * PreData(jj,kk,ii);
            dsumPlus = dsumPlus + OriData(jj,kk,ii) * PreData(jj,kk,ii);
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
dResult = dResult / d;
end
