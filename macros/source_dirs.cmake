macro(source_dirs)
  foreach(f ${ARGN})
    set(source_dirs_GLOBALSRCS ${SRCS})
    set(source_dirs_DIRSRCS "")
    
    set(SRCS "")    # Unset SRCS
    add_subdirectory(${f})
    set(SRCS ${source_dirs_GLOBALSRCS})  # Restore SRCS
    
    get_directory_property(source_dirs_DIRSRCS DIRECTORY ${f} DEFINITION SRCS)
  
    foreach(file ${source_dirs_DIRSRCS})      
      # Propogate to parent
      set(SRCS ${SRCS} ${f}/${file})
    endforeach()
  endforeach()
endmacro()