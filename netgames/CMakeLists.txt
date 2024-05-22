include_directories("includes")
add_subdirectory(dmfc)
add_subdirectory(anarchy)
add_subdirectory(coop)
add_subdirectory(ctf)
add_subdirectory(entropy)
add_subdirectory(hoard)
add_subdirectory(hyperanarchy)
add_subdirectory(monsterball)
add_subdirectory(roboanarchy)
add_subdirectory(tanarchy)

add_custom_target(NetgamesDir
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:anarchy> "$<TARGET_FILE_DIR:Descent3>/netgames/Anarchy.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:coop> "$<TARGET_FILE_DIR:Descent3>/netgames/Co-op.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ctf> "$<TARGET_FILE_DIR:Descent3>/netgames/CTF.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:entropy> "$<TARGET_FILE_DIR:Descent3>/netgames/Entropy.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hoard> "$<TARGET_FILE_DIR:Descent3>/netgames/Hoard.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hyperanarchy> "$<TARGET_FILE_DIR:Descent3>/netgames/Hyper-Anarchy.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:monsterball> "$<TARGET_FILE_DIR:Descent3>/netgames/Monsterball.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:roboanarchy> "$<TARGET_FILE_DIR:Descent3>/netgames/Robo-Anarchy.d3m"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tanarchy> "$<TARGET_FILE_DIR:Descent3>/netgames/Team Anarchy.d3m"
  COMMENT "Create netgames/ directory"
)
