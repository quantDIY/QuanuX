

add_executable(test_sma test_sma.cpp)
target_link_libraries(test_sma PRIVATE quanux_indicators_lib)
add_test(NAME test_sma COMMAND test_sma)

add_executable(test_market_profile test_market_profile.cpp)
target_link_libraries(test_market_profile PRIVATE quanux_indicators_lib)
add_test(NAME test_market_profile COMMAND test_market_profile)


add_executable(test_volume_profile test_volume_profile.cpp)
target_link_libraries(test_volume_profile PRIVATE quanux_indicators_lib)
add_test(NAME test_volume_profile COMMAND test_volume_profile)

add_executable(test_registry test_registry.cpp)
target_link_libraries(test_registry PRIVATE quanux_indicators_lib)
add_test(NAME test_registry COMMAND test_registry)


