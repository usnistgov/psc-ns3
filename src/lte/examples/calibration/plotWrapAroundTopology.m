function plotWrapAroundTopology(positions_file, connections_file, wr_positions_file, wr_connections_file)
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

wrConnections = 0;
if(exist(wr_connections_file, 'file'))
   size = dir(wr_connections_file);
   if size.bytes ~= 0
   WA = dlmread(wr_connections_file,' '); 
   wrConnections = 1;
   end
end   

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
    set(findobj(gca, 'Type', 'Line', 'Linestyle', '-'), 'LineWidth', 2);
    plot(P(cid,2)+X_vcell, P(cid,3)+Y_vcell,'k');
    %Plot inter-sector's boundaries
    for j=1:3
        X_sb = P(cid,2) + X_3sbounds(j);
        Y_sb = P(cid,3) + Y_3sbounds(j);
        plot([P(cid,2) X_sb], [P(cid,3) Y_sb], 'k');
    end
    
    ueid = A(i,2);
    
if (wrConnections == 1)
    
    index = find(WA(:,1) == ueid);
     if(index ~= 0)
        
       Xw = [WA(index,5), WA(index,3)];
       Yw = [WA(index,6), WA(index,4)];
       kw = mod(WA(index,2)-1,3)+1;
       
        %Plot links eNB----UE
       plot(Xw,Yw, [marker_color(kw) '--']);
       plot(Xw(2),Yw(2), [marker_color(kw) '*']);  
     
     else
        
       X = [P(cid,2), P(ueid+eNBnum,2)];
       Y = [P(cid,3), P(ueid+eNBnum,3)];
    
       %Plot links eNB----UE
       k = mod(cid-1,3)+1;
       plot(X,Y, [marker_color(k) ':']);
       plot(X(2),Y(2), [marker_color(k) '*']);
     end
else
       X = [P(cid,2), P(ueid+eNBnum,2)];
       Y = [P(cid,3), P(ueid+eNBnum,3)];
    
       %Plot links eNB----UE
       k = mod(cid-1,3)+1;
       plot(X,Y, [marker_color(k) ':']);
       plot(X(2),Y(2), [marker_color(k) '*']);
end 
    
    i = i + 1;
end

% eNBs
plot(P(1:eNBnum,2),P(1:eNBnum,3), 'k^');

wrEnbPositions = 0;
if(exist(wr_positions_file, 'file'))
   size = dir(wr_positions_file);
   if size.bytes ~= 0
   P2 = importPositions(wr_positions_file); 
   wrEnbPositions = 1;
   end
end  

if (wrEnbPositions == 1)
w=1;
 while(w<=length(P2))
       plot(P2(w,2)+X_vcell, P2(w,3)+Y_vcell,'k');
       plot(P2(w,2), P2(w,3), 'k^');
       for j=1:3
        X_sb = P2(w,2) + X_3sbounds(j);
        Y_sb = P2(w,3) + Y_3sbounds(j);
        plot([P2(w,2) X_sb], [P2(w,3) Y_sb], 'k');
       end
  w= w + 1;
    
 end

end 

end
