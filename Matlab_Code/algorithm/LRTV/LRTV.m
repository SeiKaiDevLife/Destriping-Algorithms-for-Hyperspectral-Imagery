function [ output_image, out_stripe] = LRTV(oriData3_noise, tau,lambda, r, mu, imgOri)
[M,N,p] = size(oriData3_noise);

% tagstruct.ImageLength = M; % 影像的长度
% tagstruct.ImageWidth = N;  % 影像的宽度
% tagstruct.Photometric = 1;
% tagstruct.BitsPerSample = 64;
% tagstruct.SamplesPerPixel = p;
% tagstruct.RowsPerStrip = 16;
% tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
% tagstruct.Software = 'MATLAB'; 
% tagstruct.SampleFormat = 3;
Y = zeros(M*N,p);
for i=1:p 
  bandp  = oriData3_noise(:,:,i);
  Y(:,i) = bandp(:); 
end

[d,p] = size(Y);
%%%%%% 计算D在非零位置的F范数
d_norm = norm(Y, 'fro');
%%%%%% 计算D在非零位置的F范数
% initialize
tol = 1e-6;
tol1 = tol;
tol2 = tol1;
maxIter = 50;
rho = 1.5;
max_mu = 1e6;
L = Y;
X = Y;
S = zeros(d,p);

A1 = zeros(d,p);
A2 = zeros(d,p);

param2.verbose=1;
param2.max_iter=50;
param2.verbose = 0;


% main loop
iter = 0;
tic
while iter<maxIter
    iter = iter + 1;

    %Updata L
    temp = (Y + X - S + (A1 + A2) / mu) / 2;
    thr = 1/(2 * mu);
    L = m_Lemma(temp, thr);

    %Updata X
    temp = L - A2 / mu;
    for i =1:p  
     z = prox_TV(reshape(temp(:,i),[M,N]),2*tau/mu,param2);
     X(:,i) = z(:);  
    end
    
    %updata S
    temp_S = Y - L + A1/mu;
    thr = lambda/mu;
    S = Softthreshold(temp_S, thr);

    A1 = A1 + mu * (Y - L - S);
    A2 = A2 + mu * (X - L);
    mu = min(max_mu,mu*rho);

    %stop criterion
    leq1 = X - L;
    leq2 = Y -L -S ;
    stopC1 = max(max(abs(leq1)));
    stopC2 = norm(leq2, 'fro') / d_norm;
    if stopC1<tol  && stopC2<tol2
       break;
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    img  = reshape(L,[M,N,p]);
    [Temp_MPSNR,Temp_MSSIM,Temp_MSAD] = Cal_Pal_60P(img,imgOri);
    string = ['iter:', num2str(iter), ' Result:',num2str(Temp_MPSNR),' ',num2str(Temp_MSSIM),' ',num2str(Temp_MSAD)];
    disp(string);

%     filepath = ['D:\PaviaC\Test\142_163_L_',num2str(iter),'.tif'];
%     Result = img;
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close
%     filepath = ['D:\PaviaC\Test\142_163_X_',num2str(iter),'.tif'];
%     Result = reshape(X,[M,N,p]);
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close
%     filepath = ['D:\PaviaC\Test\142_163_S_',num2str(iter),'.tif'];
%     Result = reshape(S,[M,N,p]);
%     Result = double(Result);
%     t = Tiff(filepath,'w');
%     t.setTag(tagstruct);
%     t.write(Result);
%     t.close
end
toc
output_image = reshape(L,[M,N,p]);
out_stripe = reshape(S,[M,N,p]);
end

