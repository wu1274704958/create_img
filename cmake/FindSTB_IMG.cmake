message( "finding STB_IMG!"  )

if(WIN32)

    message("Is Windows")
    set(STB_IMG_PATH $ENV{STB_IMG_PATH})
    if( STB_IMG_PATH )

        message("Find STB_IMG_PATH env!")
        message(${STB_IMG_PATH})

        find_path( STB_IMG_INCLUDE_DIR stb.h "${STB_IMG_PATH}" )

        if( STB_IMG_INCLUDE_DIR )

            set( STB_IMG_FOUND TRUE )

        else()

            set( STB_IMG_FOUND FALSE )

        endif()

    else()

        set( STB_IMG_FOUND FALSE )
        message("Not Find STB_IMG_PATH env!")

    endif()

else()

    message("Not Windows!")
    find_path( STB_IMG_INCLUDE_DIR glm "/usr/include" )

    if( STB_IMG_INCLUDE_DIR )

        message("find glm!")
        set( STB_IMG_FOUND TRUE )

    else()

        set( STB_IMG_FOUND FALSE )

    endif()

endif()

message("................................................................")