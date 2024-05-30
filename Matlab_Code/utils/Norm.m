img = double(imread("D:\RS-RPC\TEST\IN\PaviaCOri.tif"));
[h,w,d] =size(img); 


for i = 1:d
    max = img(1,1,i);
    min = img(1,1,i);
    for j = 1:h
        for k = 1:w
            if img(j,k,i) < min
                min = img(j,k,i);
            end
            if img(j,k,i) > max
                max = img(j,k,i);
            end
        end
    end

    for j = 1:h
        for k = 1:w
            img(j,k,i) = (img(j,k,i) - min) / (max - min);
        end
    end
end

tagstruct.ImageLength = h; % 影像的长度
tagstruct.ImageWidth = w;  % 影像的宽度
tagstruct.Photometric = 1;
tagstruct.BitsPerSample = 64;
tagstruct.SamplesPerPixel = d;
tagstruct.RowsPerStrip = 16;
tagstruct.PlanarConfiguration = Tiff.PlanarConfiguration.Chunky;
tagstruct.Software = 'MATLAB'; 
tagstruct.SampleFormat = 3;

filepath = "D:\RS-RPC\TEST\IN\PaviaCOriNorm.tif";
Result = img;
Result = double(Result);
t = Tiff(filepath,'w');
t.setTag(tagstruct);
t.write(Result);
t.close