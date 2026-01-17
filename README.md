Original project structure by Infrasonic Audio

# Voltages

Eurorack = +/-5v at 0v, 10vpp, 1v/oct
Conversion of 3.6:10 (10 * 0.36) for Eurorack -> Daisy
Daisy = + 3.6v 0v, 3.6vpp, 360mv/oct. Bipolar CV is only possible if jack is normaled to a 1.6v reference, requiring a voltage divider or an output pin. Unipolar + attenuverter is a simpler solution.

A voltage divider could take Eurorack -> Daisy. To go Daisy -> Eurorack you'd have to subtract 1.8v then amplify by 2.78 (ish) if using bipolar CV.