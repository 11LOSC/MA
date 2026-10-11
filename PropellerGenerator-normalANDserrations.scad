// Cted by Hamish Trolove - Feb 2019
//www.techmonkeybusiness.com

// Heavily modified by Maarten van Schagen and Niels Odijk - Nov 2019 + Jun 2020

// Modified to add greater functionality and fixed bug in PitchAdjustment function by Vladimir Smrkolj - Avg 2021

// Bullnose tip rounding added - 2025

//Licensed under a Creative Commons license - attribution
// share alike. CC-BY-SA

//Airfoils:
// Points from naca4412.dat in the But dow archive: http://m-selig.ae.illinois.edu/ads/archives/coord_seligFmt.tar.gz
// Not necessarily in the same order as in: http://m-selig.ae.illinois.edu/ads/coord/naca4412.dat


//////////////////////////////////////////////////////
//////////////  CUSTOMIZABLE OPTIONS  ////////////////
//////////////////////////////////////////////////////

// * = formula adjustable below (which means, the value is adjustable for each calculated slice of the blade)
// ** = advanced formula


////////////////////
/*    [Blades]    */
////////////////////

//Rotation  --  1 is clockwise, 0 is counterclockwise
Clockwise = 0; // [0:CCW, 1:CW]

//Number of blades
BladeNumber = 3;

//pitch* (inches)
PitchInches = 3.5;

//diameter (inches)
DiameterInches = 4.65;

//pitch* (mm)  --  Leave zero to use inches
PitchMM = 0;

//diameter (mm)  --  Leave zero to use inches
DiameterMM = 0;

//Number of sections calculated along the blade  -  Sets resolution
Sections = 90;

//Pitch adjustment factor hub end*
PitchAdjustmentHub = 1.0; // [0.1:0.1:3.0]

//Pitch adjustment factor tip end* - value less than 1 is called washout
PitchAdjustmentTip = 1.0; // [0.1:0.1:3.0]

//Maximum chord length* (mm)  =  Blade width
MaxChordLength = 16;

//Blade sweep at the tip** (deg)  -  -90 < value < 90
BladeSweepFinalAngle = 20;

//Blade thickness* (factor)
ThicknessFactor = 1.0; // [0.1:0.1:5.0]

//Blade thickness at base (factor)
ThicknessFactorAtBase = 3.0; // [0.1:0.1:10.0]

//Length of blade thickening at base (% of Total centerline length)
BaseBladeThickeningLenght = 0.23; // [0.01:0.01:1.0]

//Height of blade tip* (mm)  -  can be negative
BladeTipHeight = 1;

//Blade curvature in z direction*  -  Curve shape: z = x^BladeCurveHeight
BladeCurveHeight = 1.5;  

//Position of blade centerline on chord (%)  -  You can see the effects best when bladesweep is set to zero
CenterlinePosition = 35;

//Bullnose tip radius (mm)  -  Rounds off the blade tip with a quarter-circle taper instead of a flat cut. Set to 0 to get the original flat tip. Increase for a longer, more gradual point.
TipBullnoseRadius = 4; // [0:0.5:20]

//Number of extra slices used to round the tip (higher = smoother rounding)
TipCapSections = 20; // [2:1:30]

//Chordwise position (%) that the tip cap shrinks towards - 100 = trailing edge, 0 = leading edge
TipCapAnchor = 90; // [0:1:100]

//Trailing edge serration height (mm) - how deep each notch cuts chordwise into the blade. 0 = disabled.
SerrationHeight = 0; // [0:0.25:20]

//Trailing edge serration width (mm) - spanwise width of a single tooth
SerrationWidth = 5; // [0.5:0.5:30]

//Number of serrations (teeth), starting at the outer tip (r=1) and going inward
SerrationCount = 0; // [0:1:40]


//Airfoil type at the hub (these 3 numbers represent NACA "MPXX" 4 digit airfoils)
NACA1=[4,4,12];
//Airfoil type at the tip
NACA2=[4,4,12];

//DO NOT CHANGE THESE UNLESS YOU KNOW WHAT YOU'RE DOING
AF_terms= [0.2969,-0.1260,-0.3516,0.2843];
//Fifth term in yt equation. It defines open or closed trailing edge
a4 = -0.1015; // [-0.1036:closed, -0.1015:open]



////////////////////
/*     [Hidden]   */
////////////////////

//Number of vertexes on airfoil crossection. More gives smoother surface
s=30;

/* IGNORE ME */ AirfoilWidth = 1000;
function AirfoilHeight(r) = AirfoilWidth*T_r(r);

/* IGNORE ME */ Pitch = (PitchMM > 0)? (PitchMM):(PitchInches * 25.4);  //Turn pitch values into metric
/* IGNORE ME */ Diameter = (DiameterMM > 0)? (DiameterMM):(DiameterInches * 25.4);  //Turn pitch values into metric

//Blade airfoil profile
//AirfoilPoints = [[1000,1.3],[950,14.7],[900,27.1],[800,48.9],[700,66.9],[600,81.4],[500,91.9],[400,98],[300,97.6],[250,94.1],[200,88],[150,78.9],[100,65.9],[75,57.6],[50,47.3],[25,33.9],[12.5,24.4],[0,0],[12.5,-14.3],[25,-19.5],[50,-24.9],[75,-27.4],[100,-28.6],[150,-28.8],[200,-27.4],[250,-25],[300,-22.6],[400,-18],[500,-14],[600,-10],[700,-6.5],[800,-3.9],[900,-2.2],[950,-1.6],[1000,-1.3]];  //(naca4412 design)


////////////////////
/*     [Hub]      */
////////////////////

//Screw hole diameter (mm)
HubScrewHoleDiameter = 5;

//Hub circle diameter (mm)
HubDiameter = 12;

//Blade connection distance from center (mm)
BladeConnectionDistance = 15;

//Imaginary diameter for guiding hub cone cutting angle (mm)  -  This number should be larger then HubDiameter
HubCutoutDiameter = 20;

//Thickness of the hub (mm)
HubThickness = 5;

//Sag the hub down (mm)
HubSag = 0;

//Diameter of hub pins (mm)  -  Set zero to disable
HubPinDiameter = 0;

//Pitch circle diameter of hub pins (mm)  -  Set zero to disable
HubPinPitchCircleDiameter = 0;



//////////////////////////////////////////////////////
//////////  CUSTOMIZABLE FORMULAS advanced ///////////
//////////////////////////////////////////////////////

//  The formulas below are calculated for every slice of the blade
//  r = length of centerline to point / total length of centerline
//  Diameter = User defined propeller diameter
//  Pitch = User defined propeller pirch


//Function for slope of the centerline (Bladesweep, section yaw)
function CenterlineAngle(r) = BladeSweepFinalAngle*r; // Should be in sync with CenterlineDistance(r)

//Function for distance from the centerline
function CenterlineOffset(r) = BladeSweepFinalAngle != 0 ? -(ln(cos(BladeSweepFinalAngle*r)))/(BladeSweepFinalAngle*PI/180) : 0;  // Is integral of tan(CenterlineAngle(r))

/* IGNORE ME */ CenterlineLength = sqrt(pow(.5*Diameter, 2) / (pow(0.5 * CenterlineOffset(1),2)+.25))/2;

//////////////////////////////////////////////////////
///////////  CUSTOMIZABLE FORMULAS simple ////////////
//////////////////////////////////////////////////////

//  The formulas below are calculated for every slice of the blade
//  r = length of centerline to point / total length of centerline
//  CenterlineLength = Total centerline length = (radius when bladesweep = 0)
//  Pitch = User defined propeller pitch
//  AirfoilWidth = Width of unscaled airfoil shape
//  AirfoilHeight = Height of unscaled airfoil shape
//  
//  To calculate total length of the centerline to a point use r * CenterlineLength

//Blade thickness at base (mm)
function BaseThicknessAdjustment(r) = 1 - (20 * (r - BaseBladeThickeningLenght + 0.1) / (sqrt(pow(20 * (r - BaseBladeThickeningLenght + 0.1), 2) + 1)) - 1) * (ThicknessFactorAtBase - 1); //adjust the thickness at the base of the blade to make the prop stronger

//Blade thickness (mm)
function BladeThickness(r) = ThicknessFactor * BaseThicknessAdjustment(r) * AirfoilHeight(r) * (ChordLength(r)/AirfoilWidth); //Set height based on ChordLength and airfoil designed scaled by the ThicknessFactor

//Height of blade (mm)  //Make sure it goes through (0, 0) to connect it to the hub
function BladeHeight(r) = BladeTipHeight * pow(r, BladeCurveHeight) / CenterlineLength; // Should be in sync with CenterlineDistance(r)

//Blade rol angle (deg) = angle upwards or downwards //This has a very small effect, make this zero if you're not sure how to use this
function RolAngle(r) = atan(BladeTipHeight/CenterlineLength * BladeCurveHeight * pow(r, BladeCurveHeight - 1)); // atan(Derivative of BladeHeight(r))

//The following blade width shape is a function taken from an existing blade.
//It defines the centercord length (mm) (blade width)
function ChordLength(r) = (1.392*pow(r,4) -1.570*pow(r,3)-2.46*pow(r,2)+3.012*r+0.215) * MaxChordLength;

//Function to adjust the pitch angle to match manufactured blades (which are flatter), this function is only used in BladeAngle(r), so you can delete this function if you wish to edit PitchAngle(r) instead
function PitchAngleAdjustment(r) = (PitchAdjustmentTip-PitchAdjustmentHub)*r+PitchAdjustmentHub;

//Blade pitch (deg)
function PitchAngle(r) = atan(Pitch/(2*PI*r*CenterlineLength)) * PitchAngleAdjustment(r);


//////////////////////////////////////////////////////
//////////  Generate airfoil points  ///////////
//////////////////////////////////////////////////////



//linear function for transition from NACA1 to NACA2 airfoil.
function M_r(r) = ((NACA2[0]-NACA1[0])*r+NACA1[0])/100;
function P_r(r) = ((NACA2[1]-NACA1[1])*r+NACA1[1])/10;
function T_r(r) = ((NACA2[2]-NACA1[2])*r+NACA1[2])/100;

//number of x coordinates on chord line for which to caluclate the (x,y) vertex of the blade
AirfoilX = [for (i = [0:s]) 1-(1-cos(i*180/s))/2];

//thickness function for NACA
function yt(x, Tinit) = (Tinit/0.2)*((AF_terms[0]*sqrt(x))+(AF_terms[1]*x)+(AF_terms[2]*x^2)+(AF_terms[3]*x^3)+(a4*x^4));

//Generates a NACA airfoil at each r. The code requires some cleanup if possible
module AirfoilGeneration(r)
{
    Minit=M_r(r);
    Pinit=P_r(r);
    Tinit=T_r(r);
    AirfoilPoints2 = 1000*[for (i = [0:2*len(AirfoilX)-2]) if (i <= len(AirfoilX)-1) (if (AirfoilX[i] < Pinit) [AirfoilX[i] - yt(AirfoilX[i], Tinit)*sin(atan(((2*Minit)/(Pinit^2))*(Pinit-AirfoilX[i]))), (Minit/Pinit^2)*(2*Pinit*AirfoilX[i]-AirfoilX[i]^2) + yt(AirfoilX[i], Tinit)*cos(atan(((2*Minit)/(Pinit^2))*(Pinit-AirfoilX[i])))] else [AirfoilX[i] - yt(AirfoilX[i], Tinit)*sin(atan(((2*Minit)/(1-Pinit)^2)*(Pinit-AirfoilX[i]))), (Minit/(1-Pinit)^2)*(1-2*Pinit+2*Pinit*AirfoilX[i]-AirfoilX[i]^2) + yt(AirfoilX[i], Tinit)*cos(atan(((2*Minit)/(1-Pinit)^2)*(Pinit-AirfoilX[i])))]) else (if (AirfoilX[2*len(AirfoilX)-2-i] < Pinit) [AirfoilX[2*len(AirfoilX)-2-i] + yt(AirfoilX[2*len(AirfoilX)-2-i], Tinit)*sin(atan(((2*Minit)/(Pinit^2))*(Pinit-AirfoilX[2*len(AirfoilX)-2-i]))), (Minit/Pinit^2)*(2*Pinit*AirfoilX[2*len(AirfoilX)-2-i]-AirfoilX[2*len(AirfoilX)-2-i]^2) - yt(AirfoilX[2*len(AirfoilX)-2-i], Tinit)*cos(atan(((2*Minit)/(Pinit^2))*(Pinit-AirfoilX[2*len(AirfoilX)-2-i])))] else [AirfoilX[2*len(AirfoilX)-2-i] + yt(AirfoilX[2*len(AirfoilX)-2-i], Tinit)*sin(atan(((2*Minit)/(1-Pinit)^2)*(Pinit-AirfoilX[2*len(AirfoilX)-2-i]))), (Minit/(1-Pinit)^2)*(1-2*Pinit+2*Pinit*AirfoilX[2*len(AirfoilX)-2-i]-AirfoilX[2*len(AirfoilX)-2-i]^2) - yt(AirfoilX[2*len(AirfoilX)-2-i], Tinit)*cos(atan(((2*Minit)/(1-Pinit)^2)*(Pinit-AirfoilX[2*len(AirfoilX)-2-i])))])];
    polygon(points=AirfoilPoints2); 
}


//The following variable is used as a message that is displayed in the customizer
//Want to customize the formulas used to indicate the shape? You can edit these in the source OpenSCAD code!
//CustomizeFormulas = 0;

//////////////////////////////////////////////////////
/////////////////  PROPELLER DESIGN  /////////////////
//////////////////////////////////////////////////////

// If you want to change the propeller, try editing the modules BladeSlice(r) or Blade()
mergeSlices = true; //Enable or disable merging of slices (helpfull for debugging, set to true for standard renders)

SectionLength =  CenterlineLength/Sections;  //Length of each section

module Hub()
{
    hull() // Attach hull cilinder to blade slice for extra strength
    {
        translate([0,0,-HubSag])cylinder (r = HubDiameter/2, h = HubThickness, center = true, $fn = 100);
       rotate([90,0,0])BladeSlice(BladeConnectionDistance/2 / CenterlineLength);
    }
}

module Hubcutout()
{
    union()
    {
        //Main cutting shape
        cylinder (r = HubScrewHoleDiameter/2, h = HubThickness*10, center = true, $fn = 100);
        translate([0,0,-1.5*HubThickness])cylinder (r2 = HubDiameter/2, r1 = HubCutoutDiameter/2, h = 2*HubThickness, center = true, $fn = 100);
        
        //Pins (if enabled)
        if(HubPinDiameter > 0 && HubPinPitchCircleDiameter > 0)
        {
            translate([0.5*HubPinPitchCircleDiameter,0,0])cylinder(r = 0.5*HubPinDiameter, h = HubThickness*3, center = true, $fn = 100);
            translate([-0.5*HubPinPitchCircleDiameter,0,0])cylinder(r = 0.5*HubPinDiameter, h = HubThickness*3, center = true, $fn = 100);
        }
    }
}

module BladeSlice(r)
{
    PitchAngle = PitchAngle(r);
    ChordLength = ChordLength(r); //Width of blade
    CenterlineAngle = CenterlineAngle(r); //Yaw angle
    Height = BladeHeight(r) * CenterlineLength;
    RolAngle = RolAngle(r);
    CenterlineOffset = CenterlineOffset(r) * CenterlineLength;
    BladeThickness = BladeThickness(r);
    Offset = r*CenterlineLength;
    
    translate([CenterlineOffset, Height, Offset])
    rotate([-RolAngle, CenterlineAngle, -PitchAngle])
    linear_extrude(height=0.00000001, slices = 0)  //Turn into 3d object so we can move and rotate it in 3d
    translate([-CenterlinePosition/100*ChordLength,0])  //Set the centerline in the correct position
    scale([ChordLength/AirfoilWidth, BladeThickness/AirfoilHeight(r)]) //Scale to correct width and height (CentercordLength and BladeThickness are set in mm)
        AirfoilGeneration(r);
}

//A single cross-section for the rounded tip cap.
//Orientation is kept fixed at the r=1 values (this is what gives the cap a smooth, evenly-rounded
//look). Chordwise, the cap shrinks towards TipCapAnchor (near the trailing edge) instead of towards
//CenterlinePosition (near the leading edge), so the round/wide part of the nose sits towards the
//trailing edge. The chordwise pivot shift is eased in with (1-ScaleFactor)^2 instead of linearly,
//so it starts even more gently than the size shrink itself - this keeps the leading edge flowing
//smoothly into the cap instead of visibly kinking where the cap begins.
//t runs from 0 (flat tip, r=1) to 1 (the very point of the rounded nose).
// =========================================================
// KORRIGIERTE TIP-CAP: Simuliert r nahtlos über r=1 hinaus
// =========================================================

// Generiert eine Scheibe, die alle Funktionen (Pitch, Height, Rol, Sweep) 
// kontinuierlich über r = 1.0 hinaus weiterführt!
// =========================================================
// KORRIGIERTE TIP-CAP: Inklusive deinem ShiftFactor!
// =========================================================

// =========================================================
// PERFEKTE KOMBINATION: Dynamisches r + Ankerpunkt-Shift (TipCapAnchor)
// =========================================================

// =========================================================
// PERFEKTE BULLNOSE: Runder Radius + Ankerpunkt + Kein Knick
// =========================================================

// =========================================================
// PERFEKTE KOMBINATION: Tangentialer Übergang (Kein Knick) 
// + Exakte originale Nasenform
// =========================================================

module BladeTipCapSlice(t)
{
    Chord1 = ChordLength(1.0); 
    Thick1 = BladeThickness(1.0);
    CapLength = min(TipBullnoseRadius, Chord1);

    ExtraOffset = sin(t * 100) * CapLength; 
    
    ScaleFactor = cos(t * 90); 
    ShiftFactor = pow(1 - ScaleFactor, 1.2); 
    
    // 1. Position und Orientierung EXAKT an der Kante (r = 1.0)
    PitchAngle = PitchAngle(1.0);
    CenterlineAngle = CenterlineAngle(1.0);
    RolAngle = RolAngle(1.0);
    Height1 = BladeHeight(1.0) * CenterlineLength;
    CenterlineOffset1 = CenterlineOffset(1.0) * CenterlineLength;

    // 2. TANGENTEN-KORREKTUR: Berechne die lokale Steigung des Hauptblattes bei r=1.0,

    // damit die Kappe nahtlos in der gleichen Flucht weiterläuft (eliminiert den Knick!):
    dr = 0.001;
    dHeight = (BladeHeight(1.0) - BladeHeight(1.0 - dr)) * CenterlineLength / dr;
    dOffset = (CenterlineOffset(1.0) - CenterlineOffset(1.0 - dr)) * CenterlineLength / dr;

    // Die Basis-Position wird um die Steigung entlang ExtraOffset verlängert:
    Height_tan = Height1 + dHeight * (ExtraOffset / CenterlineLength);
    CenterlineOffset_tan = CenterlineOffset1 + dOffset * (ExtraOffset / CenterlineLength);
    Offset = CenterlineLength + ExtraOffset;
    Delta = (TipCapAnchor - CenterlinePosition) / 100 * Chord1;

    // 3. Transformation mit tangential geglätteter Position:
    translate([CenterlineOffset_tan, Height_tan, Offset])
    rotate([-RolAngle, CenterlineAngle, -PitchAngle])
    linear_extrude(height=0.00000001, slices = 0)
    translate([Delta * ShiftFactor, 0])               
    scale([ScaleFactor, ScaleFactor])                 
    translate([-CenterlinePosition/100 * Chord1, 0])  
    scale([Chord1/AirfoilWidth, Thick1/AirfoilHeight(1.0)])
        AirfoilGeneration(1.0);
}

module BladeTipCap()
{
    if(TipBullnoseRadius > 0)
    {
        for(i = [0:TipCapSections-1])
        {
            // Sinus-Verteilung für feine Auflösung an der Spitze
            t0 = i / TipCapSections;
            t1 = (i + 1) / TipCapSections;
            hull()
            {
                BladeTipCapSlice(t0);
                BladeTipCapSlice(t1);
            }
        }
    }
}

//A single cutting cross-section at spanwise position r, that removes material from the trailing
//edge inward by 'depth' mm. Built the same way as BladeSlice (same translate/rotate), but with a
//simple rectangle instead of the airfoil, oversized so it always cuts cleanly through the blade.
module SerrationCutSlice(r, depth)
{
    PitchAngle = PitchAngle(r);
    ChordLength = ChordLength(r);
    CenterlineAngle = CenterlineAngle(r);
    Height = BladeHeight(r) * CenterlineLength;
    RolAngle = RolAngle(r);
    CenterlineOffset = CenterlineOffset(r) * CenterlineLength;
    Offset = r*CenterlineLength;
    
    TE_local = ChordLength * (1 - CenterlinePosition/100); //trailing edge position in the local frame
    Margin = 100; //generous margin so the cut always reaches fully through/past the edge
    
    translate([CenterlineOffset, Height, Offset])
    rotate([-RolAngle, CenterlineAngle, -PitchAngle])
    linear_extrude(height=0.00000001, slices = 0)
    translate([TE_local - depth, -Margin])
        square([depth + Margin, 2*Margin]);
}

//Cuts triangular ("Zacken") notches into the trailing edge, starting at the outer tip (r=1) and
//working inward. Each tooth is a V: 0mm depth -> SerrationHeight -> 0mm depth, SerrationWidth mm wide.
//Built as ONE hull() over all three cross-sections per tooth (not two separate halves) - this
//guarantees a watertight cut with no seam/leftover material at the tip, and is cheaper to compute.
module BladeSerrations()
{
    if(SerrationCount > 0 && SerrationHeight > 0)
    {
        for(i = [0:SerrationCount-1])
        {
            OffsetOuter = CenterlineLength - i*SerrationWidth;
            OffsetMid   = CenterlineLength - (i+0.5)*SerrationWidth;
            OffsetInner = CenterlineLength - (i+1)*SerrationWidth;
            
            rOuter = max(OffsetOuter/CenterlineLength, 0.001);
            rMid   = max(OffsetMid/CenterlineLength, 0.001);
            rInner = max(OffsetInner/CenterlineLength, 0.001);
            
            hull()
            {
                SerrationCutSlice(rOuter, 0);
                SerrationCutSlice(rMid, SerrationHeight);
                SerrationCutSlice(rInner, 0);
            }
        }
    }
}

module Blade()
{
    difference()
    {
        union()
        {
            for(i = [0:Sections-1])
            {
                if(mergeSlices){
                    //For standard renders
                    hull(){ //hull the blade slices to fill space in between
                        BladeSlice(i*SectionLength/CenterlineLength);     //Section start
                        BladeSlice((i+1)*SectionLength/CenterlineLength); //Section end
                    }
                }else{
                    //For debugging
                    union(){ //union the blade slices to keep open space in between
                        BladeSlice(i*SectionLength/CenterlineLength);     //Section start
                        BladeSlice((i+1)*SectionLength/CenterlineLength); //Section end
                    }
                }
            }
            BladeTipCap(); //Round off the tip into a bullnose shape
        }
        BladeSerrations(); //Cut the trailing edge teeth, if enabled
    }
}

// Main object
module main(){
    difference()
    {
        union()
        {
            for(i = [0:BladeNumber-1])
            {
                rotate([90,0,i*360/BladeNumber])Blade();
                rotate([0,0,i*360/BladeNumber])Hub();
                //A hub piece is generated for each blade, but they are all merged
            }
        }
        Hubcutout();
    }
}


//Start drawing, mirror if needed
if(!Clockwise){
    mirror([1, 0, 0])main();
}else{
    main();
}
