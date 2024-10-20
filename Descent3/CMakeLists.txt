set(HEADERS
  AIGoal.h
  AIMain.h
  BOA.h
  Briefing.h
  BriefingParse.h
  ConfigItem.h
  CtlCfgElem.h
  D3ForceFeedback.h
  DeathInfo.h
  DllWrappers.h
  Inventory.h
  LoadLevel.h
  Mission.h
  NewPyroGauges.h
  ObjScript.h
  PilotPicsAPI.h
  SmallViews.h
  TelCom.h
  TelComAutoMap.h
  TelComCargo.h
  TelComEffects.h
  TelComEfxStructs.h
  TelComGoals.h
  aiambient.h
  aipath.h
  aistruct.h
  aistruct_external.h
  aiterrain.h
  ambient.h
  args.h
  attach.h
  audiotaunts.h
  bnode.h
  bsp.h
  buddymenu.h
  cinematics.h
  cockpit.h
  config.h
  controls.h
  credits.h
  ctlconfig.h
  ctlconfigtxt.h
  d3movie.h
  d3music.h
  damage.h
  damage_external.h
  deathinfo_external.h
  debuggraph.h
  descent.h
  difficulty.h
  difficulty_external.h
  door.h
  doorway.h
  fireball.h
  fireball_external.h
  game.h
  gamecinematics.h
  gamecinematics_external.h
  gameevent.h
  gamefile.h
  gamefont.h
  gameloop.h
  gamepath.h
  gamesave.h
  gamesequence.h
  gamespy.h
  gamespyutils.h
  gametexture.h
  gauges.h
  help.h
  hotspotmap.h
  hud.h
  init.h
  levelgoal.h
  levelgoal_external.h
  lighting.h
  lightmap_info.h
  list.h
  localization.h
  marker.h
  matcen.h
  matcen_external.h
  menu.h
  mission_download.h
  mmItem.h
  multi.h
  multi_client.h
  multi_dll_mgr.h
  multi_external.h
  multi_save_settings.h
  multi_server.h
  multi_ui.h
  multi_world_state.h
  multisafe.h
  newui.h
  newui_core.h
  object.h
  object_external.h
  object_lighting.h
  objinfo.h
  objinit.h
  osiris_dll.h
  osiris_predefs.h
  osiris_share.h
  pilot.h
  pilot_class.h
  player.h
  player_external.h
  player_external_struct.h
  postrender.h
  powerup.h
  procedurals.h
  program.h
  render.h
  renderobject.h
  resource.h
  robot.h
  robotfire.h
  room.h
  room_external.h
  scorch.h
  screens.h
  ship.h
  slew.h
  soar.h
  soar_helpers.h
  sounds.h
  special_face.h
  spew.h
  splinter.h
  stringtable.h
  subtitles.h
  terrain.h
  trigger.h
  vclip.h
  viseffect.h
  viseffect_external.h
  voice.h
  weapon.h
  weapon_external.h
  weather.h)

set(CPPS
  aiambient.cpp
  AIGoal.cpp
  AImain.cpp
  aipath.cpp
  aiterrain.cpp
  ambient.cpp
  args.cpp
  attach.cpp
  audiotaunts.cpp
  bnode.cpp
  BOA.cpp
  Briefing.cpp
  BriefingParse.cpp
  bsp.cpp
  buddymenu.cpp
  cinematics.cpp
  cockpit.cpp
  config.cpp
  ConfigItem.cpp
  Controls.cpp
  credits.cpp
  CtlCfgElem.cpp
  ctlconfig.cpp
  D3ForceFeedback.cpp
  d3movie.cpp
  d3music.cpp
  damage.cpp
  debuggraph.cpp
  dedicated_server.cpp
  demofile.cpp
  descent.cpp
  difficulty.cpp
  DllWrappers.cpp
  door.cpp
  doorway.cpp
  fireball.cpp
  game.cpp
  Game2DLL.cpp
  GameCheat.cpp
  gamecinematics.cpp
  gameevent.cpp
  gamefile.cpp
  gamefont.cpp
  GameLoop.cpp
  gamepath.cpp
  gamesave.cpp
  gamesequence.cpp
  gamespy.cpp
  gamespyutils.cpp
  gametexture.cpp
  gauges.cpp
  help.cpp
  hotspotmap.cpp
  hud.cpp
  huddisplay.cpp
  hudmessage.cpp
  init.cpp
  Inventory.cpp
  levelgoal.cpp
  lighting.cpp
  lightmap_info.cpp
  list.cpp
  LoadLevel.cpp
  loadstate.cpp
  localization.cpp
  marker.cpp
  matcen.cpp
  megacell.cpp
  menu.cpp
  Mission.cpp
  mission_download.cpp
  mmItem.cpp
  multi.cpp
  multisafe.cpp
  multisafe_server.cpp
  multi_client.cpp
  multi_connect.cpp
  multi_dll_mgr.cpp
  multi_save_setting.cpp
  multi_server.cpp
  multi_ui.cpp
  NewPyroGauges.cpp
  newui.cpp
  newui_core.cpp
  newui_filedlg.cpp
  object.cpp
  object_lighting.cpp
  objinfo.cpp
  ObjInit.cpp
  ObjScript.cpp
  OsirisLoadandBind.cpp
  osiris_predefs.cpp
  pilot.cpp
  PilotPicsAPI.cpp
  pilot_class.cpp
  Player.cpp
  postrender.cpp
  procedurals.cpp
  program.cpp
  render.cpp
  renderobject.cpp
  robotfire.cpp
  room.cpp
  scorch.cpp
  screens.cpp
  ship.cpp
  SLEW.cpp
  SmallViews.cpp
  soar.cpp
  soar_helpers.cpp
  special_face.cpp
  spew.cpp
  splinter.cpp
  subtitles.cpp
  TelCom.cpp
  TelComAutoMap.cpp
  TelComCargo.cpp
  TelComEffects.cpp
  TelcomEffectsRender.cpp
  TelComGoals.cpp
  terrain.cpp
  terrainrender.cpp
  TerrainSearch.cpp
  trigger.cpp
  vclip.cpp
  viseffect.cpp
  voice.cpp
  weapon.cpp
  WeaponFire.cpp
  weather.cpp
  sdlmain.cpp)

if(WIN32)
  set(PLATFORM_LIBS wsock32.lib winmm.lib)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO /NODEFAULTLIB:LIBC")

  set(MANIFEST ${CMAKE_CURRENT_BINARY_DIR}/Descent3.exe.manifest)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Descent3.exe.manifest.in
    ${MANIFEST}
    @ONLY
    NEWLINE_STYLE WIN32
  )
  set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/Descent3.rc)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Descent3.rc.in
    ${RC_FILE}
    @ONLY
    NEWLINE_STYLE WIN32
  )
endif()

if(UNIX AND NOT APPLE)
  set(PLATFORM_LIBS m ${CMAKE_DL_LIBS})
endif()

if(APPLE)
  set(PLATFORM_LIBS)
  set(CMAKE_EXE_LINKER_FLAGS "-framework IOKit -framework Cocoa -framework OpenGL -framework Carbon")

  # Set app icon
  set(MACOSX_BUNDLE_ICON_FILE Descent3.icns)
  set(D3Icon ${CMAKE_CURRENT_SOURCE_DIR}/Descent3.icns)
  set_source_files_properties(${D3Icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

file(GLOB_RECURSE INCS "../lib/*.h")

add_executable(Descent3 WIN32 MACOSX_BUNDLE ${D3Icon} ${HEADERS} ${CPPS} ${INCS} ${MANIFEST} ${RC_FILE})
target_link_libraries(Descent3 PRIVATE
  SDL2::SDL2
  2dlib
  AudioEncode
  bitmap
  cfile
  ddebug
  ddio
  fix
  grtext
  libmve
  libacm
  linux
  logger
  manage
  mem
  md5
  misc
  model
  module
  music
  networking
  physics
  renderer
  rtperformance
  sndlib
  stream_audio
  ui
  unzip
  vecmat
  ${PLATFORM_LIBS})
target_include_directories(Descent3 PRIVATE ${PROJECT_BINARY_DIR}/lib)
target_link_options(Descent3 PRIVATE $<$<PLATFORM_ID:Windows>:/DEBUG:FULL>)
add_dependencies(Descent3
  get_git_hash
  Direct_TCP_IP_Hog
  Descent3_Online_TCP_IP_Hog
  HogFull
  Parallax_Online_Hog
  anarchy
  coop
  ctf
  entropy
  hoard
  hyperanarchy
  monsterball
  roboanarchy
  tanarchy
)

# Custom command for copying custom targets such HOG files
# We cannot use here TARGET_FILE_DIR generated expressions since custom target don't have one
add_custom_command(TARGET Descent3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/scripts/$<CONFIG>/d3-${HOG_NAME}.hog" "$<TARGET_FILE_DIR:Descent3>"
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Descent3>/online"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/netcon/descent3onlineclient/$<CONFIG>/Descent3 Online.d3c" "$<TARGET_FILE_DIR:Descent3>/online"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/netcon/lanclient/$<CONFIG>/Direct TCP~IP.d3c" "$<TARGET_FILE_DIR:Descent3>/online"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/netcon/mtclient/$<CONFIG>/Parallax Online.d3c" "$<TARGET_FILE_DIR:Descent3>/online"
  COMMENT "Copying HOG-files into Descent3 work dir"
)

install(TARGETS Descent3 RUNTIME BUNDLE DESTINATION .)
if(MSVC)
  install(FILES $<TARGET_PDB_FILE:Descent3> DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
