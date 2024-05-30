imgSSTV = double(imread("D:\TestData\TestImage\Out\X_48.tif"));
imgDLR = double(imread("D:\TestData\TestImage\Out\L_5.tif"));
imgOri = double(imread("D:\TestData\TestImage\pavia_HSI_23_102_128.tif"));
h = 128;
w = 128;

zzDLR = Cal_MPSNR(imgOri, imgDLR + imgOri);
zzSSTV = Cal_MPSNR(imgOri, imgSSTV + imgOri);

imgDLR = imgDLR(:,:,55);
imgSSTV = imgSSTV(:,:,55);
imgOri = imgOri(:,:,55);

zzzDLR = Cal_MPSNR(imgOri, imgDLR + imgOri);
zzzSSTV = Cal_MPSNR(imgOri, imgSSTV + imgOri);

tempDLR = zeros(h,w);
tempSSTV = zeros(h,w);

for i = 1:h
for j = 1:w
tempDLR(i,j) = imgDLR(i,j) * imgDLR(i,j);
tempSSTV(i,j) = imgSSTV(i,j) * imgSSTV(i,j);
end
end

zDLR = 0;
zSSTV = 0;
for i = 1:h
for j = 1:w
zDLR = zDLR + tempDLR(i,j);
zSSTV = zSSTV + tempSSTV(i,j);

end
end




