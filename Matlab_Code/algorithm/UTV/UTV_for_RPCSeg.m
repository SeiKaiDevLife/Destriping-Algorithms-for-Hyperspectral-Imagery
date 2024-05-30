for SegX = 0:4
    for SegY = 0:4
        PathSegTifInOri = "D:\RS-RPC\TEST\IN\SegImage\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        PathSegTifInNoise = "D:\RS-RPC\TEST\IN\SegImageNoise\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        PathSegTifInDenoise = "D:\RS-RPC\TEST\IN\SegImageDenoise\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        
        img = double(imread(PathSegTifInNoise));
        imgOri = double(imread(PathSegTifInOri));
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
        iter = 8;
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
        
            [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_60P_for_RPCSeg(imgOri, u);
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
        
        end
        
        string = ['Best Result: ',num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
        Result = Umax;
        Result = double(Result);
        t = Tiff(PathSegTifInDenoise,'w');
        t.setTag(tagstruct);
        t.write(Result);
        t.close
        disp(string);
    end
end





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