% img = imread("D:\TestData\TestImage\Stripe_WP60_BP60_M0_R20_Pre_Band5.tif");
% [h,w] = size(img);
% 
% tagstruct.ImageLength = h; % 影像的长度
% tagstruct.ImageWidth = w;  % 影像的宽度
% tagstruct.Photometric = 1;
% tagstruct.BitsPerSample = 64;
% tagstruct.SamplesPerPixel = 1;
% tagstruct.RowsPerStrip = 16;
% tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
% tagstruct.Software = 'MATLAB'; 
% tagstruct.SampleFormat = 3;
% 
% mu = 0.2;                                                                                               
% lambda = 0.1;
% iter = 30;
% 
% Y = img;
% Uk = img;
% Uk1 = zeros(h,w);
% Uk_x = zeros(h,w);
% Uk_y = zeros(h,w);
% dx = zeros(h,w);
% dy = zeros(h,w);
% bx = zeros(h,w);
% by = zeros(h,w);
% 
% [D,Dt]      = defDDT();
% 
% for i = 1:iter
%     Uk1 = FFT(mu, lambda,Y,dx,bx,dy,by);
% 
%     [Uk_x, Uk_y] = D(Uk1);
% 
%     thr = 1 / lambda;
%     dx = Softthreshold(Uk_x + bx, thr);
%     dy = Softthreshold(Uk_y + by, thr);
% 
%     bx = bx + (Uk_x - dx);
%     by = by + (Uk_y - dy);
% 
%     si = num2str(i);
%     filepath = ['D:\TestData\TestImage\Out\Stripe_WP60_BP60_M0_R20_Pre_Band5_',si,'.tif'];
%     Result = Uk1;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close
% 
%     Uk = Uk1;
% 
% end

img = double(imread("D:\Denoise\WDC\测试图\80PR.tif"));
%img = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));
%img = double(imread("D:\PaviaC\EO-1\band_142_163_tifNorm.tif"));
imgOri = double(imread("D:\Denoise\WDC\WDCtifnorm.tif"));
[h,w,d] = size(img);

tagstruct.ImageLength = h; % 影像的长度
tagstruct.ImageWidth = w;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = d;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

mu = 0.1;                                                                                               
lambda = 0.1;
iter = 30;

tic

Y = img;
Uk = zeros(h,w,d);
Uk1 = zeros(h,w,d);
dx = zeros(h,w,d);
dy = zeros(h,w,d);
bx = zeros(h,w,d);
by = zeros(h,w,d);
Iter_MPSNR = 0;
[D,Dt]      = defDDT();

string = ['Try —— Miu:',num2str(mu), ' Lambda:',num2str(lambda)];
disp(string);
for i = 1:iter
    for ibandnum = 1:d

        Ym = Y(:,:,ibandnum);
        Ukm = Uk(:,:,ibandnum);
        Uk1m = Uk1(:,:,ibandnum);
        dxm = dx(:,:,ibandnum);
        dym = dy(:,:,ibandnum);
        bxm = bx(:,:,ibandnum);
        bym = by(:,:,ibandnum);
        
        Uk1m = FFT(mu, lambda,Ym,dxm,bxm,dym,bym);
    
        [Uk_x, Uk_y] = D(Uk1m);
    
        thr = 1 / lambda;
        dxm = Softthreshold(Uk_x + bxm, thr);
        dym = Softthreshold(Uk_y + bym, thr);
    
        bxm = bxm + (Uk_x - dxm);
        bym = bym + (Uk_y - dym);

        dx(:,:,ibandnum) = dxm;
        dy(:,:,ibandnum) = dym;
        bx(:,:,ibandnum) = bxm;
        by(:,:,ibandnum) = bym;
        Uk1(:,:,ibandnum) = Uk1m;
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_80PR(imgOri, Uk1);
    string = ['iter:', num2str(i), ' Result:',num2str(Temp_MPSNR)];
    disp(string);

    if Temp_MPSNR > Iter_MPSNR + 0.03
        iCount = 0;
    else
        iCount = iCount + 1;
    end

    if Temp_MPSNR > Iter_MPSNR
        Iter_MPSNR = Temp_MPSNR;
        Iter_MSSIM = Temp_MSSIM;
        Iter_MSAD = Temp_MSAD;
        Umax = Uk1;
    end
    
    if iCount >= 3
       break;
    end

%     si = num2str(i);
%     filepath = ['D:\PaviaC\Test\U_',si,'.tif'];
%     Result = Uk1;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close

end

toc

string = ['Best Result: ',num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
disp(string);


filepath = 'D:\Denoise\WDC\Out\TV\PreRand\80\Ug.tif';
Result = Umax;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close

filepath = 'D:\Denoise\WDC\Out\TV\PreRand\80\Sg.tif';
Result = imgOri - Umax;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close