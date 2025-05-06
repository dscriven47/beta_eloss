# beta_eloss
 LET and Range of Electrons in Custom Materials

 This code is a simple ROOT macro that uses Energy Loss tables from estar 
 to calculate the range and stopping power of electrons/beta- in various 
 materials. 

 The estar calculator is found here: 
 https://physics.nist.gov/PhysRefData/Star/Text/ESTAR.html

 Simply save the tabulated data from estar by clicking "Download Data". Change
 the file path in the .C file point at the estar tabulated data.

 do: "root beta_eloss.C"


 The first plot simply shows the stopping power data tabulated in the estar
 data file.

 The second plot shows the range tabulated in the estar data file.

 The third plot shows the stopping power for various initial eletron kinetic
 energies. 

 The fourth plot gives the total energy deposited at a specific depth of
 interaction of the electron for various initial electron kinetic energies.