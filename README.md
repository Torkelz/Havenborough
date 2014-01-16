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

### Linking DLL ###
Right click the Client project in Visual Studio and click properties.  
Open the page Configuration Properties->Debugging.  
In the field Environment add $(FMOD_DLL_DIR)  