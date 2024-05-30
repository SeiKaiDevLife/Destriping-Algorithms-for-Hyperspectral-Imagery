function [D,Dt] = defDDT()
D  = @(U) ForwardDiff(U);
Dt = @(U) BehindDiff(U);
end

function [Dux,Duy,Duz] = ForwardDiff(U)
 [h,w,d] = size(U);
 Dux = [diff(U,1,2), U(:,1,:) - U(:,end,:)];
 Duy = [diff(U,1,1); U(1,:,:) - U(end,:,:)];
 Duz = zeros(h,w,d);
 Duz(:,:,1:end - 1) = diff(U,1,3);
 Duz(:,:,end) = U(:,:,1) - U(:,:,end);
end

function [Duxt,Duyt,Duzt] = BehindDiff(U)
 [h,w,d] = size(U);
 Duxt = [U(:,end,:) - U(:,1,:), -diff(U,1,2)];
 Duyt = [-U(1,:,:) + U(end,:,:); -diff(U,1,1)];
 Duzt = zeros(h,w,d);
 Duzt(:,:,2:end) = -diff(U,1,3);
 Duzt(:,:,1) = -U(:,:,1) + U(:,:,end);
end