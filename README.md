# Showerfy
Showerfy v0.0.2 - A VST/AU to simulate my shower.

This is a Impulse Response plugin written for my senior project at CSU Chico.  It makes what is recorded sound like it was made in my shower.  Tested on REAPER, working reasonably well.  

## Installation Instructions
1)  Download the .zip file
2)  For Windows: Copy the .dll file to the appropriate plugin directory.
3)  For OSX: Copy the xxx file to the appropriate plugin directory.
4)  Save the WAVs folder in a memorable place.
5)  Load the WAVs into the plugin when it loads.

## Features
1) A wet/dry control knob to choose how much convlution is allowed.
2) A Pre-Delay knob, with the option of locking the channels.  If channels are unlocked a funky effect happens.  
3) A Shower sound knob, used to mix in me showering.

This is written in C++ using the JUCE framework.
www.juce.com

Also uses the FFTW library
http://www.fftw.org/

## Warning
```
This plugin is still under development!!  There have been (occasional) spikes in volume above 18db.  Take proper precocious.  I am not responsible for any damage/problems that can occur. 
```

## License
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
