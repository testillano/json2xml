function(set_cmake_build_type)

    if ( DEFINED CMAKE_BUILD_TYPE AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )

        if( DEFINED ENV{BUILD_TYPE} )
            message( STATUS
                "Variable BUILD_TYPE ($ENV{BUILD_TYPE}) ignored because "
                "CMAKE_BUILD_TYPE (${CMAKE_BUILD_TYPE}) has higher "
                "precedence."
            )
        endif( DEFINED ENV{BUILD_TYPE} )

    else( DEFINED CMAKE_BUILD_TYPE AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )

        set( build_type "Release" )

        if( DEFINED ENV{BUILD_TYPE} )
            if( $ENV{BUILD_TYPE} STREQUAL ".debug" )
                set( build_type "Debug")
            elseif($ENV{BUILD_TYPE} STREQUAL ".release")
                set( build_type "Release")
            else()
                set( build_type "$ENV{BUILD_TYPE}" )
            endif()
        endif( DEFINED ENV{BUILD_TYPE} )

        set( CMAKE_BUILD_TYPE "${build_type}" PARENT_SCOPE )

    endif( DEFINED CMAKE_BUILD_TYPE AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )

endfunction()
