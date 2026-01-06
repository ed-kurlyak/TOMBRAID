# Microsoft Developer Studio Project File - Name="TOMBRAID" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TOMBRAID - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TOMBRAID.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TOMBRAID.mak" CFG="TOMBRAID - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TOMBRAID - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TOMBRAID - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TOMBRAID - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TOMBRAID - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TOMBRAID - Win32 Release"
# Name "TOMBRAID - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Specific\3d_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\3d_line.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\3d_out.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Abortion.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\alligator.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Ape.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\backbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Baldy.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Bat.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Bear.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Blood.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Boat.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\body_part.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Box.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Bridge.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Bubble.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Cabin.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Centaur.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\chain_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Cinema.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Cog.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Collide.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\control_util.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Cowboy.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\crocodile.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\damocles_sword.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\Dart.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\dart_emitter.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Dino.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\dino_stomp.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Door.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\drawprimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\earthquake.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\explosion.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\falling_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\falling_ceiling.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\File.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\finish_level.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\Flame.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Flicker.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Flipmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Flood.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Gameflow.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Gunshot.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Init.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Input.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Inventry.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Invfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Invvars.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Items.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Keyhole.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Lara.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Lara1gun.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Lara2gun.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\lara_effects.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Larafire.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Laramisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Larasurf.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Laraswim.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Larson.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\Lava.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\lightning_emitter.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Lion.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Lot.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\midas_touch.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Missile.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\movable_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Mummy.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Mutant.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Natla.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Option.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\option_compass.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\option_control.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\option_graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\option_passport.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\option_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Overlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\Pendulum.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\People.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Phd_math.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Pickup.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Pierre.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Pod.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Powerup.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\puzzle_hole.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\raising_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Raptor.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Rat.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\requester.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Ricochet.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\rolling_ball.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\rolling_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Sand.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Savegame.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Scalespr.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Scion.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Setup.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\skate_kid.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\Spikes.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\stairs2slope.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Statue.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Switch.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\teeth_trap.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\Traps\thors_hammer.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Trapdoor.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Turn_180.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\Twinkle.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Vars.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Vole.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects\waterfall.cpp
# End Source File
# Begin Source File

SOURCE=.\Specific\Winmain.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Wolf.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Specific\3d_gen.h
# End Source File
# Begin Source File

SOURCE=.\Specific\3d_line.h
# End Source File
# Begin Source File

SOURCE=.\Specific\3d_out.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Abortion.h
# End Source File
# Begin Source File

SOURCE=.\Objects\alligator.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Ape.h
# End Source File
# Begin Source File

SOURCE=.\Specific\backbuffer.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Baldy.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Bat.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Bear.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Blood.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Boat.h
# End Source File
# Begin Source File

SOURCE=.\Effects\body_part.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Box.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Bridge.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Bubble.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Cabin.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Centaur.h
# End Source File
# Begin Source File

SOURCE=.\Effects\chain_block.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Cinema.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Cog.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Collide.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Const.h
# End Source File
# Begin Source File

SOURCE=.\Specific\control_util.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Cowboy.h
# End Source File
# Begin Source File

SOURCE=.\Objects\crocodile.h
# End Source File
# Begin Source File

SOURCE=.\Traps\damocles_sword.h
# End Source File
# Begin Source File

SOURCE=.\Traps\Dart.h
# End Source File
# Begin Source File

SOURCE=.\Traps\dart_emitter.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Dino.h
# End Source File
# Begin Source File

SOURCE=.\Effects\dino_stomp.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Door.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Draw.h
# End Source File
# Begin Source File

SOURCE=.\Specific\drawprimitive.h
# End Source File
# Begin Source File

SOURCE=.\Effects\earthquake.h
# End Source File
# Begin Source File

SOURCE=.\Effects\explosion.h
# End Source File
# Begin Source File

SOURCE=.\Traps\falling_block.h
# End Source File
# Begin Source File

SOURCE=.\Traps\falling_ceiling.h
# End Source File
# Begin Source File

SOURCE=.\Specific\File.h
# End Source File
# Begin Source File

SOURCE=.\Effects\finish_level.h
# End Source File
# Begin Source File

SOURCE=.\Traps\Flame.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Flicker.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Flipmap.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Flood.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Game.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Gameflow.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Gunshot.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Init.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Input.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Inv.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Items.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Keyhole.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Lara.h
# End Source File
# Begin Source File

SOURCE=.\Effects\lara_effects.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Larson.h
# End Source File
# Begin Source File

SOURCE=.\Traps\Lava.h
# End Source File
# Begin Source File

SOURCE=.\Traps\lightning_emitter.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Lion.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Lot.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Traps\midas_touch.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Misc.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Missile.h
# End Source File
# Begin Source File

SOURCE=.\Traps\movable_block.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Mummy.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Mutant.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Natla.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Option.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Overlay.h
# End Source File
# Begin Source File

SOURCE=.\Traps\Pendulum.h
# End Source File
# Begin Source File

SOURCE=.\Specific\People.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Phd_math.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Pickup.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Pierre.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Pod.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Powerup.h
# End Source File
# Begin Source File

SOURCE=.\Objects\puzzle_hole.h
# End Source File
# Begin Source File

SOURCE=.\Effects\raising_block.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Raptor.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Rat.h
# End Source File
# Begin Source File

SOURCE=.\Specific\requester.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Ricochet.h
# End Source File
# Begin Source File

SOURCE=.\Traps\rolling_ball.h
# End Source File
# Begin Source File

SOURCE=.\Traps\rolling_block.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Sand.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Savegame.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Scalespr.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Scion.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Screen.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Setup.h
# End Source File
# Begin Source File

SOURCE=.\Objects\skate_kid.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Sphere.h
# End Source File
# Begin Source File

SOURCE=.\Traps\Spikes.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Effects\stairs2slope.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Statue.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Switch.h
# End Source File
# Begin Source File

SOURCE=.\Traps\teeth_trap.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Text.h
# End Source File
# Begin Source File

SOURCE=.\Traps\thors_hammer.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Timer.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Trapdoor.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Turn_180.h
# End Source File
# Begin Source File

SOURCE=.\Effects\Twinkle.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Types.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Util.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Vars.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Vole.h
# End Source File
# Begin Source File

SOURCE=.\Effects\waterfall.h
# End Source File
# Begin Source File

SOURCE=.\Specific\Winmain.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Wolf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Specific\3d_xouta.s

!IF  "$(CFG)" == "TOMBRAID - Win32 Release"

!ELSEIF  "$(CFG)" == "TOMBRAID - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\Specific\3d_xouta.s
InputName=3d_xouta

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /coff /Fo$(OutDir)\$(InputName) $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
