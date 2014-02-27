Havenborough
============

Dependencies
------------
### Boost ###
Download Boost from http://www.boost.org/
Unpack and run the following commands from the extracted folder:

```
bootstrap
b2 --with-system --with-serialization --with-test --with-date_time --with-regex --with-thread --with-filesystem install
```
For better integration with Visual Studio, optionally install the extension Boost Unit Test Adapter (Alpha 5).

### Environment Variables ###
- BOOST_INC_DIR: Path to boost includes, for example 'C:\Boost\include\boost-1_55'
- BOOST_LIB_DIR: Path to boost libs, for example 'C:\Boost\lib'


### FMOD Ex ###
Download FMOD Ex API v.4.44.29 from http://www.fmod.org/  
Install FMOD Ex by running the downloaded .exe file.

### Environment Variables ###
- FMOD_INC_DIR: Path to FMOD includes, for example 'C:\Program Files (x86)\FMOD SoundSystem\FMOD Programmers API Windows\api\inc'
- FMOD_LIB_DIR: Path to FMOD libs, for example 'C:\Program Files (x86)\FMOD SoundSystem\FMOD Programmers API Windows\api\lib'
- FMOD_DLL_DIR: Path to FMOD dll, for example 'C:\Program Files (x86)\FMOD SoundSystem\FMOD Programmers API Windows\api'  

Remember to restart your computer before using the variables!
### Linking DLL ###
Right click the Client project in Visual Studio and click properties.  
Select Debug as Configuration and open the page Configuration Properties->Debugging.  
In the field 'Environment' add '$(SolutionDir)Sound\Test;$(FMOD_DLL_DIR)'  
Switch Configuration to Release, in the field 'Environment' add '$(SolutionDir)Sound\Bin;$(FMOD_DLL_DIR)'  


### VLD ###
Download VLD at http://vld.codeplex.com/releases
Install VLD by running the installer.

### Environment Variables ###
- VLD_INC_DIR: Path to VLD includes, for example 'C:\Program Files (x86)\Visual Leak Detector\include'
- VLD_LIB_DIR: Path to VLD lib, for example 'C:\Program Files (x86)\Visual Leak Detector\lib\Win32'
- VLD_DLL_DIR: Path to VLD dll, for example 'C:\Program Files (x86)\Visual Leak Detector\bin\Win32'

### Settings ###

```
https://vld.codeplex.com/wikipage?title=Using%20Visual%20Leak%20Detector&referringTitle=Documentation
```

- Follow the instructions for visual studio 2012 from the link above.
- After the changes you will have to restart visual studio 2012. You might have to either relog the user or restart the computer.
- Go to the property pages for BoostIntegrationTest. 
- Configuration Properties(Release) -> Debugging.
- In the Command Arguments add '--log_level=message > release_log.txt'. 
- On the same page in the field Working Directory set it to '$(ProjectDir)Bin'.
- Change to Debug configuration. 
- In the Command Arguments add '--log_level=message > debug_log.txt'. 
- On the same page in the field Working Directory set it to '$(ProjectDir)Bin'.


### MahApps.Metro ###
- Start the Havenborough Solution as usual. Set Havenborough Launcher as StartUp Project. (May need to restart Visual Studio afterwards.)
- Press Ctrl+Q (allows you to type in quick launch)
- Type the following: nuget
- Press on NuGet Package Manager Console
- In the console that just popped up type: Install-Package MahApps.Metro
- Once installed you should be able to build the Havenborough Launcher
