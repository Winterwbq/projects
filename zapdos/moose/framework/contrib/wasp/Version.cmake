##---------------------------------------------------------------------------##
## wasp/Version.cmake
##---------------------------------------------------------------------------##
# Single file that needs to be changed on a release branch
# or on the development branch in order to configure wasp
# for release mode and set the version.

SET(${PROJECT_NAME}_NAME "wasp")
SET(${PROJECT_NAME}_VERSION 4.4.0)
SET(${PROJECT_NAME}_FULL_VERSION "Workbench Analysis Sequence Processor 4.4.0")
SET(${PROJECT_NAME}_VERSION_STRING "4.4.0")
SET(${PROJECT_NAME}_MAJOR_VERSION 04)
SET(${PROJECT_NAME}_MAJOR_MINOR_VERSION 040400)
SET(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE_DEFAULT ON) # Change to 'OFF' for a release (beta or final)

##---------------------------------------------------------------------------##
##                    end of wasp/Version.cmake
##---------------------------------------------------------------------------##
