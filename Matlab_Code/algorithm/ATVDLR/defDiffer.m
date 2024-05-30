function [Dx, Dy, Dz, Dxt, Dyt, Dzt] = defDiffer()
Dx  = @(U) Diffx(U);
Dy  = @(U) Diffy(U);
Dz  = @(U) Diffz(U);
Dxt  = @(U) Diffxt(U);
Dyt  = @(U) Diffyt(U);
Dzt  = @(U) Diffzt(U);
end

function Dx = Diffx(U)
    Dx = [diff(U,1,2), U(:,1,:) - U(:,end,:)];
end

function Dy = Diffy(U)
    Dy = [diff(U,1,1); U(1,:,:) - U(end,:,:)];
end

function Dz = Diffz(U)
    [h,w,d] = size(U);
    Dz = zeros(h,w,d);
    Dz(:,:,1:end - 1) = diff(U,1,3);
    Dz(:,:,end) = U(:,:,1) - U(:,:,end);
end

function Dxt = Diffxt(U)
    Dxt = [-U(:,end,:) + U(:,1,:), diff(U,1,2)];
end

function Dyt = Diffyt(U)
    Dyt = [U(1,:,:) - U(end,:,:); diff(U,1,1)];
end

function Dzt = Diffzt(U)
    [h,w,d] = size(U);
    Dzt = zeros(h,w,d);
    Dzt(:,:,2:end) = diff(U,1,3);
    Dzt(:,:,1) = +U(:,:,1) - U(:,:,end);
end