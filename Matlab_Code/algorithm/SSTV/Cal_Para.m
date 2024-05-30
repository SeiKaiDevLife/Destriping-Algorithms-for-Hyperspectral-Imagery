function [p, q, s, b1, b2] = Cal_Para(x, y, D1, D2, b1, b2, Mu, Nu, Lambda, p, q)

function X=SoftTh(B,lambda)
      
       X=sign(B).*max(0,abs(B)-(lambda/2));
       
end

p=SoftTh(D1*x+b1,Mu/Nu);
q=SoftTh(D2*x+b2,Mu/Nu);
s=SoftTh(y-x,Lambda);

b1=b1+D1*x-p;
b2=b2+D2*x-q;

end