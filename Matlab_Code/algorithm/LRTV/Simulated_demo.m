%img = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));
% img = double(imread("D:\PaviaC\EO-1\band_142_163_tifNorm.tif"));
img = double(imread("D:\Denoise\WDC\测试图_纯条带\60P.tif"));
imgOri = double(imread("D:\Denoise\WDC\WDCtifnorm.tif"));
[h,w,d] = size(img);

%% LRTV denoising
%
tau = 0.5;
lambda =0.004;
mu = 0.01;
rank = 8;
string = ['tau = ',num2str(tau),' lambda = ',num2str(lambda),' mu = ',num2str(mu)];
disp(string);
[ output_image, out_stripe] = LRTV(img, tau,lambda, rank, mu, imgOri);
tagstruct.ImageLength = h; % 影像的长度
tagstruct.ImageWidth = w;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = d;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

filepath = 'D:\Denoise\WDC\Out\LRTV\Pre\60\L.tif';
Result = output_image;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close

filepath = 'D:\Denoise\WDC\Out\LRTV\Pre\60\S.tif';
Result = out_stripe;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close