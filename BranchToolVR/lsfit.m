% compute least square fit
% cd D:\Sayak\BranchToolVR\BranchToolVR
clear;
load('D:\Sayak\BranchToolVR\BranchToolVR\input.dat')
pces = 1;
%if pces ==1,
	%input = [input(1:144,:);  input(144,:); input(144,:)];
%else
	%input = [input(1:144,:);  input(144,:)];
%end
%input(3,:) = 100*[1,1,1]; % fake outlier
[n,dim] = size(input);

approx = []; err = []; ttt = []; lo = 1;
step = floor(n/pces);
for pc = 1:pces
    hi = lo + step;  
	if (pces==1 && pc==pces) hi = n-1; 
end;
	 
	xy = input(lo:hi,:);
    [nloc,dim] = size(xy);
	% get parameters
	df = xy(2:nloc,:)-xy(1:nloc-1,:);
	len = sqrt(sum(df.^2,2));  % length between points
	avglen = sum(len/nloc);
	outlieridx = len(len>2*avglen)
	tt = tril(ones(nloc-1))*len;  % increasing length along curve
	tt = [0; tt/tt(nloc-1)];  % normalized to 0..1

	% basis functions
	B0 = (1-tt).^3;
	B1 = 3*(1-tt).^2 .* tt;
	B2 = 3*(1-tt).*tt.^2;
	B3 = tt.^3;

	% least squares fit min ||sum c_i Bi - xyi||^2
	A = [B1'*B1  B1'*B2; B2'*B1 B2'*B2];
	b1 = [B0'*B1  B0'*B2; B3'*B1 B3'*B2]*[xy(1,:) ; xy(nloc,:)];
	b2 = [B1'*xy; B2'*xy];
	cf = A\(b2-b1);

	%compute error 
	bb{pc} = [xy(1,:) ; cf(1,:); cf(2,:); xy(nloc,:)];
	bez{pc} = [B0 B1 B2 B3]*bb{pc};  % points on least-squares fit
	df = bez{pc}-xy;
	err{pc} = sqrt(sum(df.^2,2));
	approx = [approx; bez{pc}];
    lo = hi;
	% max(err)
end;

figure(2)  
clf;
plot3(xy(:,1),xy(:,2 ),xy(:,3),'b'); hold on;
plot3(approx(:,1),approx(:,2 ), approx(:,3),'r'); hold on;
for pc=1:pces,
    %plot3(bb{pc}(:,1),bb{pc}(:,2 ), bb{pc}(:,3)); hold on;
end;
figure(3)
clf;
for pc=1:pces,
    plot(tt, err{pc}); hold on;
end;
