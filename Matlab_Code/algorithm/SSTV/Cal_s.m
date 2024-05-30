function s = Cal_s(y, x, Lambda)
function X=SoftTh(B,lambda)
      
       X=sign(B).*max(0,abs(B)-(lambda/2));
       
end
s=SoftTh(y-x,Lambda);
end