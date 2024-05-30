%img = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));

tic
img = double(imread("D:\Denoise\WDC\测试图_纯条带\40P.tif"));
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

iter = 50;

Y = img;

Umax = zeros(h,w,d);
Smax = zeros(h,w,d);

dx = zeros(h,w,d);
dy = zeros(h,w,d);
dz = zeros(h,w,d);

bx = zeros(h,w,d);
by = zeros(h,w,d);
bz = zeros(h,w,d);

dAlpha = 12;
dBeta = 13;
dGamma = 3;
dLambda1 = 0.2;
dLambda2 = 10;
dLambda3 = 0.2;

string = ['Try: dAlpha = ',num2str(dAlpha),' dBeta = ',num2str(dBeta),' dGamma = ',num2str(dGamma),' dLambda1 = ',num2str(dLambda1), ' dLambda2 = ',num2str(dLambda2),' dLambda3 = ',num2str(dLambda3)];
disp(string);

[Dx, Dy, Dz, Dxt, Dyt, Dzt] = defDiffer();

Dyf = Dy(Y);

% for dAlpha = 1:4:17
%     for dBeta = 1:4:17
%         for dGamma = 1:4:17
%             for dLambda1 = 1:4:17
%                 for dLambda2 = 1:4:17
%                     for dLambda3 = 1:4:17

Iter_MPSNR = 0;
Iter_MSSIM = 0;
Iter_MSAD = 0;

for i = 1:iter
    Temp_dx_bx = dx - bx;
    DxtTemp_dx_bx = Dxt(Temp_dx_bx);

    Temp_dy_dyf_by = dy + Dyf - by;
    DytTemp_dy_dyf_by = Dyt(Temp_dy_dyf_by);

    Temp_dz_bz = dz - bz;
    DztTemp_dz_bz = Dzt(Temp_dz_bz);

    C = Y + dAlpha * DxtTemp_dx_bx + dBeta * DytTemp_dy_dyf_by + dGamma * DztTemp_dz_bz;

    u = FFT(C, dAlpha, dBeta, dGamma);

    Dxu = Dx(u);
    Dzu = Dz(u);
    Dyu_f = Dy(u - Y);

    dx = Softthreshold(Dxu + bx, dLambda1 / dAlpha);
    dy = Softthreshold(Dyu_f + by, dLambda2 / dBeta);
    dz = Softthreshold(Dzu + bz, dLambda3 / dGamma);

    bx = bx + Dxu - dx;
    by = by + Dyu_f - dy;
    bz = bz + Dzu - dz;

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_40P(imgOri, u);
    string = ['iter:', num2str(i), ' Result:',num2str(Temp_MPSNR)];
    disp(string);

    si = num2str(i);
    disp(si);
    filepath = ['D:\Denoise\WDC\Test\ASSTV_U_',si,'.tif'];
    Result = u;
    Result = double(Result);
    t = Tiff(filepath,'w');
    t.setTag(tagstruct);
    t.write(Result);
    t.close
% 
%     si = num2str(i);
%     filepath = ['D:\TestData\TestImage\Out\S_',si,'.tif'];
%     Result = Y - u;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close

    if Temp_MPSNR > Iter_MPSNR + 0.01
        iCount = 0;
    else
        iCount = iCount + 1;
    end
     if Temp_MPSNR > Iter_MPSNR
        Iter_MPSNR = Temp_MPSNR;
        Iter_MSSIM = Temp_MSSIM;
        Iter_MSAD = Temp_MSAD;
        Umax = u;
        Smax = Y - u;
    end 
    
    if iCount >= 3 && i > 10
       break;
    end

end

string = ['dAlpha = ',num2str(dAlpha),' dBeta = ',num2str(dBeta),' dGamma = ',num2str(dGamma),' dLambda1 = ',num2str(dLambda1), ' dLambda2 = ',num2str(dLambda2),' dLambda3 = ',num2str(dLambda3),' BestResult:', num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
disp(string);
toc

% filepath = 'D:\Denoise\WDC\Out\ASSTV\PreRand\80\Ug.tif';
% Result = Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\ASSTV\PreRand\80\Sg.tif';
% Result = Smax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close