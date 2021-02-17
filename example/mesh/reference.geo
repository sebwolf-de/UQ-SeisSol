/*
 * @author Carsten Uphoff and Duo Li and Sebastian Wolf
 */
 
cl_1 = 5000;
Point(1) = { 5000,  5000, 0,    cl_1};
Point(2) = {-5000,  5000, 0,    cl_1};
Point(3) = {-5000, -5000, 0,    cl_1};
Point(4) = { 5000, -5000, 0,    cl_1};
Point(5) = { 5000,  5000, 5000, cl_1};
Point(6) = {-5000,  5000, 5000, cl_1};
Point(7) = {-5000, -5000, 5000, cl_1};
Point(8) = { 5000, -5000, 5000, cl_1};
Point(20) = {0, 0, 2500, cl_1};

Line(11) = {1, 2};
Line(12) = {2, 3};
Line(13) = {3, 4};
Line(14) = {4, 1};
Line(21) = {5, 6};
Line(22) = {6, 7};
Line(23) = {7, 8};
Line(24) = {8, 5};
Line(31) = {1, 5};
Line(32) = {2, 6};
Line(33) = {3, 7};
Line(34) = {4, 8};


Line Loop(1) = {11,  12,  13,  14};
Plane Surface(1) = {1};
Line Loop(2) = {31,  21, -32, -11};
Plane Surface(2) = {2};
Line Loop(3) = {32,  22, -33, -12};
Plane Surface(3) = {3};
Line Loop(4) = {33,  23, -34, -13};
Plane Surface(4) = {4};
Line Loop(5) = {34,  24, -31, -14};
Plane Surface(5) = {5};
Line Loop(6) = {21,  22,  23,  24};
Plane Surface(6) = {6};

Surface Loop(1) = {1, 2, 3, 4, 5, 6};
Volume(1) = {1};
Physical Surface(101) = {1};
Physical Surface(105) = {2, 3, 4, 5, 6};
Physical Volume(1) = {1};

// define refined box
Field[1] = Box;
Field[1].VIn = 100;
Field[1].VOut = 400;
Field[1].Thickness = 10;
Field[1].XMax = 2000;
Field[1].XMin = -2000;
Field[1].YMax = 2000;
Field[1].YMin = -2000;
Field[1].ZMax = 3000;
Field[1].ZMin = 0;

// define distance to reference point 20
Field[2] = Distance;
Field[2].NodesList = {20};

// define radial refinement method
Field[3] = MathEval;
Field[3].F = "50 + F2^2/200^2*100";

// final 
Field[4] = Min;
Field[4].FieldsList = {1, 3};
Background Field = 4;
// specify gmsh version 
Mesh.MshFileVersion = 2.2;
