function Result = DLR_Cal(imgOri, Miu, Lambda1, Lambda2, iter)

[h,w,d] = size(imgOri);

Y = reshape(imgOri,h*w,d);
L = zeros(h*w,d);
S = zeros(h*w,d);
B = zeros(h*w,d);
A = zeros(h*w,d);

for i = 1:iter
    thr = 1 / MiuTemp;
    W = Y - S - B + A / MiuTemp;
    L = m_Lemma(W,thr);

    thr = Lambda1 / MiuTemp;
    W = Y - L - B + A / MiuTemp;
    S = Softthreshold(W,thr);

    Yn = reshape(Y,h,w,d);
    Ln = reshape(L,h,w,d);
    Sn = reshape(S,h,w,d);
    Bn = reshape(B,h,w,d);
    An = reshape(A,h,w,d);
    Wn = zeros(h,w);

    thr = Lambda2 / MiuTemp;
    for j = 1:d
        Wn = Yn(:,:,j) - Ln(:,:,j) - Sn(:,:,j) + An(:,:,j) / Miu;
        Bn(:,:,j) = m_Lemma(Wn, thr);
    end

    B = reshape(Bn, h*w,d);

    A = A + MiuTemp * (Y - L - S - B);

    MiuTemp = min(MiuTemp * Rou, Miumax);


end

