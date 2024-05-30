tic

img = double(imread("D:\Denoise\WDC\测试图\20P.tif"));
%img = double(imread("D:\PaviaC\EO-1\band_186_223_tifNorm.tif"));
% img = double(imread("D:\PaviaC\EO-1\band_142_163_tifNorm.tif"));
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

Iter_MPSNR = 0;
Miumax = 1000000;

for TempMiu = 0.01:0.01
    for Lambda1 = 0.04:0.04
        for Lambda2 = 0.35:0.35
            string = ['Try —— Miu:',num2str(TempMiu), ' Lambda1:',num2str(Lambda1),' Lambda2:',num2str(Lambda2)];
            disp(string);

            Miu = TempMiu;
            Iter_MPSNR = 0;
            Iter_MSSIM = 0;
            Iter_MSAD = 0;
            iCount = 0;
            Rou = 1.1;
            iter = 50;

            Y = reshape(img,h*w,d);
            Lmax = zeros(h,w,d);
            Bmax = zeros(h,w,d);
            Gmax = zeros(h,w,d);
            Smax = zeros(h,w,d);
            L = zeros(h*w,d);
            S = zeros(h*w,d);
            B = zeros(h*w,d);
            A = zeros(h*w,d);
    
            for i = 1:iter
                thr = 1 / Miu;
                W = Y - S - B + A / Miu;
                L = m_Lemma(W,thr);
            
                thr = Lambda1 / Miu;
                W = Y - L - B + A / Miu;
                S = Softthreshold(W,thr);
            
                Yn = reshape(Y,h,w,d);
                Ln = reshape(L,h,w,d);
                Sn = reshape(S,h,w,d);
                Bn = reshape(B,h,w,d);
                An = reshape(A,h,w,d);
                Wn = zeros(h,w);
            
                thr = Lambda2 / Miu;
                for j = 1:d
                    Wn = Yn(:,:,j) - Ln(:,:,j) - Sn(:,:,j) + An(:,:,j) / Miu;
                    Bn(:,:,j) = m_Lemma(Wn, thr);
                end
            
                B = reshape(Bn, h*w,d);
            
                A = A + Miu * (Y - L - S - B);
            
                Miu = min(Miu * Rou, Miumax);
            
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_20P(imgOri, Ln);
           
            
                string = ['iter:', num2str(i), ' Result:',num2str(Temp_MPSNR)];
                disp(string);

                if Temp_MPSNR > Iter_MPSNR
                    iCount = 0;
                else
                    iCount = iCount + 1;
                end
    
                if Temp_MPSNR > Iter_MPSNR
                    Iter_MPSNR = Temp_MPSNR;
                    Iter_MSSIM = Temp_MSSIM;
                    Iter_MSAD = Temp_MSAD;
                    Lmax = Ln;
                    Bmax = Bn;
                    Smax = Sn;
                    Gmax = img - Ln - Bn - Sn;
                end
                
                if iCount >= 3 && i >=10
                   break;
                end
    
%                 si = num2str(i);
%                 filepath = ['D:\Denoise\WDC\Test\X_',si,'.tif'];
%                 Result = reshape(L,h,w,d);
%                 Result = double(Result);
%                 t = Tiff(filepath,'w');
%                 t.setTag(tagstruct);
%                 t.write(Result);
%                 t.close
%     
%                 si = num2str(i);
%                 filepath = ['D:\Denoise\WDC\Test\S_',si,'.tif'];
%                 Result = reshape(S,h,w,d);
%                 Result = double(Result);
%                 t = Tiff(filepath,'w');
%                 t.setTag(tagstruct);
%                 t.write(Result);
%                 t.close
%     
%                 si = num2str(i);
%                 filepath = ['D:\Denoise\WDC\Test\G_',si,'.tif'];
%                 Result = reshape(Y - L - B,h,w,d);
%                 Result = double(Result);
%                 t = Tiff(filepath,'w');
%                 t.setTag(tagstruct);
%                 t.write(Result);
%                 t.close
%     
%                 si = num2str(i);
%                 filepath = ['D:\Denoise\WDC\Test\B_',si,'.tif'];
%                 Result = reshape(B,h,w,d);
%                 Result = double(Result);
%                 t = Tiff(filepath,'w');
%                 t.setTag(tagstruct);
%                 t.write(Result);
%                 t.close
            end
            toc
            string = ['Try —— Miu:',num2str(TempMiu), ' Lambda1:',num2str(Lambda1),' Lambda2:',num2str(Lambda2), ' BestResult:', num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
            disp(string);


            filepath = 'D:\Denoise\WDC\Out\DLR\Pre\20\Lg.tif';
            Result = Lmax;
            Result = double(Result);
            t = Tiff(filepath,'w');
            t.setTag(tagstruct);
            t.write(Result);
            t.close

            filepath = 'D:\Denoise\WDC\Out\DLR\Pre\20\Bg.tif';
            Result = Bmax;
            Result = double(Result);
            t = Tiff(filepath,'w');
            t.setTag(tagstruct);
            t.write(Result);
            t.close

            filepath = 'D:\Denoise\WDC\Out\DLR\Pre\20\Sg.tif';
            Result = Smax;
            Result = double(Result);
            t = Tiff(filepath,'w');
            t.setTag(tagstruct);
            t.write(Result);
            t.close

            filepath = 'D:\Denoise\WDC\Out\DLR\Pre\20\Gg.tif';
            Result = Gmax;
            Result = double(Result);
            t = Tiff(filepath,'w');
            t.setTag(tagstruct);
            t.write(Result);
            t.close
        end
    end
end
