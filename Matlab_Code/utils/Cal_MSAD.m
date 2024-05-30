function dResult = Cal_MSAD(OriData, PreData)
OriData = double(OriData);
PreData = double(PreData);
[h,w,d]=size(OriData);
dResult = 0;

for ii = 1:d
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
        end
    end
end

for ii = 1:h
    for jj = 1:w
        xy = 0;
        xx = 0;
        yy = 0;
        for kk = 1:d
            xy = xy + OriData(ii,jj,kk) * PreData(ii,jj,kk);
            xx = xx + OriData(ii,jj,kk) * OriData(ii,jj,kk);
            yy = yy + PreData(ii,jj,kk) * PreData(ii,jj,kk);
        end

        xx = sqrt(xx);
        yy = sqrt(yy);

        dResult = dResult + acos(xy / (xx * yy)) / pi * 180;
    end
end

dResult = dResult / (h * w);
end