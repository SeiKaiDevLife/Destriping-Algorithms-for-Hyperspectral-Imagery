clc
tic
% [h,w,d] = size(img);
% 
% Iter_MPSNR = 0;
% Iter_MSSIM = 0;
% Iter_MSAD = 0;
% Iter_i = 0;
% maxU = zeros(h,w,d);
% 
% for i = 0:19
%     path = ['D:\PaviaC\Test\U_',num2str(i),'.tif'];
%     img = double(imread(path));
%     [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_40P(imgOri, img);
%     if Temp_MPSNR > Iter_MPSNR
%         Iter_MPSNR = Temp_MPSNR;
%         Iter_MSSIM = Temp_MSSIM;
%         Iter_MSAD = Temp_MSAD;
%         Umax = u;
%         Iter_i = i;
%     end
% end
% 
% string = ['Best Result: ',num2str(Iter_i),' ',num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
% disp(string);
% 
% 
% filepath = 'D:\PaviaC\Out\UTV\40\Pre\U.tif';
% Result = Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\PaviaC\Out\UTV\40\Pre\S.tif';
% Result = imgOri - Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
img = double(imread("D:\Denoise\WDC\测试图_纯条带\40P.tif"));
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

f = img;
vk = img;
uk1 = img;
uk = zeros(h,w,d);
u = zeros(h,w,d);
iter = 20;
lambda = 100;
Iter_MPSNR = 0;

for iii = 1:iter
    for ibandnum = 1:d
        fm = f(:,:,ibandnum);
        uk1m = uk1(:,:,ibandnum);
        ukm = uk(:,:,ibandnum);
        vkm = vk(:,:,ibandnum);
        um = u(:,:,ibandnum);

        for jjj = 1:100
            ukm = uk1m;
            uk1m = Guass(ukm,vkm,lambda);
        end

        um = um + uk1m;
        vkm = fm - um;

        uk1m = vkm;

        uk(:,:,ibandnum) = ukm;
        uk1(:,:,ibandnum) = uk1m;
        vk(:,:,ibandnum) = vkm;
        u(:,:,ibandnum) = um;
    end
    lambda = lambda / 2;

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_40P(imgOri, u);
    string = ['iter:', num2str(iii), ' Result:',num2str(Temp_MPSNR)];
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
        Umax = u;
    end
    
    if iCount >= 3
       %break;
    end  

    si = num2str(iii);
    %disp(si);
    %filepath = ['D:\PaviaC\Test\186_223_U_',si,'.tif'];
    filepath = ['D:\Denoise\WDC\Test\U_',si,'.tif'];
    Result = u;
    Result = double(Result);
    t = Tiff(filepath,'w');
    t.setTag(tagstruct);
    t.write(Result);
    t.close

%     si = num2str(iii);
%     filepath = ['D:\Denoise\WDC\Test\Uk1m_',si,'.tif'];
%     Result = uk1;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close
% 
%      si = num2str(iii);
%     filepath = ['D:\Denoise\WDC\Test\Vkm_',si,'.tif'];
%     Result = vk;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close

end

string = ['Best Result: ',num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
disp(string);
toc

% filepath = 'D:\Denoise\WDC\Out\UTV\PreRand\80\Ug.tif';
% Result = Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\UTV\PreRand\850\Sg.tif';
% Result = imgOri - Umax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close