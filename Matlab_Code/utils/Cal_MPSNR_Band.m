function dResult = Cal_MPSNR_Band(OriData, PreData,iBand)
OriData = double(OriData);
PreData = double(PreData) - OriData;
PreData = PreData .* PreData;
[h,w,~]=size(OriData);
dResult = 0;
dsum = sum(sum(PreData(:,:,iBand)));

if dsum == 0
    dsum = 0.001;
end
dmse = dsum / (h * w);
dResult = dResult + 10 * log10((power(2,1) - 1) * (power(2, 1) - 1) / dmse);
end