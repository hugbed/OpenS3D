# - Try to find the BlackMagic DeckLinkAPI
#
#  DECKLINKAPI_FOUND        - system has BlackMagic DeckLinkAPI installed
#  DECKLINKAPI_INCLUDE_DIR  - the include directory containing DeckLinkAPIDispatch.cpp
#  DECKLINKAPI_LIBRARY_DIR  - the directory containing libDeckLinkAPI.so
#

if(UNIX)
  if(APPLE)
    # APPLE
    set(DECKLINKAPI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/mac/decklink_sdk/include")

    find_library( DECKLINKAPI_LIBRARY DeckLinkAPI
        PATHS /usr/lib/
        /usr/local/lib/
        $ENV{DECKLINKAPI_DIR}/lib/ )

  else (APPLE)
    # LINUX
    set(DECKLINKAPI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/linux/decklink_sdk/include")

    find_library( DECKLINKAPI_LIBRARY DeckLinkAPI
        PATHS /usr/lib/
        /usr/local/lib/
        $ENV{DECKLINKAPI_DIR}/lib/ )

  endif(APPLE)
endif(UNIX)

set( DECKLINKAPI_FOUND FALSE )

if ( DECKLINKAPI_INCLUDE_DIR AND DECKLINKAPI_LIBRARY )
  set ( DECKLINKAPI_FOUND TRUE )
endif ( DECKLINKAPI_INCLUDE_DIR AND DECKLINKAPI_LIBRARY )

mark_as_advanced(
    DECKLINKAPI_INCLUDE_DIR
    DECKLINKAPI_LIBRARY
)


