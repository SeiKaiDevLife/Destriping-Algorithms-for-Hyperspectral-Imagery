function X
fig = figure('Units','normalized','Position',[.1,.1,.5,.8],'Color',[0,9,33]/255);
ax  = axes('Parent',fig,'Position',[-1/4,-1/3,1+1/2,1+2/3],'UserData',[18,11,0,13,3,0,17,4,17]+97,...
      'XLim',[-7,7],'YLim',[-7,7],'ZLim',[-6,18],'Color',[0,9,33]/255); 
hold on; view(-40,15); disp(char(ax.UserData)); % rng(5)
ax.DataAspectRatio=[1,1,1.2];

XMT = @(h,r,a,z) deal((h-z)./h.*r.*cos(a.*z), (h-z)./h.*r.*sin(a.*z), z);
OFS = @(x,y,z,p) deal(x+rand(size(x)).*p-p/2, y+rand(size(y)).*p-p/2, z+rand(size(z)).*p-p/2);
h = 10.05;
r = h/(1+sqrt(5));
[X,Y,Z] = XMT(h,r,100*pi,rand(3000,1).*h);
[X,Y,Z] = OFS(X,Y,Z,.1);
scatter3(X,Y,Z,12,[213,194,100]./255,'filled','MarkerFaceAlpha',.9,'MarkerEdgeAlpha',.9)

[X,Y,Z] = XMT(h,r,100*pi,rand(500,1).*h);
[X,Y,Z] = OFS(X,Y,Z,1.5);
scatter3(X,Y,Z,8,[255,255,255]./255,'filled','MarkerFaceAlpha',.2,'MarkerEdgeAlpha',.2)

[X,Y,Z] = XMT(h,r,20*pi,linspace(0,1,1000)*h);
scatter3(X,Y,Z,90,[213,194,100]./255,'filled','MarkerFaceAlpha',.2,'MarkerEdgeAlpha',.2)
scatter3(X,Y,Z,12,[255,255,255]./255,'filled','MarkerFaceAlpha',.9,'MarkerEdgeAlpha',.9)
plot3(0,0,10.5,'p', 'MarkerSize',25,'MarkerFaceColor',[255,223,153]/255,'MarkerEdgeColor','none','LineWidth', 1);
starLightHdl = scatter3(0,0,10.5,8000,'o','MarkerFaceColor','w','MarkerEdgeColor','w','MarkerEdgeAlpha',0,'MarkerFaceAlpha', 0.1);
a = 100;
b = (.5:a)'/a;
c = (-cos(b*2*pi)+1).^.2;
f = b-.5;
r = f'.^2+f.^2;
h = abs(ifftn(exp(5i*rand(a))./r.^1.05)).*(c*c')./2-2;
[X,Y] = meshgrid(linspace(-10,10,a)); [X,Y,h] = OFS(X,Y,h,.1);
scatter3(X(:),Y(:),h(:),8,[91,131,253]./255,'filled','MarkerFaceAlpha',.2,'MarkerEdgeAlpha',.2)
h = abs(ifftn(exp(5i*rand(a))./r.^1.05)).*(c*c')./1.5-2.5;
[X,Y] = meshgrid(linspace(-10,10,a)); [X,Y,h] = OFS(X,Y,h,.2);
scatter3(X(:),Y(:),h(:),8,[91,131,253]./255,'filled','MarkerFaceAlpha',.1,'MarkerEdgeAlpha',.1)
snowXYZ1=rand(90,3);
snowXYZ1(:,1:2)=snowXYZ1(:,1:2).*16-8;
snowXYZ1(:,3)=snowXYZ1(:,3).*20;
snowXYZ2=rand(120,3);
snowXYZ2(:,1:2)=snowXYZ2(:,1:2).*16-8;
snowXYZ2(:,3)=snowXYZ2(:,3).*20;
snowHdl1=plot3(snowXYZ1(:,1),snowXYZ1(:,2),snowXYZ1(:,3),'*','Color',[.8,.8,.8]);
snowHdl2=plot3(snowXYZ2(:,1),snowXYZ2(:,2),snowXYZ2(:,3),'.','Color',[.6,.6,.6]);
for i=1:1e8
    starLightHdl.SizeData=8000+sin(i/5).*1600;
    snowXYZ1(:,3)=snowXYZ1(:,3)-.05;snowXYZ2(:,3)=snowXYZ2(:,3)-.005;
    snowXYZ1(snowXYZ1(:,3)<0,3)=snowXYZ1(snowXYZ1(:,3)<0,3)+20;
    snowXYZ2(snowXYZ2(:,3)<0,3)=snowXYZ2(snowXYZ2(:,3)<0,3)+20;
    snowHdl1.XData=snowXYZ1(:,1);snowHdl1.YData=snowXYZ1(:,2);snowHdl1.ZData=snowXYZ1(:,3);
    snowHdl2.XData=snowXYZ2(:,1);snowHdl2.YData=snowXYZ2(:,2);snowHdl2.ZData=snowXYZ2(:,3);
    view([i,15]);
    drawnow;pause(.05)
end
end 