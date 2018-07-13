function plotHexTopology(positions_file, connections_file, buildings_file)
%Plot the topology and connections

%Cell boundaries
theta=0:pi/3:2*pi;
isd=500;
r_cell=isd/2/cos(pi/6);
X_vcell=r_cell*cos(theta);
Y_vcell=r_cell*sin(theta);

%3 inter-sector's boundaries 
r_sector=isd/2;
phi=(pi/2:2*pi/3:2*pi);
X_3sbounds=r_sector*cos(phi);
Y_3sbounds=r_sector*sin(phi);

marker_color='brg';
figure ();
hold on;
set(gcf, 'Color', 'white');
set(gca, 'Box', 'on', 'Xgrid', 'on', 'Ygrid', 'on');
axis square;

P=importPositions(positions_file);
A=importConnections(connections_file);

tableToFindEnbNum = readtable(positions_file);
eNBnum = nnz(strcmp(tableToFindEnbNum.Var1,'eNB'));
UEnum = nnz(strcmp(tableToFindEnbNum.Var1,'UE'));

fprintf('Total number of eNBs =');
disp(eNBnum);
fprintf('Total number of UEs =');
disp(UEnum);


i=1;
while(i<=length(A))
    cid = A(i,1);
    %Plot cell
    plot(P(cid,2)+X_vcell, P(cid,3)+Y_vcell,'k');
    %Plot inter-sector's boundaries
    for j=1:3
        X_sb = P(cid,2) + X_3sbounds(j);
        Y_sb = P(cid,3) + Y_3sbounds(j);
        plot([P(cid,2) X_sb], [P(cid,3) Y_sb], 'k');
    end
    
    ueid = A(i,2);
    X = [P(cid,2), P(ueid+eNBnum,2)];
    Y = [P(cid,3), P(ueid+eNBnum,3)];
    
    %Plot links eNB----UE
    k = mod(cid-1,3)+1;
    plot(X,Y, [marker_color(k) ':']);
    plot(X(2),Y(2), [marker_color(k) '*']);
    
    i = i + 1;
end

% eNBs
plot(P(1:eNBnum,2),P(1:eNBnum,3), 'k^');

if nargin==3 
    
  T = dlmread(buildings_file,' '); 
  fprintf('Total number of buildings =');
  disp(size (T,1));

  for j=1:size (T,1)
        X_sb =[T(j,1) T(j,1) T(j,2) T(j,2) T(j,1)]; 
        Y_sb = [T(j,3) T(j,4) T(j,4) T(j,3) T(j,3)]; 
        plot(X_sb,Y_sb,'r', 'LineWidth',1);
  end
  
end


end
