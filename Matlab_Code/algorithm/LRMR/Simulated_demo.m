% clear all;
% close all;
% clc;
%clc
tic
% load ['Pavia.mat']
OriData3=double(imread("D:\Denoise\WDC\WDCtifnorm.tif"));

oriData3_noise = double(imread("D:\Denoise\WDC\测试图\80PR.tif"));
%oriData3_noise = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));
% oriData3_noise = double(imread("D:\PaviaC\EO-1\band_142_163_tifNorm.tif"));
[M N p] = size(oriData3_noise);

tagstruct.ImageLength = M; % 影像的长度
tagstruct.ImageWidth = N;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = p;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

%% simulated experiment
% ratio = 0.15*ones(1,224);            % for case 1
% noiselevel = 0.075*ones(1,224);      % for case 1
% %noise simulated
% for i =1:p
%      oriData3_noise(:,:,i)=OriData3(:,:,i)  + noiselevel(i)*randn(M,N);
% %        mixed(vv,:)=addnoise(mixed(vv,:),'mg',ratio);
% end
% % oriData3_noise = OriData3 + 0.075*randn(M,N,p);
% for i =1:p
%      oriData3_noise(:,:,i)=imnoise(oriData3_noise(:,:,i),'salt & pepper',ratio(i));
% %        mixed(vv,:)=addnoise(mixed(vv,:),'mg',ratio);
% end
%                  filepath = 'D:\TestData\TestImage\Out\LRMRIn.tif';
%                 Result = oriData3_noise;
%                 Result = double(Result);
%                 t = Tiff(filepath,'w');
%                 t.setTag(tagstruct);
%                 t.write(Result);
%                 t.close

%% LRMR
r = 3;
blocksize = 32;
% s= 3000;  % for LRM6 implemented with GoDec 
s = 0.01;   % for LRMR implemented with ssGoDec 
stepsize = 4;
[ output_image ] = LRMR_HSI_denoise( oriData3_noise,r,blocksize,s,stepsize );

[Temp_MPSNR,Temp_MSSIM,Temp_MSAD] = Cal_Pal_80PR(OriData3,output_image);

string = ['Try- r = ',num2str(r),' blocksize = ',num2str(blocksize),' s = ', num2str(s),' stepsize = ',num2str(stepsize), ' Result:',num2str(Temp_MPSNR),' ',num2str(Temp_MSSIM),' ',num2str(Temp_MSAD)];
disp(string);
toc
filepath = 'D:\Denoise\WDC\Out\LRMR\PreRand\80\Xg.tif';
Result = output_image;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close
%%
% PSNR_in=10*log10((mean((OriData3(:)).^2))/mean((oriData3_noise(:)-OriData3(:)).^2));
% PSNR_out=10*log10((mean((OriData3(:)).^2))/mean((output_image(:)-OriData3(:)).^2));

% PSNRvector=zeros(1,p);
% for i=1:1:p
%     J=255*OriData3(:,:,i);
%     I=255*output_image(:,:,i);
%     PSNRvector(1,i)=PSNR(J,I,M,N);
% end
% dlmwrite('PSNRvector.txt',PSNRvector,'delimiter','\t','newline','pc'); 
% MPSNR = mean(PSNRvector);
