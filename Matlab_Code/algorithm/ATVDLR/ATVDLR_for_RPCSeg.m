for SegX = 0:5
    for SegY = 0:5
        PathSegTifInOri = "D:\RS-RPC\TEST\IN\SegImage\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        PathSegTifInNoise = "D:\RS-RPC\TEST\IN\SegImageNoise\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        PathSegTifInDenoise = "D:\RS-RPC\TEST\IN\SegImageDenoiseATVDLR\SegImg_" + num2str(SegX) + "_" + num2str(SegY) + ".tif";
        
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

        Xmax = zeros(h,w,d);
        Bmax = zeros(h,w,d);
        Smax = zeros(h,w,d);
        
        H = img;
        X = img;
        S = zeros(h,w,d);
        B = zeros(h,w,d);
        N = zeros(h,w,d);
        
        H_X = zeros(h,w,d);
        
        DxX = zeros(h,w,d);
        DyB = zeros(h,w,d);
        DzX = zeros(h,w,d);
        
        X12 = zeros(h,w,d);
        X13 = zeros(h,d,w);
        X23 = zeros(w,d,h);
        
        P = zeros(h,w,d);
        Q = zeros(h,w,d);
        T = zeros(h,w,d);
        J = zeros(h,w,d);
        
        DyT = zeros(h,w,d);
        
        Z = zeros(h,w,d);
        Z12f = img;
        Z13f = img;
        Z23f = img;
        Z12uf = zeros(h,w,d);
        Z13uf = zeros(h,d,w);
        Z23uf = zeros(w,d,h);
        
        A = zeros(h,w,d);
        A12f = zeros(h,w,d);
        A13f = zeros(h,w,d);
        A23f = zeros(h,w,d);
        A12uf = zeros(h,w,d);
        A13uf = zeros(h,d,w);
        A23uf = zeros(w,d,h);
        
        Mp = zeros(h,w,d);
        Mq = zeros(h,w,d);
        Mb = zeros(h,w,d);
        Mt = zeros(h,w,d);
        Mh = zeros(h,w,d);
        
        Iter_MPSNR = 0;
        
        iter = 100;
        
        dMu = 0.01;
        dMumax = 1000000;
        dRou = 1.1;
        
        dIter = 45;
        
        dw12 = 1 / 3;
        dw13 = 1 / 3;
        dw23 = 1 / 3;
        dLambda = 1;
        dBeta = 6;
        dTao1 = 0.006;
        dTao2 = 0.02;
        dTao3 = 0.06;
        
        TempdMu = dMu;
        
        [Dx, Dy, Dz, Dxt, Dyt, Dzt] = defDiffer();
        
        for i = 1:dIter    
        
            %update Z12
            thr = dw12 / dMu;
            X12 = unfold_mode_12(X);
            A12uf = unfold_mode_12(A12f);
            C = X12 + (A12uf ./ dMu);
            Z12uf = t_Lemma(C, thr);
            Z12f = fold_mode_12(Z12uf);
        
            %update Z13
            thr = dw13 / dMu;
            X13 = unfold_mode_13(X);
            A13uf = unfold_mode_13(A13f);
            C = X13 + (A13uf ./ dMu);
            Z13uf = t_Lemma(C, thr);
            Z13f = fold_mode_13(Z13uf);
        
            %update Z23
            thr = dw23 / dMu;
            X23 = unfold_mode_23(X);
            A23uf = unfold_mode_23(A23f);
            C = X23 + (A23uf ./ dMu);
            Z23uf = t_Lemma(C, thr);
            Z23f = fold_mode_23(Z23uf);
        
            %update S
            thr = dLambda / dMu;
            C = H - X - B + (Mh ./ dMu);
            S = Softthreshold(C, thr);
        
            %update X
            DxtP = Dxt(P);
            DxtMp = Dxt(Mp);
            DztQ = Dzt(Q);
            DztMq = Dzt(Mq);
            C = H - S - B + (Mh ./ dMu) + DxtP + (DxtMp ./ dMu) + Z12f - (A12f ./ dMu) + Z13f - (A13f ./ dMu) + Z23f - (A23f ./ dMu) + DztQ - (DztMq ./ dMu);
            X = t_XSub(C);
        
            %updata P
            DxX = Dx(X);
            thr = dTao1 / dMu;
            C = DxX - (Mp ./ dMu);
            P = Softthreshold(C, thr);
        
            %updata Q
            DzX = Dz(X);
            thr = dTao3 / dMu;
            C = DzX - (Mq ./ dMu);
            Q = Softthreshold(C, thr);
        
            %update B
            thr = dBeta / (2 * dMu);
            C = (H ./ 2) - (X ./ 2) - (S ./ 2) + (T ./ 2) + (Mh ./ (2 * dMu)) - (Mb ./ (2 * dMu));
            for j = 1:d
                B(:,:,j) = m_Lemma(C(:,:,j), thr);
            end
        
            %update T
            DytJ = Dyt(J);
            DytMt = Dyt(Mt);
            C = B + (Mb ./ dMu) + DytJ + (DytMt ./ dMu);
            T = t_TSub(C);
        
            %update J
            DyT = Dy(T);
            thr = dTao2 / dMu;
            C = DyT - (Mt ./ dMu);
            J = Softthreshold(C, thr);
        
            %update Lagrangian
            A12f = A12f + dMu * (X - Z12f);
            A13f = A13f + dMu * (X - Z13f);
            A23f = A23f + dMu * (X - Z23f);
            Mp = Mp + dMu * (P - DxX);
            Mq = Mq + dMu * (Q - DzX);
            Mb = Mb + dMu * (B - T);
            Mt = Mt + dMu * (J - DyT);
            Mh = Mh + dMu * (H - X - S - B);
        
            %update N
            N = H - S - X - B;
        
            dMu = dMu * dRou;
            if dMu > dMumax
                dMu = dMumax;
            end
        
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
            [Temp_MPSNR, Temp_MSSIM, Temp_MSAD] = Cal_Pal_60P_for_RPCSeg(imgOri, X);
                    
            string = ['iter:', num2str(i), ' Result:',num2str(Temp_MPSNR),' ',num2str(Temp_MSSIM),' ',num2str(Temp_MSAD)];
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
                Xmax = X;
                Bmax = B;
                Smax = S;
                Gmax = img - X - B - S;
            end
            
            if iCount >= 3
                if i >= 30
                    %break;
                end
            end                        
        end
        string = ['Try —— Miu:',num2str(TempdMu), ' Lambda:',num2str(dLambda),' Beta:',num2str(dBeta),' tau1:',num2str(dTao1),' tau2:',num2str(dTao2),' tau3:',num2str(dTao3), ' BestResult:', num2str(Iter_MPSNR),' ',num2str(Iter_MSSIM),' ',num2str(Iter_MSAD)];
        disp(string);

        Result = Xmax;
        Result = double(Result);
        t = Tiff(PathSegTifInDenoise,'w');
        t.setTag(tagstruct);
        t.write(Result);
        t.close

    end
end




% filepath = 'D:\Denoise\WDC\Out\ATVDLR\Pre\20\X.tif';
% Result = Xmax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\ATVDLR\Pre\20\B.tif';
% Result = Bmax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\ATVDLR\Pre\20\S.tif';
% Result = Smax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
% 
% filepath = 'D:\Denoise\WDC\Out\ATVDLR\Pre\20\G.tif';
% Result = Gmax;
% Result = double(Result);
% t = Tiff(filepath,'w');
% t.setTag(tagstruct);
% t.write(Result);
% t.close
