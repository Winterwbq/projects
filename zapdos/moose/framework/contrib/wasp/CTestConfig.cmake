##---------------------------------------------------------------------------##
## wasp/CTestConfig.cmake
## Jordan Lefebvre
## Tuesday Janurary 3 17:52:20 2017
##---------------------------------------------------------------------------##

# Must match what is in CDash project 'wasp'
SET(CTEST_NIGHTLY_START_TIME "02:00:00 UTC")
SET(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS 200)
set(CTEST_PROJECT_NAME "wasp")
set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "cdash.ornl.gov")
set(CTEST_DROP_LOCATION "/submit.php?project=WASP")
set(CTEST_TRIGGER_SITE "")
set(CTEST_DROP_SITE_CDASH TRUE)

##---------------------------------------------------------------------------##
##                     end of wasp/CTestConfig.cmake
##---------------------------------------------------------------------------##
