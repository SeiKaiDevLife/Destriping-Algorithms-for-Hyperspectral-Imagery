function [D,Dt] = defDDT()
D  = @(U) ForwardDiff(U);
Dt = @(U) BehindDiff(U);
end

function [Dux,Duy] = ForwardDiff(U)
 Dux = [diff(U,1,2), U(:,1,:) - U(:,end,:)];
 Duy = [diff(U,1,1); U(1,:,:) - U(end,:,:)];
end

function [Duxt,Duyt] = BehindDiff(U)
 Duxt = [U(:,end,:) - U(:,1,:), -diff(U,1,2)];
 Duyt = [-U(1,:,:) + U(end,:,:); -diff(U,1,1)];
end