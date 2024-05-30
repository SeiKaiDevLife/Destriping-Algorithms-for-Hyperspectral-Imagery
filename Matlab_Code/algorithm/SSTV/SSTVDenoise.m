
tic
img = double(imread("D:\Denoise\WDC\测试图_纯条带\40P.tif"));
%img = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));
% img = double(imread("D:\PaviaC\EO-1\band_142_163_tif.tif"));
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

Lambda = 0.01;
Mu = 0.02;
Nu = 0.2;
maxiter = 100;

Umax = zeros(h,w,d);
Smax = zeros(h,w,d);

[m, n, dim] = size(img);
mn = m * n;
b1=zeros(mn*dim,1); 
b2=zeros(mn*dim,1); 
p=zeros(mn*dim,1);
q=zeros(mn*dim,1);
s=zeros(mn*dim,1); 

[D1,D2] = Cal_Oper(m, n, dim);

y=reshape(img,mn*dim,1);x=y;

Iter_MPSNR = 0;
Iter_MSSIM = 0;
Iter_MSAD = 0;

string = ['Lambda:',num2str(Lambda),' Mu:',num2str(Mu),' Nu:',num2str(Nu)];
disp(string);

for i=1:maxiter
    
    Output = SSTV(x, y, Nu, s, p, q, b1, b2, D1, D2, m, n, dim);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_40P(imgOri, Output);
    string = ['iter:', num2str(i), ' Result:',num2str(Temp_MPSNR)];
    disp(string);

    si = num2str(i);
    filepath = ['D:\Denoise\WDC\Test\SSTV_U_',si,'.tif'];
    Result = Output;
    Result = double(Result);
    t = Tiff(filepath,'w');
    t.setTag(tagstruct);
    t.write(Result);
    t.close

    if Temp_MPSNR > Iter_MPSNR + 0.03
        iCount = 0;
    else
        iCount = iCount + 1;
    end
     if Temp_MPSNR > Iter_MPSNR
        Iter_MPSNR = Temp_MPSNR;
        Iter_MSSIM = Temp_MSSIM;
        Iter_MSAD = Temp_MSAD;
        Umax = Output;
        Smax = img - Output;
    end
    
    if iCount >= 3
       break;
    end

    x = reshape(Output,mn*dim,1);
    [p, q, s, b1, b2] = Cal_Para(x, y, D1, D2, b1, b2, Mu, Nu, Lambda, p, q);

   
end
toc
string = [' BestResult:', num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
disp(string);
% 
% filepath = 'D:\Denoise\WDC\Out\SSTV\PreRand\80\Ug.tif';
% Result = Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\SSTV\PreRand\80\Sg.tif';
% Result = Smax;
% Result = double(Result); 
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
