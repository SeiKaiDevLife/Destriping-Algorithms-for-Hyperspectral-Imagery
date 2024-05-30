function C = face_mult(A,B)
% Face multiplication of two sensors
dimsA=size(A);
dimsB=size(B);
if (dimsA(2)~=dimsB(1)) || (dimsA(3)~=dimsB(3))
    error('tensors not comensuarate')
end
C=zeros(dimsA(1),dimsB(2),dimsA(3));
for k=1:dimsA(3)
    C(:,:,k)=A(:,:,k)*B(:,:,k);
end