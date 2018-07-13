clear all
close all
clc

% without buildings
%   plotHexTopology('hex-node-positions.txt', 'hex-connections-trace.txt');

% with buildings
% plotHexTopology('hex-node-positions.txt', 'hex-connections-trace.txt', 'hex-buildings-positions.txt');

% with wrap-around
plotWrapAroundTopology('example-wrap-around-node-positions.txt', 'example-wrap-around-connections.txt', 'example-wrap-around-enbs-positions.txt', 'example-wrap-around-enbs-connections.txt');
