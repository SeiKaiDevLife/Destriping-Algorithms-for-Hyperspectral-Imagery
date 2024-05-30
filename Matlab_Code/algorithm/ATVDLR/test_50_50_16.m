clear;
clc;
A = [1,2,3;4,5,6;7,8,8];



image = imread("D:\\TestData\\ATVDLRDenoise\\ATVDLR_IN\\191_220_tif.tif");
image = image(1:50,1:50,1:16);

[U1,S1,V1] = tsvd(image);

Vh1 = Conjugate_Transpose(V1);
Vh2 = tran(V1);
US1 = tprod(U1,S1);
USVh1 = tprod(US1,Vh2);