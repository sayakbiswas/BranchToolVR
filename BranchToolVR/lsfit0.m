% compute least square fit
% cd D:\Sayak\BranchToolVR\BranchToolVR
load('D:\Sayak\BranchToolVR\BranchToolVR\input.dat')
xy = input;

[n,dim] = size(xy);

% get parameters
df = xy(2:n,:)-xy(1:n-1,:);
len = sqrt(sum(df.^2,2));  % length between points
tt = tril(ones(n-1))*len;  % increasing length along curve
tt = [0; tt/tt(n-1)];  % normalized to 0..1

% basis functions
B0 = (1-tt).^3;
B1 = 3*(1-tt).^2 .* tt;
B2 = 3*(1-tt).*tt.^2;
B3 = tt.^3;

% least squares fit min ||sum c_i Bi - xyi||^2
A = [B1'*B1  B1'*B2; B2'*B1 B2'*B2];
b1 = [B0'*B1  B0'*B2; B3'*B1 B3'*B2]*[xy(1,:) ; xy(n,:)];
b2 = [B1'*xy; B2'*xy];
cf = A\(b2-b1);

%compute error 
bb = [xy(1,:) ; cf(1,:); cf(2,:); xy(n,:)];
approx = [B0 B1 B2 B3]*bb;  % points on least-squares fit
df = approx-xy;
err = sqrt(sum(df.^2,2));
% max(err)


figure(2)  
clf;
plot3(xy(:,1),xy(:,2 ),xy(:,3)); hold on;
plot3(approx(:,1),approx(:,2 ), approx(:,3)); hold on;
plot3(bb(:,1),bb(:,2 ), bb(:,3)); hold on;
figure(3)
clf;
plot(tt, err);
