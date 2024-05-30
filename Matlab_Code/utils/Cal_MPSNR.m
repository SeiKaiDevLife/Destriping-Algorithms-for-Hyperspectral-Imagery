function dResult = Cal_MPSNR(OriData, PreData)
OriData = double(OriData);
PreData = double(PreData);
[h,w,d]=size(OriData);
dResult = 0;
for ii = 1:d
    dsum = 0;
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
            dsum = dsum + (OriData(jj,kk,ii) - PreData(jj,kk,ii)) * (OriData(jj,kk,ii) - PreData(jj,kk,ii));
        end
    end
    dmse = (dsum) / (h * w);
    dResult = dResult + 10 * log10((power(2,1) - 1) * (power(2, 1) - 1) / dmse);
end
dResult = dResult / d;
end

